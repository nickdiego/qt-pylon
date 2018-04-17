#include "pyloncamera.h"

#include <QDateTime>
#include <QDebug>
#include <QImage>
#include <QVideoFrame>
#include <QVideoSurfaceFormat>
#include <QtConcurrent/QtConcurrentRun>

#include <pylon/PylonIncludes.h>

using Pylon::CInstantCamera;
using Pylon::CPylonImage;
using Pylon::CGrabResultPtr;
using Pylon::CImageFormatConverter;
using Pylon::PylonInitialize;
using Pylon::PylonTerminate;
using Pylon::CTlFactory;
using Pylon::PixelType_RGB8packed;
using Pylon::TimeoutHandling_Return;
using Pylon::CImageEventHandler;
using Pylon::RegistrationMode_ReplaceAll;
using Pylon::Cleanup_None;
using Pylon::GrabStrategy_OneByOne;
using Pylon::GrabLoop_ProvidedByInstantCamera;
using Pylon::CFeaturePersistence;
using Pylon::String_t;

static long _frame_counter = 0;

PylonCamera::PylonCamera(QObject *parent) :
    QObject(parent),
    m_surface(nullptr),
    m_camera(nullptr),
    m_startRequested(false)
{
    qRegisterMetaType<QVector<QImage> >("QVector<QImage>");
    PylonInitialize();
    openCamera();
}

PylonCamera::~PylonCamera()
{
    qDebug() << __PRETTY_FUNCTION__;
    stop();

    m_camera->Close();
    m_camera->DestroyDevice();
    delete m_camera;
    m_camera = nullptr;

    PylonTerminate();
}

QString PylonCamera::name() const {
    return m_name;
}

void PylonCamera::setName(const char *name) {
    m_name = QString(name);
    emit nameChanged();
}

QAbstractVideoSurface *PylonCamera::videoSurface() const
{
    return m_surface;
}

void PylonCamera::setVideoSurface(QAbstractVideoSurface *surface)
{
    m_surface = surface;
    emit videoSurfaceChanged();

    if (m_startRequested) {
        start();
    } else {
        stop();
    }
}

bool PylonCamera::isOpen() const
{
    return m_camera != nullptr && m_camera->IsOpen();
}

void PylonCamera::openCamera()
{
    if (isOpen())
        return;

    try {
        m_camera = new CInstantCamera(CTlFactory::GetInstance().CreateFirstDevice());
        setName(m_camera->GetDeviceInfo().GetModelName().c_str());
        qDebug() << "Opening device" << m_name << "..";
        m_camera->Open();

        CFeaturePersistence::SaveToString(m_originalConfig, &m_camera->GetNodeMap());
        qDebug() << "Saved original config: " << m_originalConfig.c_str();

        emit isOpenChanged();
    }
    catch (GenICam::GenericException &e) {
        m_camera = nullptr;
        qWarning() << "Camera Error: " << e.GetDescription();
    }
}

void PylonCamera::stop()
{
	if (!isOpen())
        return;

    stopGrabbing();
    m_startRequested = false;

    emit isOpenChanged();
}

bool PylonCamera::start()
{
    m_startRequested = true;
    openCamera();

    if (!isOpen()) {
        qWarning() << "Failed to open camera!";
        return false;
    }

    if (m_camera->IsGrabbing()) {
        qWarning() << "Camera already started!";
        return true;
    }

    if (!m_surface) {
        qWarning() << "Surface not set. Start pending.";
        return true;
    }

    try {
        restoreOriginalConfig();

        auto v = grabImage();
        CPylonImage img = v.first();
        if (!img.IsValid()) {
            qWarning() << "Failed to get camera format metadata!";
            return false;
        }

        QSize size(img.GetWidth(), img.GetHeight());
        QVideoFrame::PixelFormat f = QVideoFrame::pixelFormatFromImageFormat(QImage::Format_RGB32);
        QVideoSurfaceFormat format(size, f);
        m_surface->start(format);

    }
    catch (GenICam::GenericException &e) {
        m_camera = nullptr;
        qWarning() << "Camera Error: " << e.GetDescription();
    }

    startGrabbing();
    return true;
}

bool PylonCamera::capture(int nFrames, const QString &config)
{
    if (!isOpen()) {
        qWarning() << "Failed to capture: Camera not open!";
        return false;
    }

    stopGrabbing();

    if (!config.isEmpty()) {
        try {
            qDebug() << "Configuring camera [ config.size=" << config.size() << "]";
            String_t strconfig(config.toStdString().c_str());
            CFeaturePersistence::LoadFromString(strconfig, &m_camera->GetNodeMap(), true);
        }
        catch (GenICam::GenericException &e) {
            qWarning() << "Failed to config camera: " << e.GetDescription();
            restoreOriginalConfig();
            return false;
        }
    }

    QtConcurrent::run([this, nFrames]() {
        auto v = grabImage(nFrames);
        QVector<QImage> images(v.size());

        for(int i = 0; i < v.size(); ++i) {
            images[i] = PylonCamera::toQImage(v[i]);
        }
        emit captured(images);
    });
    return true;
}

void PylonCamera::startGrabbing()
{
    if (!isOpen())
        throw std::runtime_error("Camera failed to initialize");

    connect(this, &PylonCamera::frameGrabbedInternal, this, &PylonCamera::renderFrame);

    m_camera->RegisterImageEventHandler(this, RegistrationMode_ReplaceAll, Cleanup_None);
    m_camera->StartGrabbing(GrabStrategy_OneByOne, GrabLoop_ProvidedByInstantCamera);
}

void PylonCamera::stopGrabbing()
{
    if (!isOpen())
        return;

    disconnect(this, &PylonCamera::frameGrabbedInternal, this, &PylonCamera::renderFrame);
    m_camera->DeregisterImageEventHandler(this);

    if (m_camera->IsGrabbing())
        m_camera->StopGrabbing();
}

void PylonCamera::OnImageGrabbed(CInstantCamera& camera, const CGrabResultPtr& ptrGrab)
{
    CImageFormatConverter fc;
    fc.OutputPixelFormat = PixelType_RGB8packed;

    CPylonImage pylonImage;

    fc.Convert(pylonImage, ptrGrab);
    if (!pylonImage.IsValid()) {
        qWarning() << "failed to conver frame" << _frame_counter;
        return;
    }

    QImage qimage = toQImage(pylonImage).convertToFormat(QImage::Format_RGB32);
    emit frameGrabbedInternal(qimage);
}

QImage PylonCamera::toQImage(CPylonImage &pylonImage) {
    int width = pylonImage.GetWidth();
    int height = pylonImage.GetHeight();
    void *buffer = pylonImage.GetBuffer();
    int step = pylonImage.GetAllocatedBufferSize() / height;
    QImage img(static_cast<uchar*>(buffer), width, height, step, QImage::Format_RGB888);
    return img;
}

void PylonCamera::renderFrame(const QImage &img)
{
    if (!m_surface)
        return;

    QVideoFrame frame(img);
    bool r = m_surface->present(frame);
    //qDebug() << "grabbed frame" << _frame_counter++ << r;
}

QVector<CPylonImage> PylonCamera::grabImage(int nFrames)
{
    if (!isOpen())
        throw std::runtime_error("Camera failed to initialize");

    QVector<CPylonImage> images;
    CImageFormatConverter fc;
    fc.OutputPixelFormat = PixelType_RGB8packed;

    CGrabResultPtr ptrGrab;

    m_camera->StartGrabbing(nFrames);

    while(m_camera->IsGrabbing()){
        //qDebug() << "grabbed frame " << _frame_counter++;
        CPylonImage image;
        m_camera->RetrieveResult(1000, ptrGrab, TimeoutHandling_Return);
        if (ptrGrab->GrabSucceeded()) {
            fc.Convert(image, ptrGrab);
        }
        images += image;
    }

    m_camera->StopGrabbing();
    return images;
}

void PylonCamera::restoreOriginalConfig()
{
    qDebug() << "Restoring original camera config [ config.size="
             << m_originalConfig.length() << "]";
    CFeaturePersistence::LoadFromString(m_originalConfig, &m_camera->GetNodeMap());
}

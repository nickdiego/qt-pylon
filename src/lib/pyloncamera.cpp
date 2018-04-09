#include "pyloncamera.h"

#include <QDateTime>
#include <QDebug>
#include <QImage>
#include <QVideoFrame>
#include <QVideoSurfaceFormat>
#include <QStringBuilder>
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

static long _frame_counter = 0;

PylonCamera::PylonCamera(QObject *parent) :
    QObject(parent),
    m_surface(nullptr),
    m_camera(nullptr)
{
    PylonInitialize();
}

PylonCamera::~PylonCamera()
{
    qDebug() << __PRETTY_FUNCTION__;
    stop();
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

	if (m_surface) {
		start();
	} else {
		stop();
	}
}

bool PylonCamera::isOpen() const
{
    return m_camera != nullptr;
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
        emit isOpenChanged();
    }
    catch (GenICam::GenericException &e) {
        m_camera = nullptr;
        qWarning() << "An exception occurred." << e.GetDescription();
    }
}

void PylonCamera::stop()
{
	if (!isOpen())
        return;

    stopGrabbing();
	m_camera->Close();
    m_camera->DestroyDevice();
    m_camera = nullptr;
    emit isOpenChanged();
}

void PylonCamera::start()
{
    openCamera();

    if (!isOpen()) {
        qWarning() << "Failed to open camera!";
        return;
    }

    CPylonImage img;
    grabImage(img);
    if (!img.IsValid()) {
        qWarning() << "Failed to get camera format metadata!";
        return;
    }

    QSize size(img.GetWidth(), img.GetHeight());
    QVideoFrame::PixelFormat f = QVideoFrame::pixelFormatFromImageFormat(QImage::Format_RGB32);
	QVideoSurfaceFormat format(size, f);
	m_surface->start(format);

    startGrabbing();
}

bool PylonCamera::capture()
{
    if (!isOpen()) {
        qWarning() << "Failed to capture: Camera not open!";
        return false;
    }

    stopGrabbing();
    QtConcurrent::run(this, &PylonCamera::captureInternal);
    return true;
}

void PylonCamera::captureInternal() {
    qDebug() << __PRETTY_FUNCTION__;

    CPylonImage pylonImage;
    grabImage(pylonImage);

    const char *f = "dd-MM-yyyy_hh-mm-ss-zzz";
    QString suffix = QDateTime::currentDateTime().toString(f);
    QString path = QString("/tmp/pylon-") % suffix % ".jpg";

    PylonCamera::toQImage(pylonImage).save(path);
    emit imageCaptured(QUrl::fromLocalFile(path));
}

void PylonCamera::startGrabbing()
{
    if (!isOpen())
        throw std::runtime_error("Camera failed to initialize");

    connect(this, SIGNAL(newFrameGrabbed(QImage)), this, SLOT(renderFrame(QImage)));

    m_camera->RegisterImageEventHandler(this, RegistrationMode_ReplaceAll, Cleanup_None);
    m_camera->StartGrabbing(GrabStrategy_OneByOne, GrabLoop_ProvidedByInstantCamera);
}

void PylonCamera::stopGrabbing()
{
    if (!isOpen())
        return;

    disconnect(this, SIGNAL(newFrameGrabbed(QImage)), this, SLOT(renderFrame(QImage)));
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

    QImage img = toQImage(pylonImage);
    emit newFrameGrabbed(img);
}

QImage PylonCamera::toQImage(CPylonImage &pylonImage) {
    int width = pylonImage.GetWidth();
    int height = pylonImage.GetHeight();
    void *buffer = pylonImage.GetBuffer();
    int step = pylonImage.GetAllocatedBufferSize() / height;
    QImage img(static_cast<uchar*>(buffer), width, height, step, QImage::Format_RGB888);
    return img;
}

void PylonCamera::renderFrame(QImage img)
{
    if (m_surface) {
        QImage cimg = img.convertToFormat(QImage::Format_RGB32);
        QVideoFrame frame(cimg);
        bool r = m_surface->present(frame);
        //qDebug() << "grabbed frame" << _frame_counter++ << r;
    }
}

void PylonCamera::grabImage(CPylonImage &image)
{
    if (!isOpen())
        throw std::runtime_error("Camera failed to initialize");

    CImageFormatConverter fc;
    fc.OutputPixelFormat = PixelType_RGB8packed;

    CGrabResultPtr ptrGrab;

    m_camera->StartGrabbing(1);

    while(m_camera->IsGrabbing()){
        qDebug() << "grabbed frame " << _frame_counter++;
        m_camera->RetrieveResult(1000, ptrGrab, TimeoutHandling_Return);
        if (ptrGrab->GrabSucceeded()) {
            fc.Convert(image, ptrGrab);
        }
    }

    m_camera->StopGrabbing();
}


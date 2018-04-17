#ifndef PYLON_CAMERA_H
#define PYLON_CAMERA_H

#include <QAbstractVideoSurface>
#include <QObject>
#include <QString>
#include <QUrl>

#include <pylon/ImageEventHandler.h>
#include <pylon/stdinclude.h>

namespace Pylon {
    class CInstantCamera;
    class CPylonImage;
    class CGrabResultPtr;
}

class PylonCamera : public QObject, public Pylon::CImageEventHandler
{
    Q_OBJECT
    Q_PROPERTY(bool isOpen READ isOpen NOTIFY isOpenChanged)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QAbstractVideoSurface *videoSurface READ videoSurface \
               WRITE setVideoSurface NOTIFY videoSurfaceChanged)

public:
    explicit PylonCamera(QObject *parent = nullptr);
    virtual ~PylonCamera();

    void setVideoSurface(QAbstractVideoSurface *surface);
    QAbstractVideoSurface *videoSurface() const;
    QString name() const;
    bool isOpen() const;

signals:
    void isOpenChanged();
    void nameChanged();
    void videoSurfaceChanged();
    void imageCaptured(const QImage &img);

    // Internal usage only
    // frame will be in Qimge::Format_RGB32
    void frameGrabbedInternal(const QImage &frame);

public slots:
    bool start();
    void stop();
    bool capture(const QString &config = QString());

private slots:
    void renderFrame(const QImage &frame);

private:
    // from Pylon::CImageEventHandler
    virtual void OnImageGrabbed(Pylon::CInstantCamera& camera,
                                const Pylon::CGrabResultPtr& ptrGrab);

    void openCamera();
    void startGrabbing();
    void stopGrabbing();
    void setName(const char *name);

    void grabImage(Pylon::CPylonImage &image);
    static QImage toQImage(Pylon::CPylonImage &pylonImage);
    void restoreOriginalConfig();

private:
    QAbstractVideoSurface *m_surface;
    Pylon::CInstantCamera *m_camera;
    bool m_startRequested;
    QString m_name;
    Pylon::String_t m_originalConfig;
};

#endif // PYLON_CAMERA_H

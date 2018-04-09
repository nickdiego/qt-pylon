#ifndef PYLON_CAMERA_H
#define PYLON_CAMERA_H

#include <QObject>
#include <QString>
#include <QAbstractVideoSurface>

#include <pylon/ImageEventHandler.h>

namespace Pylon {
    class CInstantCamera;
    class CPylonImage;
    class CGrabResultPtr;
}

namespace core {

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
    void newFrameGrabbed(QImage frame);
    void imageCaptured(QString path);

public slots:
    void start();
    void stop();
    bool capture();

private slots:
    void renderFrame(QImage frame);

private:
    // from Pylon::CImageEventHandler
    virtual void OnImageGrabbed(Pylon::CInstantCamera& camera,
                                const Pylon::CGrabResultPtr& ptrGrab);

    void openCamera();
    void startGrabbing();
    void stopGrabbing();
    void setName(const char *name);
    void captureInternal();

    void grabImage(Pylon::CPylonImage &image);
    static QImage toQImage(Pylon::CPylonImage &pylonImage);

private:
    QAbstractVideoSurface *m_surface;
    QTimer *m_timer;
    Pylon::CInstantCamera *m_camera;
    QString m_name;
};

} // namespace core

#endif // PYLON_CAMERA_H

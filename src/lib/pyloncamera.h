#ifndef PYLON_CAMERA_H
#define PYLON_CAMERA_H

#include <QAbstractVideoSurface>
#include <QObject>
#include <QSize>
#include <QString>
#include <QUrl>
#include <QVector>

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

    void open();
    void setVideoSurface(QAbstractVideoSurface *surface);
    QAbstractVideoSurface *videoSurface() const;
    QString name() const;
    bool isOpen() const;
    void setConfig(const QString& configStr);

signals:
    void isOpenChanged();
    void nameChanged();
    void videoSurfaceChanged();
    void captured(const QVector<QImage> &imgs);

    // Internal usage only
    // frame will be in Qimge::Format_RGB32
    void frameGrabbedInternal(const QImage &frame);

public slots:
    bool start();
    void stop();
    bool capture(int n = 1, const QString &config = QString());

private slots:
    void renderFrame(const QImage &frame);

private:
    // from Pylon::CImageEventHandler
    virtual void OnImageGrabbed(Pylon::CInstantCamera& camera,
                                const Pylon::CGrabResultPtr& ptrGrab);

    void startGrabbing();
    void stopGrabbing();
    void setName(const char *name);

    QVector<Pylon::CPylonImage> grabImage(int n = 1);
    QImage toQImage(Pylon::CPylonImage &pylonImage);
    void restoreOriginalConfig();

private:
    QAbstractVideoSurface *m_surface;
    Pylon::CInstantCamera *m_camera;
    QSize m_size;
    bool m_startRequested;
    QString m_name;
    Pylon::String_t m_config;
};

#endif // PYLON_CAMERA_H

#ifndef AUDIOSTREAMRECEIVER_H
#define AUDIOSTREAMRECEIVER_H

#include <QObject>
#include <QAudioDevice>
#include <QMediaDevices>
#include <QAudioFormat>
#include <QAudioSource>
#include <QIODevice>

class AudioStreamReceiver : public QIODevice
{
    Q_OBJECT

  public:
    explicit AudioStreamReceiver(QObject *parent = nullptr)
        : QIODevice(parent)
    {
        open(QIODevice::WriteOnly);
    }

  signals:
    void SignalAudioCaptured(const QByteArray &data);

  protected:
    qint64 writeData(const char *data, qint64 len) override
    {
        emit SignalAudioCaptured(QByteArray(data, len));
        return len;
    }

    qint64 readData(char *data, qint64 maxlen) override
    {
        Q_UNUSED(data);
        Q_UNUSED(maxlen);
        return 0;
    }
};

#endif // AUDIOSTREAMRECEIVER_H
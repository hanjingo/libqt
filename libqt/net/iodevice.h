#ifndef IODEVICE_H
#define IODEVICE_H

#include <QByteArray>
#include <QQueue>
#include <QIODevice>
#include <QMutex>
#include <QTimer>

class IODevice : public QObject
{
    Q_OBJECT

public:
    explicit IODevice(QObject *parent = NULL);
    ~IODevice();

signals:
    void produce(const QByteArray array);
    void produce(const QByteArrayList array);
    void consume(const QByteArray array);
    void consume(const QByteArrayList array);

private slots:
    void read();
    qint64 write(const QByteArray array);
    qint64 write(const QByteArrayList arrayList);

private:
    QIODevice* m_device = nullptr;

};

#endif

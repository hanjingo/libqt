#ifndef ZIPPER_H
#define ZIPPER_H

#include <QtGlobal>
#include <QtCore/private/qzipreader_p.h>

#include <QDir>
#include <QDebug>

class Zipper : public QObject
{
    Q_OBJECT
  public:
    explicit Zipper(const QString &zipFilePath, QObject *parent = nullptr)
        : QObject(parent)
        , _reader(zipFilePath)
    {
    }
    ~Zipper() override { _reader.close(); }

    bool UnZip(const QString &destDir)
    {
        if(!_reader.isReadable())
        {
            emit SignalUnZipFinished(false, destDir);
            return false;
        }

        QDir dir(destDir);
        if(!dir.exists())
            dir.mkpath(destDir);

        if(!_reader.extractAll(destDir))
        {
            emit SignalUnZipFinished(false, destDir);
            return false;
        }

        emit SignalUnZipFinished(true, destDir);
        return true;
    }

  signals:
    void SignalUnZipFinished(bool success, const QString &destDir);

  private:
    QZipReader _reader;
};

#endif // ZIPPER_H
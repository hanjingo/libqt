#ifndef ZIPPER_H
#define ZIPPER_H

#include <QtGlobal>
#include <QtCore/private/qzipreader_p.h>
#include <QtCore/private/qzipwriter_p.h>

#include <QDir>
#include <QDebug>

class Zipper : public QObject
{
    Q_OBJECT
  public:
    explicit Zipper(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
    ~Zipper() = default;

    bool zip(const QString &destFile, const QStringList &srcFiles)
    {
        QZipWriter writer(destFile);
        if(writer.status() != QZipWriter::NoError)
        {
            emit SignalZipFinished(false, destFile);
            return false;
        }

        writer.setCompressionPolicy(QZipWriter::AutoCompress);
        for(const QString &filePath : srcFiles)
        {
            QFileInfo fileInfo(filePath);
            if(!fileInfo.exists())
            {
                emit SignalZipFinished(false, destFile);
                return false;
            }

            if(fileInfo.isDir())
            {
                if(!addDirectory(writer, filePath, fileInfo.fileName()))
                {
                    emit SignalZipFinished(false, destFile);
                    return false;
                }
            } else
            {
                if(!addFile(writer, filePath, fileInfo.fileName()))
                {
                    emit SignalZipFinished(false, destFile);
                    return false;
                }
            }
        }

        writer.close();
        if(writer.status() != QZipWriter::NoError)
        {
            emit SignalZipFinished(false, destFile);
            return false;
        }

        emit SignalZipFinished(true, destFile);
        return true;
    }

    bool unZip(const QString &destDir, const QString &zipFile)
    {
        QZipReader reader(zipFile);
        if(!reader.isReadable())
        {
            emit SignalUnZipFinished(false, destDir);
            return false;
        }

        QDir dir(destDir);
        if(!dir.exists())
            dir.mkpath(destDir);

        if(!reader.extractAll(destDir))
        {
            emit SignalUnZipFinished(false, destDir);
            return false;
        }

        emit SignalUnZipFinished(true, destDir);
        return true;
    }

  signals:
    void SignalZipFinished(bool success, const QString &destFile);
    void SignalUnZipFinished(bool success, const QString &destDir);

  private:
    bool addFile(QZipWriter    &writer,
                 const QString &filePath,
                 const QString &fileNameInZip)
    {
        QFile file(filePath);
        if(!file.open(QIODevice::ReadOnly))
        {
            qWarning() << "Cannot open file:" << filePath;
            return false;
        }

        QByteArray data = file.readAll();
        file.close();

        writer.addFile(fileNameInZip, data);
        return true;
    }

    bool addDirectory(QZipWriter    &writer,
                      const QString &dirPath,
                      const QString &dirNameInZip)
    {
        QDir dir(dirPath);
        writer.addDirectory(dirNameInZip);
        QFileInfoList entries =
            dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);
        for(const QFileInfo &entry : entries)
        {
            QString entryPath      = entry.absoluteFilePath();
            QString entryNameInZip = dirNameInZip + "/" + entry.fileName();
            if(entry.isDir())
            {
                if(!addDirectory(writer, entryPath, entryNameInZip))
                    return false;
            } else
            {
                if(!addFile(writer, entryPath, entryNameInZip))
                    return false;
            }
        }

        return true;
    }
};

#endif // ZIPPER_H
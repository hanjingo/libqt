#ifndef FILE_H
#define FILE_H

#include <QObject>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QCryptographicHash>
#include <QDirIterator>

class File : public QFile
{
  public:
    File(const QString &filePath)
        : QFile(filePath)
    {
    }

    ~File() override
    {
        if(isOpen())
            close();
    }

  public:
    static bool isFile(const QString &filePath)
    {
        QFileInfo info(filePath);
        return info.isFile();
    }

    static bool isExist(const QString &filePath)
    {
        QFile file(filePath);
        return file.exists();
    }

    static qint64 fileSize(const QString &filePath)
    {
        QFile file(filePath);
        if(!file.open(QIODevice::ReadOnly))
            return -1;

        return file.size();
    }

    static qint64 fileSizeKB(const QString &filePath)
    {
        QFile file(filePath);
        if(!file.open(QIODevice::ReadOnly))
            return -1;

        return file.size() / 1024;
    }

    static QString md5(const QString &filePath)
    {
        QFile file(filePath);
        if(!file.open(QIODevice::ReadOnly))
            return "";

        QCryptographicHash hash(QCryptographicHash::Md5);
        if(!hash.addData(&file))
            return "";

        return hash.result().toHex();
    }

    static void walk(const QString                        &filePath,
                     std::function<bool(QFileInfo &, int)> cb,
                     bool                                  recursive = false)
    {
        if(!cb)
            return;

        QFileInfo fileInfo(filePath);
        if(!fileInfo.exists())
            return;

        if(fileInfo.isFile())
        {
            cb(fileInfo, 0);
            return;
        }

        if(fileInfo.isDir())
        {
            QStringList                 nameFilters;
            QDirIterator::IteratorFlags flags =
                recursive ? QDirIterator::Subdirectories
                          : QDirIterator::NoIteratorFlags;

            QDirIterator it(filePath,
                            nameFilters,
                            QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot
                                | QDir::NoSymLinks | QDir::Readable,
                            flags);

            int baseDepth = filePath.count('/');
            while(it.hasNext())
            {
                it.next();
                QFileInfo currentInfo = it.fileInfo();
                int       depth       = 0;
                if(recursive)
                    depth =
                        currentInfo.absoluteFilePath().count('/') - baseDepth;

                if(!cb(currentInfo, depth))
                    break;
            }
        }
    }

    static qint64
    findSentenceBoundary(const QString &text, qint64 start, qint64 end)
    {
        QVector<QChar> sentenceEnds = {'.', '!', '?', ';'};
        for(int i = end - 1; i > start; --i)
        {
            QChar ch = text[i];
            if(!sentenceEnds.contains(ch))
                continue;

            if(i + 1 < text.length())
            {
                if(text[i + 1].isSpace())
                    return i + 2;

                if(text[i + 1] == '"' || text[i + 1] == '\''
                   || text[i + 1] == ')' || text[i + 1] == ']')
                {
                    if(i + 2 < text.length() && text[i + 2].isSpace())
                        return i + 3;
                }
            }

            if(i + 1 == text.length())
                return i + 1;
        }
        return end;
    }

    static qint64
    findParagraphBoundary(const QString &text, qint64 start, qint64 end)
    {
        for(int i = end - 1; i > start; --i)
        {
            if(i + 3 < text.length() && text[i] == '\r' && text[i + 1] == '\n'
               && text[i + 2] == '\r' && text[i + 3] == '\n')
                return i + 4;

            if(i + 1 < text.length() && text[i] == '\n' && text[i + 1] == '\n')
                return i + 2;

            if(i + 2 < text.length() && text[i] == '\n' && text[i + 1] == '\r'
               && text[i + 2] == '\n')
                return i + 3;
        }
        return end;
    }
};

#endif // FILE_H
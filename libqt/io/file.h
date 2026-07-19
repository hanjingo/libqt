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

#ifdef Q_OS_WIN
#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#endif // windows

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

    static bool createIfNotExists(const QString &filePath)
    {
        QFile file(filePath);
        if(file.exists())
            return true;

        return file.open(QIODevice::WriteOnly);
    }

    static bool removeIfExists(const QString &filePath, bool force = true)
    {
        QFileInfo info(filePath);
        if(!info.exists())
            return true;

        // for dir
        if(info.isDir())
        {
            QDir dir(filePath);
            return dir.removeRecursively();
        }

        // for file
        QFile file(filePath);
        if(force)
        {
            file.setPermissions(QFile::ReadOwner | QFile::WriteOwner
                                | QFile::ReadUser | QFile::WriteUser
                                | QFile::ReadGroup | QFile::WriteGroup
                                | QFile::ReadOther | QFile::WriteOther);

            if(file.remove())
                return true;

            if(file.moveToTrash())
                return true;

#ifdef Q_OS_WIN
            // On Windows, if the file is in use, we can try to rename it to a temporary name and then delete it later.
            QString      nativePath = QDir::toNativeSeparators(filePath);
            std::wstring wPath      = nativePath.toStdWString();

            QString tmpPath =
                filePath + "."
                + QUuid::createUuid().toString(QUuid::WithoutBraces)
                + ".deleted";
            std::wstring wTmpPath =
                QDir::toNativeSeparators(tmpPath).toStdWString();

            if(MoveFileExW(wPath.c_str(),
                           wTmpPath.c_str(),
                           MOVEFILE_REPLACE_EXISTING))
            {
                MoveFileExW(wTmpPath.c_str(),
                            NULL,
                            MOVEFILE_DELAY_UNTIL_REBOOT);
                return true;
            }
#endif
        }

        return file.remove();
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
#ifndef FILECHUNKER_H
#define FILECHUNKER_H

#include <QString>
#include <QVector>
#include <QFile>
#include <QTextStream>

#include <functional>

class FileChunker
{
  public:
    struct Chunk
    {
        int64_t    id = 0;
        QByteArray data;
        int64_t    startPos = 0;
        int64_t    offset   = 0;
        QString    filePathName;
        QString    timestamp;
    };

    using ChunkCallback = std::function<bool(Chunk &)>;

  public:
    FileChunker()  = default;
    ~FileChunker() = default;

    qint64 chunkText(const QString       &text,
                     const qint64         chunkSize,
                     const ChunkCallback &cb,
                     const QString       &filePathName = QString())
    {
        if(text.isEmpty())
            return 0;

        m_totalChunks   = 0;
        qint64 totalLen = text.length();
        Chunk  chunk;
        chunk.startPos     = 0;
        chunk.id           = 0;
        chunk.offset       = qMin(chunkSize, totalLen);
        chunk.filePathName = filePathName;
        while(cb(chunk))
        {
            chunk.offset = qMin(chunkSize, totalLen - chunk.startPos);
            if(chunk.offset < 1)
                break;

            if(chunk.startPos >= totalLen)
                break;

            chunk.data = text.mid(chunk.startPos, chunk.offset).toUtf8();
            m_totalChunks++;
        }

        return m_totalChunks;
    }

    qint64 chunkFile(const QString       &filePath,
                     const qint64         chunkSize,
                     const ChunkCallback &cb)
    {
        QFile file(filePath);
        if(!file.open(QIODevice::ReadOnly))
            return -1;

        QTextStream stream(&file);
        stream.setEncoding(QStringConverter::Encoding::Utf8);
        QString content = stream.readAll();
        file.close();
        return chunkText(content, chunkSize, cb, filePath);
    }

    qint64 chunkFiles(const QStringList   &filePaths,
                      const qint64         chunkSize,
                      const ChunkCallback &cb)
    {
        qint64 totalChunks = 0;
        for(const QString &filePath : filePaths)
        {
            qint64 chunks = chunkFile(filePath, chunkSize, cb);
            if(chunks > 0)
                totalChunks += chunks;
        }

        return totalChunks;
    }

  private:
    qint64 m_totalChunks = 0;
};

#endif // FILECHUNKER_H
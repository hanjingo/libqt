#ifndef SHA_H
#define SHA_H

#include <QCryptographicHash>

class SHA256
{
  public:
    static QByteArray hashFile(const QString &filePath)
    {
        QFile file(filePath);
        if(!file.open(QIODevice::ReadOnly))
            return QByteArray();

        QCryptographicHash hash(QCryptographicHash::Sha256);
        if(!hash.addData(&file))
            return QByteArray();

        return hash.result();
    }
};

#endif // SHA_H
#include "uploader.h"

#include <QDebug>
#include <QFileInfo>
#include <QHttpMultiPart>
#include <QHttpPart>

Uploader::Uploader(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_reply(nullptr)
    , m_file(nullptr)
    , m_multiPart(nullptr)
    , m_isUploading(false)
{
}

Uploader::~Uploader()
{
    if(m_reply)
        m_reply->abort();

    cleanupReply();
    cleanupMultiPart();
    m_networkManager->deleteLater();
}

bool Uploader::upload(const QUrl                   &url,
                      const QString                &filePath,
                      const QString                &fieldName,
                      const QMap<QString, QString> &extraFields)
{
    if(m_isUploading)
    {
        qDebug() << "Upload already in progress, cancel first or wait";
        emit signalUploadError("Upload already in progress");
        return false;
    }

    cleanupReply();
    cleanupMultiPart();
    m_filePath = filePath;
    QFileInfo fileInfo(filePath);
    if(!fileInfo.exists())
    {
        qDebug() << "File does not exist:" << filePath;
        emit signalUploadError(
            QString("File does not exist: %1").arg(filePath));
        emit signalUploadFinished(false, filePath, QString());
        return false;
    }

    if(!fileInfo.isFile())
    {
        qDebug() << "Path is not a file:" << filePath;
        emit signalUploadError(QString("Path is not a file: %1").arg(filePath));
        emit signalUploadFinished(false, filePath, QString());
        return false;
    }

    m_file = new QFile(filePath);
    if(!m_file->open(QIODevice::ReadOnly))
    {
        qDebug() << "Unable to open file:" << filePath << "for reading";
        emit signalUploadError(
            QString("Unable to open file: %1").arg(filePath));
        emit signalUploadFinished(false, filePath, QString());
        delete m_file;
        m_file = nullptr;
        return false;
    }

    m_multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType, this);

    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader,
                       QVariant("application/octet-stream"));
    filePart.setHeader(
        QNetworkRequest::ContentDispositionHeader,
        QVariant(QString("form-data; name=\"%1\"; filename=\"%2\"")
                     .arg(fieldName)
                     .arg(QString::fromUtf8(
                         QUrl::toPercentEncoding(fileInfo.fileName())))));
    filePart.setBodyDevice(m_file);

    m_file->setParent(m_multiPart);
    m_multiPart->append(filePart);

    for(auto it = extraFields.begin(); it != extraFields.end(); ++it)
    {
        QHttpPart textPart;
        textPart.setHeader(
            QNetworkRequest::ContentDispositionHeader,
            QVariant(QString("form-data; name=\"%1\"").arg(it.key())));
        textPart.setBody(it.value().toUtf8());
        m_multiPart->append(textPart);
    }

    QNetworkRequest request(url);
    m_reply       = m_networkManager->post(request, m_multiPart);
    m_isUploading = true;

    connect(m_reply,
            &QNetworkReply::uploadProgress,
            this,
            &Uploader::onUploadProgress);
    connect(m_reply, &QNetworkReply::finished, this, &Uploader::onFinished);
    connect(m_reply,
            &QNetworkReply::errorOccurred,
            this,
            &Uploader::onErrorOccurred);

    qDebug() << "Upload started:" << filePath << "->" << url.toString();
    return true;
}

void Uploader::cancel()
{
    if(m_reply)
    {
        qDebug() << "Upload cancelled:" << m_filePath;
        m_reply->abort();
    } else
    {
        cleanupReply();
        cleanupMultiPart();
        m_isUploading = false;
    }
}

void Uploader::onUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    if(bytesTotal > 0)
    {
        double progress = static_cast<double>(bytesSent) / bytesTotal * 100.0;
        emit   signalUploadProgress(static_cast<int>(progress));
    }
}

void Uploader::onFinished()
{
    m_isUploading = false;

    if(!m_reply)
    {
        cleanupReply();
        cleanupMultiPart();
        return;
    }

    QString response;
    bool    success = false;

    if(m_reply->error() == QNetworkReply::NoError)
    {
        response = QString::fromUtf8(m_reply->readAll());
        qDebug() << "Upload finished successfully:" << m_filePath;
        qDebug() << "Response:" << response;
        success = true;
        emit signalUploadFinished(true, m_filePath, response);
    } else
    {
        qDebug() << "Upload error:" << m_reply->errorString();
        emit signalUploadFinished(false, m_filePath, QString());
    }

    cleanupReply();
    cleanupMultiPart();
}

void Uploader::onErrorOccurred(QNetworkReply::NetworkError code)
{
    Q_UNUSED(code)
    if(m_reply)
    {
        qDebug() << "Upload network error:" << m_reply->errorString();
        emit signalUploadError(m_reply->errorString());
    }
}

void Uploader::cleanupReply()
{
    if(m_reply)
    {
        disconnect(m_reply,
                   &QNetworkReply::uploadProgress,
                   this,
                   &Uploader::onUploadProgress);
        disconnect(m_reply,
                   &QNetworkReply::finished,
                   this,
                   &Uploader::onFinished);
        disconnect(m_reply,
                   &QNetworkReply::errorOccurred,
                   this,
                   &Uploader::onErrorOccurred);
        m_reply->deleteLater();
        m_reply = nullptr;
    }
    m_isUploading = false;
}

void Uploader::cleanupMultiPart()
{
    if(m_multiPart)
    {
        m_multiPart->deleteLater();
        m_multiPart = nullptr;
    }

    if(m_file && m_file->parent() != m_multiPart)
    {
        m_file->deleteLater();
    }
    m_file = nullptr;
}
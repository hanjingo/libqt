#ifndef UPLOADER_H
#define UPLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QUrl>
#include <QHttpMultiPart>

class Uploader : public QObject
{
    Q_OBJECT
  public:
    explicit Uploader(QObject *parent = nullptr);
    ~Uploader() override;

    bool upload(
        const QUrl                   &url,
        const QString                &filePath,
        const QString                &fieldName   = "files",
        const QMap<QString, QString> &extraFields = QMap<QString, QString>());
    void cancel();
    bool isUploading() const { return m_isUploading; }

  signals:
    void signalUploadProgress(int progress);
    void signalUploadFinished(bool           success,
                              const QString &filePath,
                              const QString &response);
    void signalUploadError(const QString &errorString);

  private slots:
    void onUploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void onFinished();
    void onErrorOccurred(QNetworkReply::NetworkError code);

  private:
    void cleanupReply();
    void cleanupMultiPart();

  private:
    QNetworkAccessManager *m_networkManager;
    QNetworkReply         *m_reply;
    QFile                 *m_file;
    QHttpMultiPart        *m_multiPart;
    QString                m_filePath;
    bool                   m_isUploading;
};

#endif // UPLOADER_H
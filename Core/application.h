#ifndef APPLICATION_H
#define APPLICATION_H

#include <QCoreApplication>

class Application : public QCoreApplication
{
    Q_OBJECT
public:
    Application(int& argc, char** argv);
    ~Application();

    void setAppName(const QString& name);
    QString getAppName() const;
    void setAppVersion(const QString& version);
    QString getAppVersion() const;
    void setAppAuthor(const QString& author);
    QString getAppAuthor() const;
    void setAppCopyright(const QString& copyright);
    QString getAppCopyright() const;
    void setAppLicense(const QString& license);
    QString getAppLicense() const;
    void setAppDescription(const QString& description);
    QString getAppDescription() const;    
};

#endif
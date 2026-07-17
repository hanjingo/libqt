#ifndef JSON_H
#define JSON_H

#include <QObject>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class JSON
{
  public:
    JSON()  = default;
    ~JSON() = default;

  public:
    static bool writeFile(const QString &filePath, const QJsonDocument &doc)
    {
        QFile saveFile(filePath);
        if(!saveFile.open(QIODevice::WriteOnly | QIODevice::Text))
            return false;

        saveFile.write(doc.toJson());
        return true;
    }

    static bool writeFile(const QString &filePath, const QJsonObject &jsonObj)
    {
        QJsonDocument doc(jsonObj);
        QFile         saveFile(filePath);
        if(!saveFile.open(QIODevice::WriteOnly | QIODevice::Text))
            return false;

        saveFile.write(doc.toJson());
        return true;
    }

    static bool writeFile(const QString &filePath, const QJsonArray &jsonArr)
    {
        QJsonDocument doc(jsonArr);
        QFile         saveFile(filePath);
        if(!saveFile.open(QIODevice::WriteOnly | QIODevice::Text))
            return false;

        saveFile.write(doc.toJson());
        return true;
    }

    static bool readFile(QJsonDocument &doc, const QString &filePath)
    {
        QFile file(filePath);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return false;

        QByteArray data = file.readAll();
        doc             = QJsonDocument::fromJson(data);
        return !doc.isNull();
    }

    static bool readFile(QJsonObject &jsonObj, const QString &filePath)
    {
        QJsonDocument doc;
        if(!readFile(doc, filePath))
            return false;

        if(!doc.isObject())
            return false;

        jsonObj = doc.object();
        return true;
    }

    static bool readFile(QJsonArray &jsonArr, const QString &filePath)
    {
        QJsonDocument doc;
        if(!readFile(doc, filePath))
            return false;

        if(!doc.isArray())
            return false;

        jsonArr = doc.array();
        return true;
    }
};

#endif // JSON_H
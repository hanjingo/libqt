#ifndef CRASH_H
#define CRASH_H

#include <QString>
#include <string>

class StrUtil
{
public:
	static std::string toStdString(const QString& str)
	{
		return std::string(str.toUtf8().constData());
	}
};

#endif

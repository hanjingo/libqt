#ifndef CRASH_H
#define CRASH_H

#include <QtGlobal>
#include <QString>
#include <string>

class StrUtil
{
public:
	static std::string stdstr(const QString& src)
	{
		return std::string(src.toUtf8().constData());
	}
	static int strncpy(char* dst, const QString& src, int len)
	{
		len = qMin(len, src.length());
		memcpy(dst, src.toUtf8().constData(), len);
		return len;
	}
};

#endif

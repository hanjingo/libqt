#include "ip.h"

bool IPv4::check(const QString& strIp)
{
    QStringList li = strIp.split(".");
    if(li.size() != 4)
        return false;

    for(int i=0;i<li.size();i++)
    {
        bool ok = false;
        int v = li[i].toInt(&ok);
        if(!ok || v < 0 || v > 255)
            return false;
    }
    return true;
}

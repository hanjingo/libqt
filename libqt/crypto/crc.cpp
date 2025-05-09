#include "crc.h"

quint16 Crc::calc16(const QByteArray array)
{
    quint8 index = 0;
    quint8 crcH = 0xff;
    quint8 crcL = 0xff;

    foreach(quint8 data, array.mid(1))
    {
        index = crcH ^ data;
        crcH = crcL ^ CRCHigh[index];
        crcL = CRCLow[index];
    }

    return (quint16)(crcH * 0x100 + crcL);
}

quint32 Crc::calc32(const QByteArray array)
{
    quint32 crc = 0xffffffff;
    foreach(quint8 data, array)
        crc = CRC32[(crc ^ data) & 0xff] ^ (crc >> 8);

    return crc ^ 0xffffffff;
}

#include "autogrid.h"
#include <QPainter>
#include <QPen>

AutoGrid::AutoGrid(QWidget *parent) : QWidget(parent)
{
    initial();
}

AutoGrid::~AutoGrid()
{
    initial();
}

void AutoGrid::drawBK()
{
    this->setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window,QColor("#000000"));
    this->setPalette(palette);
}

void AutoGrid::resizeEvent(QResizeEvent *event)
{
    m_widgetWidth = this->width();
    m_widgetHeight  = this->height();
    m_gridHeight = m_widgetHeight - m_marginTop - m_marginBottom;
    m_gridWidth = m_widgetWidth - m_marginLeft - m_marginRight;
    calAtomGridHeight();
    calAtomGridWidth();
}

void AutoGrid::initial()
{
    m_atomGridHeight = 60;
    m_atomGridHeightMin = 60;
    m_atomGridWidth = 640;
    m_atomGridWidthMin = 640;
    drawBK();
}

void AutoGrid::calAtomGridHeight()
{
    m_hGridNum = 0;
    int height = m_gridHeight;
    while( height - m_atomGridHeightMin > m_atomGridHeightMin)
    {
        ++m_hGridNum;
        height -= m_atomGridHeightMin;
    }
    m_atomGridHeight = m_gridHeight / m_hGridNum;
}

void AutoGrid::calAtomGridWidth()
{
    m_wGridNum = 0;
    int width = m_gridWidth;
    while( width - m_atomGridWidthMin >m_atomGridWidthMin)
    {
        ++m_wGridNum;
        width -= m_atomGridWidthMin;
    }
    m_atomGridWidth = m_gridWidth / m_wGridNum;
}

void AutoGrid::paintEvent(QPaintEvent* event)
{
    drawGrid();
}

void AutoGrid::drawHorLine()
{
    QPainter painter(this);
    QPen     pen;
    pen.setColor(QColor("#FF0000"));
    painter.setPen(pen);

    int xstart = m_marginLeft;
    int ystart = m_marginTop;
    int xend =  m_widgetWidth - m_marginRight;
    int yend = m_marginTop;

    for(int i=0;i<m_hGridNum+1;++i)
    {
        if( i == 0 || i == m_hGridNum)
        {
            pen.setStyle(Qt::SolidLine);
            painter.setPen(pen);
        }
        else
        {
            pen.setStyle(Qt::DashDotLine);
            painter.setPen(pen);
        }
        painter.drawLine(xstart,ystart+i*m_atomGridHeight,
                         xend,yend+i*m_atomGridHeight);
    }

    if( 0 == m_hGridNum)
    {
        painter.drawLine(m_marginLeft,m_marginTop,
                         m_widgetWidth - m_marginRight,m_marginTop);
        painter.drawLine(m_marginLeft,m_marginTop+m_gridHeight,
                         m_widgetWidth - m_marginRight,m_marginTop+m_gridHeight);
    }
}

void AutoGrid::drawVerLine()
{

    QPainter painter(this);
    QPen     pen;
    pen.setColor(QColor("#FF0000"));
    painter.setPen(pen);

    int xstart = m_marginLeft;
    int ystart = m_marginTop;
    int xend =  m_marginLeft;
    int yend = m_widgetHeight - m_marginBottom;

    for(int i=0;i<m_wGridNum+1;++i)
    {
        if( i == 0 || i == m_wGridNum)
        {
            pen.setStyle(Qt::SolidLine);
            painter.setPen(pen);
        }
        else
        {
            pen.setStyle(Qt::DashDotLine);
            painter.setPen(pen);
        }
        painter.drawLine(xstart+i*m_atomGridWidth,ystart,
                         xend+i*m_atomGridWidth,yend);
    }

    if( 0 == m_wGridNum)
    {
        painter.drawLine(m_marginLeft,m_marginTop,
                         m_marginLeft,m_widgetHeight - m_marginBottom);
        painter.drawLine(m_marginLeft+m_gridWidth,m_marginTop,
                         m_marginLeft+m_gridWidth,m_marginTop+m_gridHeight);
    }
}

void AutoGrid::drawGrid()
{
    drawHorLine();
    drawVerLine();
}

int AutoGrid::getMarginLeft() const
{
    return m_marginLeft;
}

void AutoGrid::setMarginLeft(int value)
{
    m_marginLeft = value;
}

int AutoGrid::getMarginRight() const
{
    return m_marginRight;
}

void AutoGrid::setMarginRight(int value)
{
    m_marginRight = value;
}

int AutoGrid::getMarginTop() const
{
    return m_marginTop;
}

void AutoGrid::setMarginTop(int value)
{
    m_marginTop = value;
}

int AutoGrid::getMarginBottom() const
{
    return m_marginBottom;
}

void AutoGrid::setMarginBottom(int value)
{
    m_marginBottom = value;
}

int AutoGrid::getWidgetHeight() const
{
    return m_widgetHeight;
}

int AutoGrid::getWidgetWidth() const
{
    return m_widgetWidth;
}

double AutoGrid::getGridHeight() const
{
    return m_gridHeight;
}

double AutoGrid::getGridWidth() const
{
    return m_gridWidth;
}

int AutoGrid::getHGridNum() const
{
    return m_hGridNum;
}

void AutoGrid::setHGridNum(int value)
{
    m_hGridNum = value;
}

int AutoGrid::getWGridNum() const
{
    return m_wGridNum;
}

void AutoGrid::setWGridNum(int value)
{
    m_wGridNum = value;
}

double AutoGrid::getAtomGridHeightMin() const
{
    return m_atomGridHeightMin;
}

void AutoGrid::setAtomGridHeightMin(double value)
{
    m_atomGridHeightMin = value;
}

double AutoGrid::getAtomGridWidthMin() const
{
    return m_atomGridWidthMin;
}

void AutoGrid::setAtomGridWidthMin(double value)
{
    m_atomGridWidthMin = value;
}

double AutoGrid::getAtomGridHeight() const
{
    return m_atomGridHeight;
}

double AutoGrid::getAtomGridWidth() const
{
    return m_atomGridWidth;
}

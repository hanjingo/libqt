#ifndef AUTOGRID_H
#define AUTOGRID_H

#include <QWidget>

class AutoGrid : public QWidget
{
    Q_OBJECT
public:
    explicit AutoGrid(QWidget *parent = 0);
    ~AutoGrid();

    void virtual initial();
    void virtual drawBK();
    void virtual resizeEvent(QResizeEvent* event);
    void virtual calAtomGridHeight();
    void virtual calAtomGridWidth();
    void virtual paintEvent(QPaintEvent* event);

    void drawGrid();
    void drawHorLine();
    void drawVerLine();

    int getMarginLeft() const;
    void setMarginLeft(int value);

    int getMarginRight() const;
    void setMarginRight(int value);

    int getMarginTop() const;
    void setMarginTop(int value);

    int getMarginBottom() const;
    void setMarginBottom(int value);

    int getWidgetHeight() const;

    int getWidgetWidth() const;

    double getGridHeight() const;

    double getGridWidth() const;

    int getHGridNum() const;
    void setHGridNum(int value);

    int getWGridNum() const;
    void setWGridNum(int value);

    double getAtomGridHeightMin() const;
    void setAtomGridHeightMin(double value);

    double getAtomGridWidthMin() const;
    void setAtomGridWidthMin(double value);

    double getAtomGridHeight() const;

    double getAtomGridWidth() const;

signals:

public slots:

private:
    // panel space
    int m_marginLeft      = 80;
    int m_marginRight     = 80;
    int m_marginTop       = 20;
    int m_marginBottom    = 20;

    int m_widgetHeight;
    int m_widgetWidth;

    double m_gridHeight;
    double m_gridWidth;

    double m_atomGridHeight;
    double m_atomGridWidth;

    int m_hGridNum;
    int m_wGridNum;

    double m_atomGridHeightMin;
    double m_atomGridWidthMin;
};

#endif // AUTOGRID_H

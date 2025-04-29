#ifndef SLIDER_H
#define SLIDER_H

#include <QObject>
#include <QSlider>
#include <QMouseEvent>

class Slider : public QSlider
{
    Q_OBJECT

signals:
    void sliderOffseted(int offset);

public:
    explicit Slider(QWidget *parent = nullptr);
    ~Slider();

    void setMousePressEnabled(bool);
    bool isAllowInc();
    bool valueInc();
    bool isAllowDec();
    bool valueDec();

protected:
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);

private:
    bool m_bMousePressEnabled;
    int  m_iLastPosition;
};

#endif // SLIDER_H

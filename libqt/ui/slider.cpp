#include "slider.h"

Slider::Slider(QWidget *parent) 
    : QSlider(parent)
    , m_bMousePressEnabled(true)
    , m_iLastPosition(value())
{
}

Slider::~Slider()
{
}

void Slider::mousePressEvent(QMouseEvent *ev)
{
    if (!m_bMousePressEnabled)
        return;

    QSlider::mousePressEvent(ev);
}

void Slider::mouseMoveEvent(QMouseEvent *ev)
{
    QSlider::mouseMoveEvent(ev);

    int position = value() - m_iLastPosition;
    if (position != 0)
        emit sliderOffseted(position);

    m_iLastPosition = value();
}

void Slider::setMousePressEnabled(bool bEnable)
{
    m_bMousePressEnabled = bEnable;
}

bool Slider::isAllowInc()
{
    return value() + singleStep() <= maximum();
}

bool Slider::valueInc()
{
    if (value() + singleStep() > maximum())
        return false;

    m_iLastPosition = value();
    setValue(value() + singleStep());
    emit sliderOffseted(value() - m_iLastPosition);
    return true;
}

bool Slider::isAllowDec()
{
    return value() - singleStep() >= minimum();
}

bool Slider::valueDec()
{
    if (value() - singleStep() < minimum())
        return false;

    m_iLastPosition = value();
    setValue(value() - singleStep());
    emit sliderOffseted(value() - m_iLastPosition);
    return true;
}

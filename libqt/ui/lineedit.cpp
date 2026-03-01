#include "lineedit.h"

#include <QRegularExpression>
#include <QRegularExpressionValidator>

IntegerLineEdit::IntegerLineEdit(QWidget *parent) : QLineEdit(parent)
{
    this->setValidator(new QRegularExpressionValidator(QRegularExpression("-?[0-9]\\d+$"), this));
}

DecimalLineEdit::DecimalLineEdit(QWidget *parent) : QLineEdit(parent)
{
    this->setValidator(new QRegularExpressionValidator(QRegularExpression("(-?[0-9]+(.\\d{0,4})?)$"), this));
}

NumAlphabetLineEdit::NumAlphabetLineEdit(QWidget *parent) : QLineEdit(parent)
{
    this->setValidator(new QRegularExpressionValidator(QRegularExpression("[A-Za-z0-9]+$"), this));
}

Ipv4LineEdit::Ipv4LineEdit(QWidget *parent) : QLineEdit(parent)
{
    this->setValidator(new QRegularExpressionValidator(QRegularExpression(
        "((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)"), this));
}

NetPortLineEdit::NetPortLineEdit(QWidget *parent) : QLineEdit(parent)
{
    this->setValidator(new QRegularExpressionValidator(QRegularExpression(
        "(/^[1-9]$|(^[1-9][0-9]$)|(^[1-9][0-9][0-9]$)|(^[1-9][0-9][0-9][0-9]$)|(^[1-6][0-5][0-5][0-3][0-5]$)/)"), this));
}

MacLineEdit::MacLineEdit(QWidget *parent) : QLineEdit(parent)
{
    this->setValidator(new QRegularExpressionValidator(QRegularExpression(
        "([0-9A-F]{2}-[0-9A-F]{2}-[0-9A-F]{2}-[0-9A-F]{2}-[0-9A-F]{2}-[0-9A-F]{2})"), this));
}

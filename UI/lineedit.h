#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QString>
#include <QLineEdit>
#include <QValidator>

class IntegerLineEdit : public QLineEdit
{
public:
    explicit IntegerLineEdit(QWidget *parent = nullptr);
    ~IntegerLineEdit() {}
};

class DecimalLineEdit : public QLineEdit
{
public:
    explicit DecimalLineEdit(QWidget *parent = nullptr);
    ~DecimalLineEdit() {}
};

class NumAlphabetLineEdit : public QLineEdit
{
public:
    explicit NumAlphabetLineEdit(QWidget *parent = nullptr);
    ~NumAlphabetLineEdit() {}
};

class Ipv4LineEdit : public QLineEdit
{
public:
    explicit Ipv4LineEdit(QWidget *parent = nullptr);
    ~Ipv4LineEdit() {}
};

class NetPortLineEdit : public QLineEdit
{
public:
    explicit NetPortLineEdit(QWidget *parent = nullptr);
    ~NetPortLineEdit() {}
};

class MacLineEdit : public QLineEdit
{
public:
    explicit MacLineEdit(QWidget *parent = nullptr);
    ~MacLineEdit() {}
};

#endif // LINEEDIT_H

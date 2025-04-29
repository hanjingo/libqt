#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QPainter>
#include <QPen>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_myGrid = new MyGrid(this);
    m_myGrid->setObjectName(tr("myGrid"));
    m_myGrid->setFocusPolicy(Qt::StrongFocus);

    this->setCentralWidget(m_myGrid);
}

MainWindow::~MainWindow()
{
    delete ui;
}

MyGrid::MyGrid(QWidget* parent)
    : AutoGrid(parent)
{
    setAtomGridHeightMin(40);
}

MyGrid::~MyGrid()
{

}

void MyGrid::paintEvent(QPaintEvent *event)
{
    AutoGrid::paintEvent(event);

    QPainter painter(this);
    QPen     pen;

    pen.setColor(Qt::red);
    painter.setPen(pen);
    double ystep = 10 / getHGridNum();
    QString str;
    for(int i = 0; i <= getHGridNum(); ++i)
    {
        str.sprintf("%d", (int)(i * ystep) );
        painter.drawText( QPoint(getWidgetWidth() - getMarginLeft() + 10,
                                getWidgetHeight() - getMarginBottom() - i * getAtomGridHeight()),
                         str);
    }
}

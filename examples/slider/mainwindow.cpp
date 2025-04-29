#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->slider1->setMaximum(5);
    connect(ui->slider1, SIGNAL(sliderOffseted(int)),
            this, SLOT(onSliderOffseted(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::onSliderOffseted(int offset)
{
    qDebug() << "onSliderOffseted with offset=" << offset;
}

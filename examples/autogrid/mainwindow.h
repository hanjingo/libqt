#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <libqt/ui/autogrid.h>

class MyGrid : public AutoGrid
{
    Q_OBJECT

public:
    explicit MyGrid(QWidget* parent);
    ~MyGrid();

    void virtual paintEvent(QPaintEvent* event);
};

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow* ui;

    AutoGrid* m_myGrid;
};
#endif // MAINWINDOW_H

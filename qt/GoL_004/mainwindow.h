#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QGraphicsLineItem>
#include <QGraphicsScene>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void onUpdateTime();
    void on_SliderUpdateSpeed_sliderMoved(int position);

    void on_checkBox1_clicked(bool checked);
    void on_checkBox2_clicked(bool checked);

    void on_SliderUpdateSpeed_valueChanged(int value);

private:
    Ui::MainWindow *ui;
    QTimer updateTimer;

    QGraphicsScene *scene;
    QGraphicsEllipseItem *ellipse;
    QGraphicsEllipseItem *dot;
    QGraphicsRectItem *rectangle;
    QGraphicsTextItem *text;
    QGraphicsLineItem *line;
};
#endif // MAINWINDOW_H

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
    void on_SliderUpdateSpeed_valueChanged(int value);

    void on_btnGenReset_clicked();

    void on_checkBox1_clicked(bool checked);
    void on_checkBox2_clicked(bool checked);
    void on_checkBox3_clicked(bool checked);

    void on_SliderBlocksize_sliderMoved(int position);
    void on_SliderBlocksize_valueChanged(int value);

    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();

    void drawRectangles(  int32_t ystart, int32_t ystop );


private:
    Ui::MainWindow *ui;
    QTimer updateTimer;

    QGraphicsScene *scene;
    QGraphicsEllipseItem *ellipse;
    QGraphicsEllipseItem *dot;
    QGraphicsRectItem *rectangle;
    //QGraphicsTextItem *text;
    QGraphicsLineItem *line;

};
#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QTimer>
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsView>



namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void onUpdateTime();
    void on_highButton_clicked();
    void on_lowButton_clicked();
    void on_quitButton_clicked();    
    void on_actionQuit_triggered();
    void on_actionGPIO25_PUP_triggered();
    void on_actionGPIO25_PDN_triggered();
    void drawGauge(QGraphicsScene *scene);

private:
    Ui::MainWindow *ui;
    QTimer updateTimer;

    QGraphicsScene *scene0;
    QGraphicsScene *scene1;
    QGraphicsScene *scene2;
    QGraphicsScene *scene3;

    QGraphicsEllipseItem *ellipse;
    QGraphicsRectItem *rectangle;
    QGraphicsTextItem *text;
    QGraphicsTextItem *text0;
    QGraphicsTextItem *text1;
    QGraphicsTextItem *text2;
    QGraphicsTextItem *text3;
    QGraphicsLineItem *line;
    QGraphicsLineItem *pline0;
    QGraphicsLineItem *pline1;
    QGraphicsLineItem *pline2;
    QGraphicsLineItem *pline3;


};

#endif // MAINWINDOW_H

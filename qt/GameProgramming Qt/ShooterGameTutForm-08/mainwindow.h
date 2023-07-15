#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>



namespace Ui {
    class MainWindow;
}



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();



private slots:
    void on_pushButton1_clicked();

    void on_verticalSlider1_sliderMoved(int position);
    void on_verticalSlider1_valueChanged(int value);

    void on_verticalSlider2_sliderMoved(int position);
    void on_verticalSlider2_valueChanged(int value);

private:
    Ui::MainWindow *ui;

    QGraphicsScene *scene;
};










#endif // MAINWINDOW_H

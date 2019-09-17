#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>


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
    void on_highButton_clicked();
    void on_lowButton_clicked();
    void on_quitButton_clicked();
    void onUpdateTime();

private:
    Ui::MainWindow *ui;
    int pin23, pin24;
    QTimer updateTimer;
};

#endif // MAINWINDOW_H

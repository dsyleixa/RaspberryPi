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

private:
    Ui::MainWindow *ui;
    QTimer updateTimer;

    // Heater: We are going to keep our grphics scene and some graphics items here.
    QGraphicsScene *scene;
    QGraphicsEllipseItem *ellipse;
    QGraphicsRectItem *rectangle;
    QGraphicsTextItem *text;
    QGraphicsLineItem *line;
};
#endif // MAINWINDOW_H

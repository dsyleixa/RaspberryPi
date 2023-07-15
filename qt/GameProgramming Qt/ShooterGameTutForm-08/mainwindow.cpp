#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <stdlib.h>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QBrush>

#include <QKeyEvent>
#include <QTimer>

#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"
#include "Score.h"
#include "Health.h"

#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QDebug>

Player * player;
Score  * score;
Health * health;

QMediaPlayer * music;
//QMediaPlayer * bulletsound;

int LoudnessMusic, LoudnessSFX;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{   
    ui->setupUi(this);

    // random seed
    srand( time(0) );

    // NoFocus for ui widgets
    ui->pushButton1->setFocusPolicy(Qt::NoFocus);

    ui->verticalSlider1->setFocusPolicy(Qt::NoFocus);
    LoudnessMusic=10;
    ui->verticalSlider1->setSliderPosition(LoudnessMusic);

    ui->verticalSlider2->setFocusPolicy(Qt::NoFocus);
    LoudnessSFX=20;
    ui->verticalSlider2->setSliderPosition(LoudnessSFX);



    scene  = new QGraphicsScene(this);
    // constant scene+view sizes, no scrollbars: also see mainwindow.ui
    // resize show the view
    // view->setFixedSize(800,600);
    scene->setSceneRect(0,0,800,600);
    ui->graphicsView->setFixedSize(800,600);
    scene->setBackgroundBrush(QBrush(QImage(":/images/bg.png")));



    // create the player
    /*
    player = new Player();
    player->setRect(0,0,100,100); // change the rect from 0x0 (default) to 100x100 pixels
    player->setPos(400,500); // TODO generalize to always be in the middle bottom of screen
    // make the player focusable and set it to be the current focus
    player->setFlag(QGraphicsItem::ItemIsFocusable);
    player->setFocus();
    // add the player to the scene
    scene->addItem(player);
    */
    player = new Player();
    player->setPos(400,500); // TODO generalize to always be in the middle bottom of screen
    // make the player focusable and set it to be the current focus
    player->setFlag(QGraphicsItem::ItemIsFocusable);
    player->setFocus();
    // add the player to the scene
    scene->addItem(player);



    // create the score/health
    score = new Score();
    scene->addItem(score);

    health = new Health();
    health->setPos(health->x(),health->y()+25);
    scene->addItem(health);
    


    // spawn enemies
    QTimer * timer = new QTimer();
    QObject::connect(timer,SIGNAL(timeout()),player,SLOT(spawn()));
    timer->start(2000);
    
    
    
    // play background music
    /*
    QMediaPlayer * music = new QMediaPlayer();
    music->setMedia(QUrl("qrc:/sounds/bgsound.mp3"));
    music->play();
    */
    QMediaPlaylist * playlist = new QMediaPlaylist();
    playlist->addMedia(QUrl("qrc:/sounds/bgsound.mp3"));
    playlist->setPlaybackMode(QMediaPlaylist::Loop);

    music = new QMediaPlayer();
    LoudnessMusic = 15;
    music->setVolume(LoudnessMusic);
    music->setPlaylist(playlist);
    music->play();


    ui->graphicsView->setScene(scene);
}


MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_pushButton1_clicked()
{
    qDebug() << "clicked or space bar hit";
    QCoreApplication::quit();
}




void MainWindow::on_verticalSlider1_sliderMoved(int position)
{
    //qDebug() << "slider moved";
    if (position>=40) position=40;
    LoudnessMusic=position;
    music->setVolume(LoudnessMusic);
}

void MainWindow::on_verticalSlider1_valueChanged(int value)
{
    //qDebug() << "slider changed";
    if (value>=40) value=40;
    LoudnessMusic=value;
    music->setVolume(LoudnessMusic);
}

void MainWindow::on_verticalSlider2_sliderMoved(int position)
{
    if (position>=40) position=40;
    LoudnessSFX=position;
}

void MainWindow::on_verticalSlider2_valueChanged(int value)
{
    if (value>=40) value=40;
    LoudnessSFX=value;
}

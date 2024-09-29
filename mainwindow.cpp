#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QTimer>
#include <QDate>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(update_clock()));
    timer->start();
}

void MainWindow::update_clock()
{
    QTime time = QTime::currentTime();
    QString time_to_text = time.toString("hh:mm:ss");
    QDate date = QDate::currentDate();
    QString date_to_text = date.toString();
    ui->clock->setText(time_to_text);
    ui->date->setText(date_to_text);
}



MainWindow::~MainWindow()
{
    delete ui;
}

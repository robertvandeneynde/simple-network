#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPainter>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.drawRect(posX, posY, 20, 40);
}

void MainWindow::mousePressEvent(QMouseEvent *ev) {

}


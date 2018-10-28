#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPainter>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    server = new QTcpServer();
    if(! server->listen(QHostAddress::Any, 8123)) {
        std::cout << "I am a client" << std::endl;
        other = new QTcpSocket();
        connect(other, SIGNAL(connected()), this, SLOT(onConnected()));
        other->connectToHost("127.0.0.1", 8123);
    } else {
        std::cout << "I am the server" << std::endl;
        other = nullptr;
    }

    connect(server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
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

void MainWindow::onNewConnection() {
    std::cout << "A new client is connecting !" << std::endl;
    other = server->nextPendingConnection();
}

void MainWindow::onConnected() {
    std::cout << "I am connected" << std::endl;
}


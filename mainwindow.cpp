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
        connect(other, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
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

void MainWindow::sendJson(QJsonObject obj) {
    QByteArray data = QJsonDocument(obj).toJson();
    QDataStream out(other);
    out << (quint32) data.length();
    other->write(data);

    std::cout << "Sending " << data.toStdString() << std::endl;
}

void MainWindow::mousePressEvent(QMouseEvent *ev) {
    int oldX = posX;
    int oldY = posY;
    posX = ev->x();
    posY = ev->y();

    QJsonObject move;
    move["oldX"] = oldX;
    move["oldY"] = oldY;
    move["newX"] = posX;
    move["newY"] = posY;

    sendJson(move);
    update();
}

void MainWindow::onNewConnection() {
    std::cout << "A new client is connecting !" << std::endl;
    other = server->nextPendingConnection();
    connect(other, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(other, SIGNAL(readyRead()), this, SLOT(onData()));

    QJsonObject info;
    posX = 0;
    posY = 0;
    info["x"] = posX;
    info["y"] = posY;
    update();

    isConfigured = true;

    sendJson(info);
}

void MainWindow::onConnected() {
    std::cout << "I am connected" << std::endl;
    connect(other, SIGNAL(readyRead()), this, SLOT(onData()));
}

void MainWindow::onDisconnected() {
    std::cout << "The other guy just disconnected" << std::endl;
}

void MainWindow::onData() {
    std::cout << "Some data !" << std::endl;
    if(currentSize == 0) {
        if(other->bytesAvailable() < 4)
            return;

        QDataStream in(other);
        in >> currentSize;
    }

    if(other->bytesAvailable() < currentSize)
        return;

    QByteArray data = other->read(currentSize);
    std::cout << data.toStdString() << std::endl;
    currentSize = 0;

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject json = doc.object();

    if(! isConfigured) {
        posX = json["x"].toInt();
        posY = json["y"].toInt();
        update();
        isConfigured = true;
    } else {
        int oldX = json["oldX"].toInt();
        int oldY = json["oldY"].toInt();
        int newX = json["newX"].toInt();
        int newY = json["newY"].toInt();

        if(!(posX == oldX && posY == oldY)) {
            std::cerr << "ERROR" << std::endl;
            destroy();
            return;
        }

        posX = newX;
        posY = newY;
        update();
    }
}

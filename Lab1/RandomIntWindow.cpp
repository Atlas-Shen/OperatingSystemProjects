#include "RandomIntWindow.h"
#include "ui_RandomIntWindow.h"
#include <QTimer>
#include <random>
#include <chrono>

RandomIntWindow::RandomIntWindow(int pid, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::RandomIntWindow),
      pid(pid),
      pTimer(new QTimer(this)) {
    ui->setupUi(this);
    ui->pidLabel->setText("PID: " + QString::number(pid));
    pTimer->setInterval(1000);
    connect(pTimer, &QTimer::timeout, this, &RandomIntWindow::update);
    pTimer->start();
    update();
}

RandomIntWindow::~RandomIntWindow() {
    delete ui;
}

void RandomIntWindow::update() {
    int seed = static_cast<int>(std::chrono::steady_clock::now().time_since_epoch().count());
    std::default_random_engine generator(seed);
    std::uniform_int_distribution<int> distribution(INT_MIN, INT_MAX);
    ui->randomIntLabel->setText(QString::number(distribution(generator), 10));
}

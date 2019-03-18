#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "System.h"
#include "CpuWidget.h"
#include "MemSwapWidget.h"
#include "StatusWidget.h"
#include "ProcessModel.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      pCpuWidget(new CpuWidget(this)),
      pMemSwapWidget(new MemSwapWidget(this)),
      pStatusWidget(new StatusWidget(this)),
      pProcessModel(new ProcessModel(this)) {

    ui->setupUi(this);
    connect(&System::instance(), &System::toMainWindow, this, &MainWindow::update);

    ui->resourceTab->layout()->addWidget(pCpuWidget);
    ui->resourceTab->layout()->addWidget(pMemSwapWidget);
    ui->statusBar->layout()->addWidget(pStatusWidget);

    ui->tableView->setModel(pProcessModel);
    ui->tableView->setShowGrid(false);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(ui->tableView->horizontalHeader(), &QHeaderView::sectionClicked, this, &MainWindow::sortProcess);
    connect(ui->refreshButton, &QPushButton::clicked, this, &MainWindow::refreshProcess);
    connect(ui->searchButton, &QPushButton::clicked, this, &MainWindow::searchProcess);

    ui->hostnameLabel->setText(System::instance().hostname());
    ui->systemVerLabel->setText(System::instance().systemVer());
    ui->cpuinfoLabel->setText(System::instance().cpuInfo());
    ui->startingTimeLabel->setText(System::instance().startingTime());

    update();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::update() {
    ui->lastingTimeLabel->setText(System::instance().lastingTime());
    ui->statusBar->showMessage(QDateTime::currentDateTime().toString("yyyy-MM-dd ddd hh:mm:ss"));
}

void MainWindow::sortProcess(int column) {
    ui->tableView->sortByColumn(column);
}

void MainWindow::refreshProcess() {
    pProcessModel->refresh();
}

void MainWindow::searchProcess() {
    QString searchText = ui->lineEdit->text();
    ui->lineEdit->clear();
    if (searchText.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Empty search input!");
    } else {
        bool ok;
        int pid = searchText.toInt(&ok);
        if (ok) {
            if (!pProcessModel->search(pid))
                QMessageBox::warning(this, "Warning", "No such process! Wrong process id!");
        } else {
            if (!pProcessModel->search(searchText.toStdString()))
                QMessageBox::warning(this, "Warning", "No such process! Wrong process name!");
        }
    }
}

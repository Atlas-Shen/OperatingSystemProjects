#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "System.h"
#include "CpuWidget.h"
#include "MemSwapWidget.h"
#include "StatusWidget.h"
#include "ProcessModel.h"
#include <QMessageBox>
#include <signal.h>
#include <cstdlib>

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
    connect(ui->endButton, &QPushButton::clicked, this, &MainWindow::endProcess);

    ui->hostnameLabel->setText(System::instance().hostname());
    ui->versionLabel->setText(System::instance().version());
    ui->startingTimeLabel->setText(System::instance().startingTime());

    connect(ui->shutDownButton, &QPushButton::clicked, []() {
        system("shutdown -h now");
    });
    connect(ui->restartButton, &QPushButton::clicked, []() {
        system("shutdown -r now");
    });

    update();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::update() {
    ui->lastingTimeLabel->setText(System::instance().lastingTime());
    ui->textBrowser->clear();
    for (auto &i : System::instance().cpuInfo())
        ui->textBrowser->append(i);
    ui->statusBar->showMessage(QDateTime::currentDateTime().toString("yyyy-MM-dd ddd hh:mm:ss"));
}

void MainWindow::sortProcess(int column) {
    ui->tableView->sortByColumn(column);
    ui->tableView->scrollToTop();
}

void MainWindow::refreshProcess() {
    pProcessModel->refresh();
    ui->tableView->scrollToTop();
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
            else
                ui->tableView->scrollToTop();
        } else {
            if (!pProcessModel->search(searchText.toStdString()))
                QMessageBox::warning(this, "Warning", "No such process! Wrong process name!");
            else
                ui->tableView->scrollToTop();
        }
    }
}

void MainWindow::endProcess() {
    QModelIndexList selectedRows = ui->tableView->selectionModel()->selectedRows();
    if (selectedRows.isEmpty()) {
        QMessageBox::warning(this, "Warning", "No selected process! Please select ths process before you end it!");
    } else if (QMessageBox::warning(this, "Warning", "Do you confirm to end the seleced process?",
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes) {
        int pid = pProcessModel->data(pProcessModel->index(selectedRows.front().row(), ProcessModel::PID)).toInt();
        switch (kill(pid, SIGKILL)) {
        case ESRCH:
            QMessageBox::warning(this, "Warning", "The selected process does not exist now!");
            break;
        case EPERM:
            QMessageBox::warning(this, "Warning", "No permission to end the process!");
            break;
        }
    }
}

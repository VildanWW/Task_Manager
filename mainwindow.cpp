#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtConcurrent/QtConcurrent>
#include <QThread>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    connect(this, &MainWindow::dataReady, this, &MainWindow::updateTable);

    process = std::make_unique<processMemory>();

    startGlobalUpdate();
}

void MainWindow::updateTable(QList<ProcessInfo> list) {
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableView->model()); // Привели обратно к QStandardItemModel(QAbstractItemModel-QStandardItemModel)

    const size_t countColumns = 7;

    if(!model) {
        model = new QStandardItemModel(this);
        model->setColumnCount(countColumns);

        QStringList headers = {
            "Name of process",
            "Memory (MB)",
            "CPU",
            "State",
            "Disk",
            "PID",
            "NetWork"
        };

        for(int i=0;i<headers.size();i++) {
            model->setHeaderData(i, Qt::Horizontal, headers[i]);
        }

        ui->tableView->setModel(model); // Обязательно привязалаи setModel к tanleView
    }

    model->setRowCount(0); // Не забыть очистить модель

    for(const auto& infoEx : list) {
        if(infoEx.memoryUsage > 0.0f) {
            QList<QStandardItem*> rowItems;
            rowItems.append(new QStandardItem(infoEx.name)); // Set name

            QStandardItem* memItem = new QStandardItem(QString::number(infoEx.memoryUsage, 'f', 2));
            memItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            rowItems.append(memItem); // Set memory

            rowItems.append(new QStandardItem("Unknows")); // Set CPU
            rowItems.append(new QStandardItem("Unknows")); // Set state
            rowItems.append(new QStandardItem("Unknows")); // Set disk
            rowItems.append(new QStandardItem(QString::number(infoEx.pid))); // Set PID
            rowItems.append(new QStandardItem("Unknows")); // Set netWork

            model->appendRow(rowItems);
        }
    }

    ui->tableView->viewport()->update(); // Команда на отрисовку(необязательно)
}

void MainWindow::startGlobalUpdate() { // Получение list каждую 500 мс
    QtConcurrent::run([this](){
        while(true) {
            auto list = process->processList();

            emit dataReady(list); // Отправляем данные из list

            QThread::msleep(500);
        }
    });
}

MainWindow::~MainWindow() {
    delete ui;
}

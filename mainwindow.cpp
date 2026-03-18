#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtConcurrent/QtConcurrent>
#include <QThread>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->tableView, &QTableView::customContextMenuRequested, this, &MainWindow::showContextMenu);
    connect(this, &MainWindow::dataReady, this, &MainWindow::updateTable);

    process = std::make_unique<processMemory>();

    startGlobalUpdate();
}

void MainWindow::updateTable(QList<ProcessInfo> list) {

    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableView->model());

    const size_t countColumns = 7;


    if(!model) { // Если модель еще не создана, то создаем
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

        for(int i = 0; i < headers.size(); i++) {
            model->setHeaderData(i, Qt::Horizontal, headers[i]);
        }

        ui->tableView->setModel(model);


        QHeaderView* header = ui->tableView->horizontalHeader();

        header->setSectionResizeMode(0, QHeaderView::Stretch);

        for(int i = 1; i < countColumns; ++i) {
            header->setSectionResizeMode(i, QHeaderView::ResizeToContents);
        }
    }

    model->setRowCount(0); // Очистить данные


    for(const auto& infoEx : list) {
        if(infoEx.memoryUsage > 0.0f) {
            QList<QStandardItem*> rowItems;

            rowItems.append(new QStandardItem(infoEx.name));

            QStandardItem* memItem = new QStandardItem(QString::number(infoEx.memoryUsage, 'f', 2));
            memItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            rowItems.append(memItem);


            rowItems.append(new QStandardItem("Unknown")); // CPU
            rowItems.append(new QStandardItem(infoEx.state)); // State
            rowItems.append(new QStandardItem("Unknown")); // Disk


            rowItems.append(new QStandardItem(QString::number(infoEx.pid))); // PID

            rowItems.append(new QStandardItem("Unknown")); // Network

            model->appendRow(rowItems);
        }
    }

    ui->tableView->viewport()->update(); // Обновляем отрисовку
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

void MainWindow::showContextMenu(const QPoint &pos) {
    QModelIndex index = ui->tableView->indexAt(pos);
    if(!index.isValid()) return;

    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableView->model());
    DWORD pid = model->data(model->index(index.row(), 5)).toUInt();
    QString procName = model->data(model->index(index.row(), 0)).toString();

    QMenu menu(this);
    QAction* killAction = menu.addAction("Kill this process: "+procName);

    QAction* selectedItem = menu.exec(ui->tableView->viewport()->mapToGlobal(pos));

    if(selectedItem == killAction) {
        if(process->killProcess(pid)) {
            qDebug()<<"Process killed";
        } else {
            qDebug()<<"Not kill process";
        }
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

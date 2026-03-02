#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    process = std::make_unique<processMemory>();
}

MainWindow::~MainWindow() {
    delete ui;
}

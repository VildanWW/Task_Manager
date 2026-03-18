#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "processmemory.h"
#include "memory"
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);

    void startGlobalUpdate();
    void showContextMenu(const QPoint &pos);

    ~MainWindow();
signals:
    void dataReady(QList<ProcessInfo> list);
private slots:
    void updateTable(QList<ProcessInfo> list);
private:
    Ui::MainWindow *ui;
    QStandardItemModel* model;
    std::unique_ptr<processMemory> process;
};
#endif // MAINWINDOW_H

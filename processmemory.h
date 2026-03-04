#ifndef PROCESSMEMORY_H
#define PROCESSMEMORY_H

#include <QObject>
#include <Windows.h>
#include <QList>
#include <TlHelp32.h>
#include <Psapi.h>
#include <QDebug>

struct ProcessInfo{
    QString name;
    DWORD pid;
    float memoryUsage;
};

class processMemory : public QObject {
    Q_OBJECT
public:
    explicit processMemory(QObject *parent = nullptr);

    QList<ProcessInfo> processList();

    bool killProcess(DWORD pid);
signals:
};

#endif // PROCESSMEMORY_H

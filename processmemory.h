#ifndef PROCESSMEMORY_H
#define PROCESSMEMORY_H

#include <QObject>

class processMemory : public QObject {
    Q_OBJECT
public:
    explicit processMemory(QObject *parent = nullptr);
private:

signals:
};

#endif // PROCESSMEMORY_H

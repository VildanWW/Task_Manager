#include "processmemory.h"

processMemory::processMemory(QObject *parent) : QObject{parent} {}

float processMemory::getMemoryUsage(DWORD pid) {
    float memory = 0.0f;

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);

    if(hProcess == NULL) return 0;

    PROCESS_MEMORY_COUNTERS pmc;
    if(GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
        memory = pmc.WorkingSetSize/1024.0f/1024.0f;
    }
    CloseHandle(hProcess);

    return memory;
}

QString processMemory::getStateWorking(DWORD pid) {
    QString state = "Unknown";

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);

    if(hProcess) {
        state = "Running";
        CloseHandle(hProcess);
    } else {
        state = "System / Protected";
    }

    return state;
}

QList<ProcessInfo> processMemory::processList() {
    QList<ProcessInfo> listProcesses; // Список процессов

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // Сделали снимок процессов

    if(hSnapshot == INVALID_HANDLE_VALUE) return listProcesses;

    PROCESSENTRY32 procEnt32; // Создание контейнера-переносчика
    procEnt32.dwSize = sizeof(PROCESSENTRY32);

    if(Process32First(hSnapshot, &procEnt32)) {
        do {
            ProcessInfo info;
            info.name = QString::fromWCharArray(procEnt32.szExeFile);
            info.pid = procEnt32.th32ProcessID; // получили уникальный номер процесса
            info.memoryUsage = getMemoryUsage(info.pid);
            info.state = getStateWorking(info.pid);

            listProcesses.push_back(info);
        } while(Process32Next(hSnapshot, &procEnt32));
    }
    CloseHandle(hSnapshot);
    return listProcesses;
}

bool processMemory::killProcess(DWORD pid) {
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);

    if(hProcess == NULL) return false;

    BOOL result = TerminateProcess(hProcess, 1); // Завершаем процесс(pid)

    CloseHandle(hProcess);
    return result;
}

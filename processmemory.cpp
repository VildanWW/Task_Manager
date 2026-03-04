#include "processmemory.h"

processMemory::processMemory(QObject *parent) : QObject{parent} {}

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

            info.memoryUsage = 0.0;

            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, info.pid);
            if(hProcess != NULL) {
                PROCESS_MEMORY_COUNTERS pmc; // Данные использования памяти процессом
                if(GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
                    info.memoryUsage = pmc.WorkingSetSize/1024.0f/1024.0f;
                }
                CloseHandle(hProcess);
            }

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

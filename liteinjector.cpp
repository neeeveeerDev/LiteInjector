#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

struct ProcessInfo {
    DWORD pid;
    std::wstring name;
};

std::vector<ProcessInfo> GetProcessList() {
    std::vector<ProcessInfo> processes;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return processes;
    PROCESSENTRY32W pe{};
    pe.dwSize = sizeof(pe);
    if (Process32FirstW(snap, &pe)) {
        do {
            processes.push_back({ pe.th32ProcessID, pe.szExeFile });
        } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap);
    return processes;
}

bool InjectDLL(DWORD pid, const std::string& dllPath) {
    HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD, FALSE, pid);
    if (!hProcess) return false;
    SIZE_T pathSize = dllPath.length() + 1;
    LPVOID pDllPath = VirtualAllocEx(hProcess, nullptr, pathSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!pDllPath) {
        CloseHandle(hProcess);
        return false;
    }
    WriteProcessMemory(hProcess, pDllPath, dllPath.c_str(), pathSize, nullptr);
    HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, pDllPath, 0, nullptr);
    if (hThread) {
        WaitForSingleObject(hThread, 2000);
        CloseHandle(hThread);
    }
    VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
    CloseHandle(hProcess);
    return hThread != nullptr;
}

int main() {
    SetConsoleTitleA("liteInjector");
    while (true) {
        std::cout << "\n========================================\n";
        std::cout << "          liteInjector v1.0\n";
        std::cout << "========================================\n";
        std::cout << "[1] Show process list\n";
        std::cout << "[2] Inject DLL\n";
        std::cout << "[3] Exit\n";
        std::cout << "========================================\n";
        std::cout << "Choice: ";
        int choice;
        std::cin >> choice;
        if (choice == 1) {
            std::cout << "\n--- Running Processes ---\n";
            auto processes = GetProcessList();
            std::wcout << std::left << std::setw(10) << L"PID" << L"Process Name\n";
            std::wcout << L"--------------------------------\n";
            for (const auto& p : processes) {
                std::wcout << std::left << std::setw(10) << p.pid << p.name << L"\n";
            }
            std::cout << "--------------------------\n";
            system("pause");
        } 
        else if (choice == 2) {
            std::cin.ignore();
            std::cout << "\nEnter DLL path: ";
            std::string dllPath;
            std::getline(std::cin, dllPath);
            if (!dllPath.empty() && dllPath.front() == '"' && dllPath.back() == '"') {
                dllPath = dllPath.substr(1, dllPath.length() - 2);
            }
            if (GetFileAttributesA(dllPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
                std::cout << "[!] File not found. Check the path.\n";
                system("pause");
                continue;
            }
            std::cout << "Enter target PID: ";
            DWORD pid;
            std::cin >> pid;
            std::cout << "\n[*] Injecting...\n";
            if (InjectDLL(pid, dllPath)) {
                std::cout << "[+] Successfully injected!\n";
            } else {
                std::cout << "[!] Injection failed. Run as Administrator or check PID.\n";
            }
            system("pause");
        } 
        else if (choice == 3) {
            break;
        }
        system("cls");
    }
    return 0;
}
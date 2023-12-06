#include <Windows.h>
#include <iostream>
#include <string>
#include <Psapi.h>
#include <conio.h>

bool IsRunningAsAdmin() {
    BOOL isRunAsAdmin = FALSE;
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    PSID AdministratorsGroup;

    BOOL result = AllocateAndInitializeSid(
        &NtAuthority,
        2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0,
        &AdministratorsGroup);

    if (result) {
        CheckTokenMembership(NULL, AdministratorsGroup, &isRunAsAdmin);
        FreeSid(AdministratorsGroup);
    }

    return isRunAsAdmin != 0;
}

bool isCtrlRPressed() {
    // VK_CONTROL = Ctrl, VK_R = R
    if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState('R')) {
        return true;
    }
    return false;
}

DWORD GetProcessIdByClassName(const char* className) {
    HWND hwnd = FindWindowA(className, NULL);
    if (hwnd == NULL) {
        std::cerr << "Game not detected -> " << className << std::endl;
        return 0;
    }

    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);
    return pid;
}



std::string GetProcessPath(DWORD pid) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess == NULL) {
        std::cerr << "Failed to open process. Error code -> " << GetLastError() << std::endl;
        return "";
    }

    char buffer[MAX_PATH];
    if (!GetModuleFileNameExA(hProcess, NULL, buffer, sizeof(buffer))) {
        std::cerr << "Failed to get process path. Error code -> " << GetLastError() << std::endl;
        CloseHandle(hProcess);
        return "";
    }
    CloseHandle(hProcess);
    return std::string(buffer);
}

void DisableNetworkForProcess(const std::string& processPath, int seconds) {
    std::string blockCmd = "netsh advfirewall firewall add rule name=\"BlockDso\" dir=out action=block program=\"" + processPath + "\" > nul 2>&1";
    std::string unblockCmd = "netsh advfirewall firewall delete rule name=\"BlockDso\" program=\"" + processPath + "\" > nul 2>&1";

    if (system(blockCmd.c_str()) != 0) {
        std::cerr << "Block -> 0" << std::endl;
        return;
    }
    else {
        std::cout << "Block -> 1" << std::endl;
    }

    std::cout << "Waiting" << std::endl;
    Sleep(seconds * 1000);

    if (system(unblockCmd.c_str()) != 0) {
        std::cerr << "Unblock -> 0" << std::endl;
    }
    else {
        std::cout << "Unblock -> 1" << std::endl;
    }
}

int main() {
    if (!IsRunningAsAdmin()) {
        std::cerr << "Please run the program with administrative privileges." << std::endl;
        return -1;
    }

    std::cout << "shit by lucifer" << std::endl;
    std::cout << "\nPress Ctrl+R to run the script." << std::endl;

    while (true) {
        if (isCtrlRPressed()) {
            const char* desiredClassName = "Nebula3::MainWindow";
            DWORD pid = GetProcessIdByClassName(desiredClassName);

            if (pid != 0) {
                std::cout << desiredClassName << " -> " << pid << std::endl;
                std::string processPath = GetProcessPath(pid);
                if (!processPath.empty()) {
                    std::cout << "Process path -> " << processPath << std::endl;
                    DisableNetworkForProcess(processPath, 15);
                }
            }
        }

        Sleep(100);
    }

    return 0;
}
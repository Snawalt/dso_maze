#include "network.h"

void n_blocker::b_listener() {
    std::cout << "ctrl+r to start" << std::endl;

    while (true) {
        if (GetAsyncKeyState(VK_CONTROL) & 0x8000 && GetAsyncKeyState('R') & 0x8000) {
            DWORD pid = b_getpid(className.c_str());

            if (pid != 0) {
                std::cout << className << " -> " << pid << std::endl;
                std::string processPath = b_getpath(pid);
                if (!processPath.empty()) {
                    b_blocker(processPath, 15);
                }
            }
        }

        Sleep(100);
    }
}

DWORD n_blocker::b_getpid(const char* className) {
    HWND hwnd = FindWindowA(className, NULL);
    if (hwnd == NULL) {
        std::cerr << "game window not detected" << std::endl;
        return 0;
    }

    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);
    return pid;
}

std::string n_blocker::b_getpath(DWORD pid) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess == NULL) {
        std::cerr << "failed to open process -> " << GetLastError() << std::endl;
        return "";
    }

    char buffer[MAX_PATH];
    if (!GetModuleFileNameExA(hProcess, NULL, buffer, sizeof(buffer))) {
        std::cerr << "failed to get process path -> " << GetLastError() << std::endl;
        CloseHandle(hProcess);
        return "";
    }
    CloseHandle(hProcess);
    return std::string(buffer);
}

void n_blocker::b_blocker(const std::string& processPath, int seconds) {
    std::string blockCmd = "netsh advfirewall firewall add rule name=\"BlockDso\" dir=out action=block program=\"" + processPath + "\" > nul 2>&1";
    std::string unblockCmd = "netsh advfirewall firewall delete rule name=\"BlockDso\" program=\"" + processPath + "\" > nul 2>&1";

    if (system(blockCmd.c_str()) != 0) {
        std::cerr << "block was failed" << std::endl;
        return;
    }
    else {
        std::cout << "block was successful" << std::endl;
    }

    std::cout << "waiting.." << std::endl;
    Sleep(seconds * 1000);

    if (system(unblockCmd.c_str()) != 0) {
        std::cerr << "unblock was failed" << std::endl;
    }
    else {
        std::cout << "unblock was successful" << std::endl;
    }
}

int main() {
    n_blocker blocker("Nebula3::MainWindow");
    blocker.b_listener();
    return 0;
}

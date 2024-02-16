#define NETWORK_H

#include <iostream>
#include <string>
#include <Windows.h>
#include <Psapi.h>

class n_blocker {
public:
    explicit n_blocker(const std::string& className) : className(className) {}

    void b_listener();

private:
    std::string className;

    DWORD b_getpid(const char* className);
    std::string b_getpath(DWORD pid);
    void b_blocker(const std::string& processPath, int seconds);
};
#include "console.h"
#include <ctime>
#include <cstdarg>

namespace Console {
    void Allocate() {
        AllocConsole();
        FILE* file;
        freopen_s(&file, "CONOUT$", "w", stdout);
        freopen_s(&file, "CONOUT$", "w", stderr);
        freopen_s(&file, "CONIN$", "r", stdin);
        SetConsoleTitleA("MHUR Internal");
    }

    void Free() {
        fclose(stdout);
        fclose(stderr);
        fclose(stdin);
        FreeConsole();
    }

    void Log(LogType type, const char* fmt, ...) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        
        time_t rawtime;
        struct tm timeinfo;
        char buffer[80];
        time(&rawtime);
        localtime_s(&timeinfo, &rawtime);
        strftime(buffer, sizeof(buffer), "[%H:%M:%S] ", &timeinfo);
        
        SetConsoleTextAttribute(hConsole, 8);
        printf("%s", buffer);

        switch (type) {
            case LOG_INFO:
                SetConsoleTextAttribute(hConsole, 15);
                printf("[INFO] ");
                break;
            case LOG_WARN:
                SetConsoleTextAttribute(hConsole, 14);
                printf("[WARN] ");
                break;
            case LOG_ERROR:
                SetConsoleTextAttribute(hConsole, 12);
                printf("[ERROR] ");
                break;
            case LOG_TRACE:
                SetConsoleTextAttribute(hConsole, 8);
                printf("[TRACE] ");
                break;
        }

        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);

        printf("\n");
        SetConsoleTextAttribute(hConsole, 15);
    }
}

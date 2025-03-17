#include "relocator.hpp"
#include <cstdlib>
#include <filesystem>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace fs = std::filesystem;

static fs::path get_executable_path() {
#ifdef _WIN32
    char buf[MAX_PATH];
    DWORD len = GetModuleFileNameA(nullptr, buf, MAX_PATH);
    if (len == 0) {
        std::cerr << "Failed to get executable path via GetModuleFileNameA."
                  << std::endl;
        return "";
    }
    return fs::path(buf);
#else
    char buf[1024];
    ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (len == -1) {
        std::cerr << "Failed to get executable path via readlink."
                  << std::endl;
        return "";
    }
    buf[len] = '\0';
    return fs::path(buf);
#endif
}

bool relocate_if_needed() {
    fs::path exePath = get_executable_path();
    if (exePath.empty()) {
        std::cerr << "Cannot determine executable path." << std::endl;
        return false;
    }

    std::cout << "Executable path: " << exePath.string() << "\n";

    if (exePath.parent_path().filename() == "EuropaLauncher") {
        return false;
    }

    fs::path targetDir = exePath.parent_path() / "EuropaLauncher";

    std::error_code ec;
    if (!fs::exists(targetDir, ec)) {
        fs::create_directories(targetDir, ec);
        if (ec) {
            std::cerr << "Failed to create directory \"" << targetDir.string()
                      << "\": " << ec.message() << "\n";
            return false;
        }
    }

    fs::path targetExe = targetDir / exePath.filename();

    fs::copy_file(
        exePath, targetExe,
        fs::copy_options::overwrite_existing, ec);
    if (ec) {
        std::cerr << "Failed to copy executable to \"" << targetExe.string()
                  << "\": " << ec.message() << "\n";
        return false;
    }

    std::cout << "Copied executable to: " << targetExe.string() << "\n";

#ifdef _WIN32
    {
        std::string command = "\"" + targetExe.string() + "\"";
        std::cout << "Launching new instance: " << command << "\n";
        system(command.c_str());
    }
#else
    {
        pid_t pid = fork();
        if (pid == 0) {
            execl(
                targetExe.string().c_str(), targetExe.filename().string().c_str(),
                static_cast<char*>(nullptr));
            std::cerr << "Failed to launch new instance using execl.\n";
            exit(1);
        }
    }
#endif

#ifdef _WIN32
    {
        fs::path batPath = exePath.parent_path() / "delete_self.bat";
        FILE *bat = fopen(batPath.string().c_str(), "w");
        if (bat) {
            fprintf(bat, ":loop\r\n");
            fprintf(bat, "del \"%s\"\r\n", exePath.string().c_str());
            fprintf(bat, "if exist \"%s\" goto loop\r\n", exePath.string().c_str());
            fprintf(bat, "del %%~f0\r\n");
            fclose(bat);
            std::string batCommand =
                "start /min \"\" \"" + batPath.string() + "\"";
            system(batCommand.c_str());
        }
    }
#else
    {
        fs::remove(exePath, ec);
        if (ec) {
            std::cerr << "Failed to delete original executable: "
                      << ec.message() << "\n";
        } else {
            std::cout << "Original executable deleted.\n";
        }
    }
#endif

    exit(0);
    return true;
}

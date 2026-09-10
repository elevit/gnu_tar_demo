#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "GnuTar.h"

#include <iostream>
#include <system_error>
#include <utility>

namespace fs = std::filesystem;

namespace {
std::wstring quote(const std::wstring& value) {
    std::wstring output = L"\"";
    std::size_t backslashes = 0;
    for (const wchar_t character : value) {
        if (character == L'\\') {
            ++backslashes;
        } else if (character == L'\"') {
            output.append(backslashes * 2 + 1, L'\\');
            output.push_back(L'\"');
            backslashes = 0;
        } else {
            output.append(backslashes, L'\\');
            backslashes = 0;
            output.push_back(character);
        }
    }
    output.append(backslashes * 2, L'\\');
    output.push_back(L'\"');
    return output;
}
}

GnuTar::GnuTar(fs::path executable) : executable_(std::move(executable)) {}

std::optional<GnuTar> GnuTar::locate(const std::optional<fs::path>& explicitPath) {
    if (explicitPath && fs::is_regular_file(*explicitPath)) {
        return GnuTar(fs::absolute(*explicitPath));
    }

    wchar_t environmentPath[32768]{};
    const DWORD environmentLength =
        GetEnvironmentVariableW(L"GNU_TAR", environmentPath, 32768);
    if (environmentLength > 0 && environmentLength < 32768 &&
        fs::is_regular_file(environmentPath)) {
        return GnuTar(fs::path(environmentPath));
    }

    for (const wchar_t* name : {L"gtar.exe", L"tar.exe"}) {
        wchar_t resolved[32768]{};
        const DWORD length = SearchPathW(nullptr, name, nullptr, 32768, resolved, nullptr);
        if (length > 0 && length < 32768) {
            return GnuTar(fs::path(resolved));
        }
    }
    return std::nullopt;
}

const fs::path& GnuTar::executable() const noexcept { return executable_; }

int GnuTar::run(const std::vector<std::wstring>& arguments) const {
    std::wstring commandLine = quote(executable_.wstring());
    for (const auto& argument : arguments) {
        commandLine += L" " + quote(argument);
    }
    std::vector<wchar_t> writable(commandLine.begin(), commandLine.end());
    writable.push_back(L'\0');

    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    PROCESS_INFORMATION process{};
    if (!CreateProcessW(executable_.c_str(), writable.data(), nullptr, nullptr, TRUE,
                        0, nullptr, nullptr, &startup, &process)) {
        std::wcerr << L"CreateProcess failed with Windows error " << GetLastError() << L".\n";
        return 1;
    }

    WaitForSingleObject(process.hProcess, INFINITE);
    DWORD exitCode = 1;
    GetExitCodeProcess(process.hProcess, &exitCode);
    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);
    return static_cast<int>(exitCode);
}

int GnuTar::showVersion() const { return run({L"--version"}); }

int GnuTar::createTarGz(const fs::path& archive, const fs::path& inputDirectory) const {
    if (!fs::is_directory(inputDirectory)) {
        std::cerr << "Input directory does not exist.\n";
        return 2;
    }
    std::error_code error;
    fs::create_directories(archive.parent_path(), error);
    if (error) {
        std::cerr << "Cannot create archive directory: " << error.message() << '\n';
        return 2;
    }
    return run({L"--create", L"--gzip", L"--file", fs::absolute(archive).wstring(),
                L"--directory", fs::absolute(inputDirectory).wstring(), L"."});
}

int GnuTar::listTarGz(const fs::path& archive) const {
    return run({L"--list", L"--gzip", L"--file", fs::absolute(archive).wstring()});
}

int GnuTar::extractTarGz(const fs::path& archive, const fs::path& outputDirectory) const {
    std::error_code error;
    fs::create_directories(outputDirectory, error);
    if (error) {
        std::cerr << "Cannot create extraction directory: " << error.message() << '\n';
        return 2;
    }
    return run({L"--extract", L"--gzip", L"--file", fs::absolute(archive).wstring(),
                L"--directory", fs::absolute(outputDirectory).wstring()});
}


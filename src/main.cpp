#include "GnuTar.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>

namespace fs = std::filesystem;

int wmain(int argc, wchar_t* argv[]) {
    std::optional<fs::path> explicitTar;
    fs::path workspace = fs::current_path() / L"demo-output";

    for (int index = 1; index < argc; ++index) {
        const std::wstring argument = argv[index];
        if (argument == L"--tar" && index + 1 < argc) {
            explicitTar = fs::path(argv[++index]);
        } else if (argument == L"--output" && index + 1 < argc) {
            workspace = fs::path(argv[++index]);
        } else {
            std::wcerr << L"Usage: tar_demo [--tar PATH_TO_GNU_TAR] [--output DIRECTORY]\n";
            return 2;
        }
    }

    const auto tar = GnuTar::locate(explicitTar);
    if (!tar) {
        std::cerr << "GNU tar was not found. Pass --tar, set GNU_TAR, or add it to PATH.\n";
        return 3;
    }

    std::wcout << L"Selected executable: " << tar->executable()
               << L"\nConfirm the next line says 'tar (GNU tar)':\n";
    if (tar->showVersion() != 0) return 4;

    const fs::path input = workspace / L"input";
    const fs::path extraction = workspace / L"extracted";
    const fs::path archive = workspace / L"sample.tar.gz";
    std::error_code error;
    fs::create_directories(input / L"subfolder", error);
    if (error) {
        std::cerr << "Could not create demo folders: " << error.message() << '\n';
        return 5;
    }
    std::ofstream(input / L"hello.txt") << "Created by the GNU tar demo.\n";
    std::ofstream(input / L"subfolder" / L"data.txt") << "Sample nested content.\n";

    std::wcout << L"\nCreating " << fs::absolute(archive) << L"\n";
    int result = tar->createTarGz(archive, input);
    if (result != 0) return result;

    std::wcout << L"\nArchive members:\n";
    result = tar->listTarGz(archive);
    if (result != 0) return result;

    std::wcout << L"\nExtracting into " << fs::absolute(extraction) << L"\n";
    result = tar->extractTarGz(archive, extraction);
    if (result == 0) std::cout << "Demo completed successfully.\n";
    return result;
}


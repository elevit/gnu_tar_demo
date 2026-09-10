#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

class GnuTar {
public:
    static std::optional<GnuTar> locate(
        const std::optional<std::filesystem::path>& explicitPath = std::nullopt);

    explicit GnuTar(std::filesystem::path executable);
    const std::filesystem::path& executable() const noexcept;
    int showVersion() const;
    int createTarGz(const std::filesystem::path& archive,
                    const std::filesystem::path& inputDirectory) const;
    int listTarGz(const std::filesystem::path& archive) const;
    int extractTarGz(const std::filesystem::path& archive,
                     const std::filesystem::path& outputDirectory) const;

private:
    int run(const std::vector<std::wstring>& arguments) const;
    std::filesystem::path executable_;
};


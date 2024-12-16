#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <zlib.h> // CRC32 i�in gerekli
#include <iomanip>
#include <chrono>
#include <thread>
namespace fs = std::filesystem;
std::ostringstream newFileName;
// CRC32 hesaplama fonksiyonu
uint32_t calculateCRC32(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Dosya a��lamad�.");
    }

    std::vector<char> buffer(1024);
    uint32_t crc = crc32(0L, Z_NULL, 0);

    while (file.good()) {
        file.read(buffer.data(), buffer.size());
        std::streamsize bytesRead = file.gcount();
        if (bytesRead > 0) {
            crc = crc32(crc, reinterpret_cast<const Bytef*>(buffer.data()), bytesRead);
        }
    }

    return crc;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Kullanim: " << argv[0] << " <dosya yolu>\n";
        return 1;
    }

    try {
        std::string filePath = argv[1];
        fs::path path(filePath);

        if (!fs::exists(path)) {
            std::cerr << "Dosya bulunamad�.\n";
            return 1;
        }

        // Dosya boyutunu hesapla
        auto fileSize = fs::file_size(path);

        // CRC32 hesapla
        uint32_t crc = calculateCRC32(filePath);

        // Yeni dosya ad� olu�tur

        newFileName << fileSize << "_" << std::hex << std::uppercase << crc << ".bin";

        // Yeniden adland�r
        fs::path newPath = path.parent_path() / newFileName.str();
        fs::rename(path, newPath);

        std::cout << "Dosya ba�ar�yla yeniden adland�r�ld�: " << newPath << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Hata: " << e.what() << "\n";
        return 1;
    }
    const std::string gitAdd = "git add " + newFileName.str();
    const std::string gitCommit = "git commit -m \" added " + newFileName.str() + " file to path. \"";

    system(gitAdd.c_str());
    std::cout << gitAdd << " printed.." << '\n';
   
    system(gitCommit.c_str());
    std::cout << gitCommit << " printed.." << '\n';
   
    system("git push --force origin main");
    std::cout << "git push" << " printed.." << '\n';
    

    return 0;
}

#include "util.hpp"

#include <cerrno>
#include <cstring>
#include <fstream>

extern "C" {
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
}

namespace AOC::Utils {

MappedStringView::MappedStringView(const std::string &filename, bool rdOnly)
  : rdOnly(rdOnly)
    , origFilename(filename) {
    fildes = open(filename.c_str(), O_RDONLY);
    if (fildes < 0) throw std::runtime_error(fmt::format("opening file {} for mmap failed with fd={}, errno={}", filename, fildes, std::strerror(errno)));
    auto closeGuard = Utils::SG::MakeGuard([this] { close(fildes); });

    struct stat stats{};
    if (fstat(fildes, &stats) < 0) throw std::runtime_error(fmt::format("stat for file {} (fd={}) for mmap failed with errno={}", filename, fildes, std::strerror(errno)));

    mapSize = stats.st_size;

    map = mmap(
      nullptr, mapSize,
      rdOnly ? PROT_READ : PROT_READ | PROT_WRITE,
      (mapSize >= 1024 * 1024 * 1024) ? MAP_SHARED : MAP_PRIVATE,
      fildes, 0);

    if (map == MAP_FAILED) throw std::runtime_error(fmt::format("mmap for file {} (fd={}) for mmap failed with errno={}", filename, fildes, std::strerror(errno)));
    auto munmapGuard = Utils::SG::MakeGuard([this] { munmap(map, mapSize); });

    //auto *mappedChars = static_cast<char *>(map);
    //impl = std::string_view{mappedChars, mappedChars + mapSize};

    munmapGuard.armed = false;
    closeGuard.armed = false;
}

MappedStringView::~MappedStringView() noexcept {
    munmap(map, mapSize);
    close(fildes);
}

extern std::vector<std::string> ReadLines(const std::string &filename) {
    std::ifstream ifs(filename);

    if (!ifs) throw std::runtime_error(fmt::format("file {} could not be opened", filename));

    std::vector<std::string> input;
    for (std::string current; std::getline(ifs, current);)
        input.push_back(current);

    return input;
}

std::vector<std::string_view> SplitStringView(std::string_view view) {
    return {};
}

}

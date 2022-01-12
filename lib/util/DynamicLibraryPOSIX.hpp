#ifndef spawn_dynamiclibrary_posix_hpp_INCLUDED
#define spawn_dynamiclibrary_posix_hpp_INCLUDED

#include <dlfcn.h>
#include <fmt/format.h>
#include <string>
#include <filesystem>

namespace openstudio {
namespace util {

struct DynamicLibrary
{
  template <typename Signature> [[nodiscard]] Signature *load_symbol(const std::string &name)
  {
    // reinterpret_cast is necessary here
    const auto symbol = reinterpret_cast<Signature *>(dlsym(m_handle.get(), name.c_str()));

    if (symbol == nullptr) {
      throw std::runtime_error(fmt::format("Unable to load symbol: '{}', reason: '{}'", name, dlerror()));
    }

    return symbol;
  }

  explicit DynamicLibrary(std::filesystem::path location)
      : m_location{std::move(location)}, m_handle{dlopen(m_location.c_str(), RTLD_LAZY), m_handle_deleter}
  {
    if (!m_handle) {
      throw std::runtime_error(fmt::format("Unable to load library '{}', reason: '{}'", m_location.string(), dlerror()));
    }
  }

  static void m_handle_deleter(void *h) {
    if (h) {
      dlclose(h);
    }
  }

  std::filesystem::path m_location{};
  std::unique_ptr<void, decltype(&m_handle_deleter)> m_handle{nullptr, m_handle_deleter};
};
}
}

#endif


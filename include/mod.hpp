#pragma once
#include <string>
#include <memory>
#include <vector>

#include <filesystem>
#include <sol/sol.hpp>

namespace loader
{
    namespace fs = std::filesystem;

    class mod
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      private:
        mod();

      public:
        ~mod();

      public:
        [[nodiscard]] fs::path path() const;

      public:
        [[nodiscard]] std::string name() const;
        [[nodiscard]] std::string author() const;

      public:
        [[nodiscard]] std::string version() const;
        [[nodiscard]] bool requires_restart() const;

      public:
        [[nodiscard]] std::string description() const;
        [[nodiscard]] std::string detailed_description() const;

      public:
        [[nodiscard]] std::vector<std::string> dependencies() const;

      public:
        [[nodiscard]] sol::environment env() const;

      public:
        [[nodiscard]] bool enabled() const;

      public:
        void enable(bool);

      public:
        void ready();
        void unload();

      public:
        void load();

      public:
        static std::shared_ptr<mod> from(const fs::path &path);
    };
} // namespace loader
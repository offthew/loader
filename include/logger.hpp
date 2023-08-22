#pragma once
#include <memory>
#include <string>
#include <vector>
#include <spdlog/spdlog.h>

namespace loader
{
    struct log_entry
    {
        std::string message;
        spdlog::level::level_enum level;
    };

    class logger
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      private:
        logger();

      public:
        ~logger();

      public:
        [[nodiscard]] std::vector<log_entry> logs() const;

      public:
        spdlog::logger *operator->() const;

      public:
        static logger &get();
    };
} // namespace loader
#include "logger.hpp"
#include "spdlog/common.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace loader
{
    struct logger::impl
    {
        std::shared_ptr<spdlog::logger> logger;
    };

    logger::logger() : m_impl(std::make_unique<impl>())
    {
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("coromodloader.log");
        auto ansi_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        auto sinks = spdlog::sinks_init_list{file_sink, ansi_sink};

        m_impl->logger = std::make_shared<spdlog::logger>("CoroModLoader", sinks);
        m_impl->logger->set_pattern("%Y-%m-%d %T [%n] %^[%-5!l]%$ %6v");

        m_impl->logger->set_level(spdlog::level::trace);
        m_impl->logger->flush_on(spdlog::level::trace);
    }

    logger::~logger() = default;

    spdlog::logger *logger::operator->() const
    {
        return m_impl->logger.get();
    }

    logger &logger::get()
    {
        static std::unique_ptr<logger> instance;

        if (!instance)
        {
            instance = std::unique_ptr<logger>(new logger);
        }

        return *instance;
    }
} // namespace loader
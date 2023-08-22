#include "logger.hpp"
#include "spdlog/common.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace loader
{
    struct logger::impl
    {
        std::shared_ptr<spdlog::logger> logger;
        std::vector<log_entry> logs;
    };

    class sink : public spdlog::sinks::base_sink<spdlog::details::null_mutex>
    {
        std::vector<log_entry> *m_logs;

      public:
        sink(std::vector<log_entry> *logs) : m_logs(logs) {}

      protected:
        void sink_it_(const spdlog::details::log_msg &msg) override
        {
            if (msg.level < spdlog::level::warn)
            {
                return;
            }

            std::string message{msg.payload.begin(), msg.payload.end()};
            m_logs->emplace_back(log_entry{.message = message, .level = msg.level});
        }

        void flush_() override {}
    };

    logger::logger() : m_impl(std::make_unique<impl>())
    {
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("coromodloader.log");
        auto sink = std::make_shared<class sink>(&m_impl->logs);
        auto ansi_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        auto sinks = spdlog::sinks_init_list{file_sink, ansi_sink, sink};
        m_impl->logger = std::make_shared<spdlog::logger>("CoroModLoader", sinks);

        m_impl->logger->set_pattern("%Y-%m-%d %T [%n] %^[%-5!l]%$ %6v");
        m_impl->logger->set_level(spdlog::level::trace);
        m_impl->logger->flush_on(spdlog::level::trace);
    }

    logger::~logger() = default;

    std::vector<log_entry> logger::logs() const
    {
        return m_impl->logs;
    }

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
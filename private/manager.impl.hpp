#include "manager.hpp"

#include <map>
#include <functional>
#include <sol/sol.hpp>

namespace loader
{
    struct manager::impl
    {
        static inline std::unique_ptr<manager> instance;

      public:
        std::unique_ptr<sol::state_view> lua;
        std::vector<std::shared_ptr<mod>> mods;

      public:
        sol::table mod_api;
        sol::function require;

      public:
        using hook_callback = std::function<void(const sol::object &)>;
        std::multimap<std::string, hook_callback> hooks;

      public:
        void setup_hooks();
        void setup_logger();
    };
} // namespace loader
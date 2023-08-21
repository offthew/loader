#include "manager.impl.hpp"
#include "logger.hpp"

#include <ranges>

namespace loader
{
    void manager::impl::setup_hooks()
    {
        auto table = mod_api.create_named("hooks");

        table["intercept_require"] = [this](const std::string &module, const sol::function &callback)
        {
            if (!hooks.contains(module))
            {
                logger::get()->debug("registering hook for \"{}\"", module);
            }

            hooks.emplace(module, callback.as<hook_callback>());
        };

        table["detour"] = [](sol::table table, const std::string &func, sol::function callback)
        {
            auto original = table.get<sol::function>(func);

            const auto detour = [=, callback = std::move(callback)](const sol::variadic_args &args)
            {
                return callback.call<sol::object>(original, sol::as_args(args));
            };

            const auto restore = [=]() mutable
            {
                table[func] = original;
            };

            table[func] = detour;

            return std::make_unique<std::function<void()>>(restore);
        };

        auto require = lua->get<sol::function>("require");
        (*lua)["require"] = [this, require](const std::string &module)
        {
            auto rtn = require.call<sol::object>(module);

            if (!hooks.contains(module))
            {
                return rtn;
            }

            logger::get()->debug("intercepting \"{}\"", module);

            using std::views::filter;
            auto callbacks = hooks | filter([&](const auto &x) { return x.first == module; });

            for (const auto &[_, callback] : callbacks)
            {
                callback(rtn);
            }

            return rtn;
        };
    }

    void manager::impl::setup_logger()
    {
        auto logger = mod_api.create_named("logger");

        const auto to_string = [this](const sol::variadic_args &args)
        {
            using std::views::transform;

            auto tostring = (*lua)["tostring"];
            auto transformed = args | transform([&](const auto &x) -> std::string { return tostring(x); });

            std::vector<std::string> strings{transformed.begin(), transformed.end()};
            return fmt::join(strings, ", ");
        };

        logger["error"] = [to_string](const sol::variadic_args &args)
        {
            logger::get()->error("[lua] {}", to_string(args));
        };

        logger["debug"] = [to_string](const sol::variadic_args &args)
        {
            logger::get()->debug("[lua] {}", to_string(args));
        };

        logger["info"] = [to_string](const sol::variadic_args &args)
        {
            logger::get()->info("[lua] {}", to_string(args));
        };
    }
} // namespace loader
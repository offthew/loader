#include "mod.impl.hpp"
#include "manager.hpp"
#include "logger.hpp"

#include <filesystem>
#include <fmt/core.h>
#include <regex>

namespace loader
{
    void mod::impl::setup_logger()
    {
        std::string name = json["name"];
        name = "[" + name + "]";

        auto logger = mod_api.create_named("logger");
        auto original = (*manager::get().lua())["mod_api"]["logger"];

        logger["logs"] = original["logs"];

        logger["error"] = [=](const sol::variadic_args &args)
        {
            original["error"](name, sol::as_args(args));
        };

        logger["debug"] = [=](const sol::variadic_args &args)
        {
            original["debug"](name, sol::as_args(args));
        };

        logger["info"] = [=](const sol::variadic_args &args)
        {
            original["info"](name, sol::as_args(args));
        };

        logger["warn"] = [=](const sol::variadic_args &args)
        {
            original["warn"](name, sol::as_args(args));
        };
    }

    void mod::impl::setup_require()
    {
        using require_t = std::function<sol::object(const std::string &)>;
        auto require = (*manager::get().lua())["require"].get<require_t>();

        env["require"] = [require, this](std::string module) -> sol::object
        {
            auto root = config_path.parent_path();

            if (module.starts_with("!"))
            {
                return require(module.substr(1));
            }

            if (module.starts_with("@") && module.find(':') != std::string::npos)
            {
                auto mod = module.substr(1);
                mod = mod.substr(0, mod.find_first_of(':'));

                auto enabled = manager::get().enabled();
                auto target_mod = std::ranges::find_if(enabled, [mod](auto &x) { return x->name() == mod; });

                if (target_mod == enabled.end())
                {
                    logger::get()->error("requested mod (\"{}\") is not loaded", mod);
                    return sol::nil;
                }

                root = target_mod->get()->path();
                module = module.substr(mod.size() + 2);
            }

            const auto name = fmt::format("{}.lua", std::regex_replace(module, std::regex{"\\."}, "/"));
            const auto target = fs::weakly_canonical(root / name);

            if (!target.string().starts_with(root.string()))
            {
                logger::get()->error("not loading \"{}\" as it would escape sandbox", target.string());
                return sol::nil;
            }

            if (!fs::exists(target) || !fs::is_regular_file(target))
            {
                logger::get()->error("requested file (\"{}\") does not exist", target.string());
                return sol::nil;
            }

            auto result = manager::get().lua()->script_file(target.string(), env);

            if (!result.valid())
            {
                logger::get()->error("loading file (\"{}\") resulted in error", target.string());
                return sol::nil;
            }

            return result.get<sol::object>();
        };
    }
} // namespace loader
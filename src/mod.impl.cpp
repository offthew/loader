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

        auto &lua = *manager::get().lua();
        auto original = lua["mod_api"]["logger"];

        auto logger = shallow_copy(original);

        logger["error"] = [=](const sol::variadic_args &args)
        {
            original["error"](name, sol::as_args(args));
        };

        logger["debug"] = [=](const sol::variadic_args &args)
        {
            original["debug"](name, sol::as_args(args));
        };

        mod_api["logger"] = logger;
    }

    void mod::impl::setup_require()
    {
        using require_t = std::function<sol::object(const std::string &)>;
        auto require = (*manager::get().lua())["require"].get<require_t>();

        env["require"] = [require, this](std::string module) -> sol::object
        {
            auto root = config_path.parent_path();

            if (module.starts_with(":"))
            {
                return require(module.substr(1));
            }

            auto environment = env;

            if (module.starts_with("@") && module.find(':') != std::string::npos)
            {
                auto mod = module.substr(1);
                mod = mod.substr(0, mod.find_first_of(':'));

                auto enabled = manager::get().enabled();
                auto it = std::ranges::find_if(enabled, [mod](auto &x) { return x->name() == mod; });

                if (it == enabled.end())
                {
                    logger::get()->error("requested mod (\"{}\") is not loaded", mod);
                    return sol::nil;
                }

                auto target = *it;

                root = target->path();
                environment = target->env();
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

            auto &lua = *manager::get().lua();
            auto result = lua.safe_script_file(target.string(), environment, sol::script_pass_on_error);

            if (!result.valid())
            {
                auto error = result.get<sol::error>();
                logger::get()->error("loading file (\"{}\") resulted in error: {}", target.string(), error.what());

                return sol::nil;
            }

            return result.get<sol::object>();
        };
    }

    sol::table mod::impl::shallow_copy(const sol::table &table)
    {
        auto copy = env.create();

        for (const auto &[key, value] : table.pairs())
        {
            copy[key] = value;
        }

        return copy;
    }
} // namespace loader
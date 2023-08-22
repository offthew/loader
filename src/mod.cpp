#include "mod.hpp"
#include "mod.impl.hpp"

#include "logger.hpp"
#include "manager.hpp"

#include <fstream>
#include <filesystem>

#include <semver.hpp>
#include <nlohmann/json.hpp>

namespace loader
{
    mod::mod() : m_impl(std::make_unique<impl>()) {}

    mod::~mod() = default;

    fs::path mod::path() const
    {
        return m_impl->config_path.parent_path();
    }

    std::string mod::name() const
    {
        return m_impl->json["name"];
    }

    std::string mod::author() const
    {
        return m_impl->json["author"];
    }

    std::string mod::version() const
    {
        return m_impl->json["version"];
    }

    bool mod::requires_restart() const
    {
        return m_impl->json.value("requires_restart", true);
    }

    std::string mod::description() const
    {
        return m_impl->json["description"];
    }

    std::string mod::detailed_description() const
    {
        return m_impl->json.value("detailed_description", description());
    }

    std::vector<std::string> mod::dependencies() const
    {
        return m_impl->json.value("dependencies", std::vector<std::string>{});
    }

    bool mod::enabled() const
    {
        return m_impl->json.value("enabled", true);
    }

    void mod::enable(bool value)
    {
        m_impl->json["enabled"] = value;

        std::ofstream file{m_impl->config_path};
        file << m_impl->json.dump();

        if (!file)
        {
            // NOLINTNEXTLINE(concurrency-mt-unsafe)
            logger::get()->error("failed to save config for \"{}\": {}", name(), strerror(errno));
            return;
        }

        if (requires_restart())
        {
            return;
        }

        value ? load() : unload();
    }

    void mod::load()
    {
        auto &lua = *manager::get().lua();
        m_impl->env = {lua, sol::create};

        m_impl->mod_api = m_impl->env.create_named("mod_api");
        m_impl->env["globals"] = lua.globals();

        auto mod_api = lua.get<sol::table>("mod_api");

        for (const auto &[key, value] : mod_api)
        {
            m_impl->mod_api[key] = value;
        }

        m_impl->setup_require();
        m_impl->setup_logger();

        lua.script_file(m_impl->init_path.string(), m_impl->env);
    }

    void mod::unload()
    {
        sol::function unload = m_impl->env["unload"];

        if (!unload.valid())
        {
            return;
        }

        unload.call();
    }

    void mod::ready()
    {
        sol::function ready = m_impl->env["ready"];

        if (!ready.valid())
        {
            return;
        }

        ready.call();
    }

    std::shared_ptr<mod> mod::from(const fs::path &path)
    {
        if (!fs::is_directory(path))
        {
            logger::get()->error("cannot load mod, \"{}\" is not a directory", path.string());
            return nullptr;
        }

        auto init = path / "init.lua";

        if (!fs::exists(init) || !fs::is_regular_file(init))
        {
            logger::get()->error("init.lua does not exist in \"{}\"", path.string());
            return nullptr;
        }

        auto config = path / "config.json";

        if (!fs::exists(config) || !fs::is_regular_file(config))
        {
            logger::get()->error("cannot load mod, \"{}\" does not exist or is not a file", config.string());
            return nullptr;
        }

        std::ifstream file{config};
        auto parsed = nlohmann::json::parse(file);

        if (!parsed["name"].is_string() ||     //
            !parsed["author"].is_string() ||   //
            !parsed["version"].is_string() ||  //
            !parsed["description"].is_string() //
        )
        {
            logger::get()->error("\"{}\" is missing one or more required fields", config.string());
            return nullptr;
        }

        auto version = parsed["version"].get<std::string>();

        if (!semver::from_string_noexcept(version))
        {
            logger::get()->error(R"("{}" is not a valid semantic version (in "{}"))", version, config.string());
            return nullptr;
        }

        auto rtn = std::shared_ptr<mod>(new mod);

        rtn->m_impl->config_path = config;
        rtn->m_impl->init_path = init;
        rtn->m_impl->json = parsed;

        return rtn;
    }
} // namespace loader
#include "manager.hpp"
#include "manager.impl.hpp"

#include "logger.hpp"

namespace loader
{
    manager::manager() : m_impl(std::make_unique<impl>()) {}

    manager::~manager() = default;

    std::vector<std::shared_ptr<mod>> manager::mods() const
    {
        return m_impl->mods;
    }

    void manager::init(lua_State *state)
    {
        if (impl::instance)
        {
            return;
        }

        impl::instance = std::unique_ptr<manager>(new manager);
        impl::instance->m_impl->lua = std::make_unique<sol::state_view>(state);

        auto &lua = *impl::instance->m_impl->lua;
        impl::instance->m_impl->mod_api = lua.create_named_table("mod_api");

        impl::instance->m_impl->setup_hooks();
        impl::instance->m_impl->setup_logger();
    }

    void manager::ready()
    {
        if (!impl::instance)
        {
            logger::get()->critical("ready() called before init!");
            return;
        }

        auto &m_impl = impl::instance->m_impl;

        for (const auto &mod : m_impl->mods)
        {
            mod->ready();
        }
    }
} // namespace loader
#include "manager.hpp"
#include "manager.impl.hpp"

#include <ranges>

namespace loader
{
    manager::manager() : m_impl(std::make_unique<impl>()) {}

    manager::~manager() = default;

    sol::state_view *manager::lua() const
    {
        return m_impl->lua.get();
    }

    std::vector<std::shared_ptr<mod>> manager::mods() const
    {
        return m_impl->mods;
    }

    std::vector<std::shared_ptr<mod>> manager::enabled() const
    {
        using std::views::filter;
        auto enabled = m_impl->mods | filter([](auto &x) { return x->enabled(); });

        return {enabled.begin(), enabled.end()};
    }

    void manager::ready()
    {
        static bool once = false;

        if (once)
        {
            return;
        }

        std::unique_lock guard{m_impl->init_mutex};
        once = true;

        for (const auto &mod : enabled())
        {
            mod->ready();
        }
    }

    void manager::init(lua_State *state)
    {
        std::unique_lock guard{m_impl->init_mutex};

        m_impl->lua = std::make_unique<sol::state_view>(state);
        m_impl->mod_api = m_impl->lua->create_named_table("mod_api");

        m_impl->setup_panic();

        m_impl->setup_mods();
        m_impl->setup_hooks();
        m_impl->setup_logger();

        m_impl->load_mods();
    }

    manager &manager::get()
    {
        if (!impl::instance)
        {
            impl::instance = std::unique_ptr<manager>(new manager);
        }

        return *impl::instance;
    }
} // namespace loader
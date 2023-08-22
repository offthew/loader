#pragma once
#include "mod.hpp"

#include <vector>
#include <memory>

#include <lua.hpp>
#include <sol/sol.hpp>

namespace loader
{
    class manager
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      private:
        manager();

      public:
        ~manager();

      public:
        [[nodiscard]] sol::state_view *lua() const;

      public:
        [[nodiscard]] std::vector<std::shared_ptr<mod>> mods() const;
        [[nodiscard]] std::vector<std::shared_ptr<mod>> enabled() const;

      public:
        void ready();

      public:
        void init(lua_State *state);

      public:
        static manager &get();
    };
} // namespace loader
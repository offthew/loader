#pragma once
#include "mod.hpp"

#include <vector>
#include <memory>
#include <lua.hpp>

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

      private:
        [[nodiscard]] std::vector<std::shared_ptr<mod>> mods() const;

      public:
        static void init(lua_State *state);
        static void ready();
    };
} // namespace loader
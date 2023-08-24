#pragma once
#include "mod.hpp"

#include <sol/sol.hpp>
#include <nlohmann/json.hpp>

namespace loader
{
    struct mod::impl
    {
        nlohmann::json json;

      public:
        fs::path init_path;
        fs::path config_path;

      public:
        sol::table mod_api;
        sol::environment env;

      public:
        void setup_logger();
        void setup_require();

      public:
        sol::table shallow_copy(const sol::table &);
    };
} // namespace loader
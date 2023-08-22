#include "logger.hpp"
#include "manager.hpp"

#include <lua.hpp>
#include <lime/module.hpp>
#include <lime/entrypoint.hpp>
#include <lime/hooks/hook.hpp>

using loader::logger;

void lime::load()
{
    logger::get()->info("all the nuts roll down to florida!");

    auto corona = lime::module::get("CoronaLabs.Corona.Native.dll");

    if (!corona)
    {
        logger::get()->error("couldn't find \"CoronaLabs.Corona.Native.dll\"");
        return;
    }

    auto version_string = corona->symbol("CoronaVersionBuildString");
    using version_string_t = const char *(*)();

    if (!version_string)
    {
        logger::get()->error("couldn't find \"CoronaVersionBuildString\"");
        return;
    }

    const auto *version = reinterpret_cast<version_string_t>(version_string)();
    logger::get()->info("running on corona version \"{}\"", version);

    auto lua = lime::module::get("lua.dll");

    if (!lua)
    {
        logger::get()->error("couldn't find \"lua.dll\"");
        return;
    }

    auto push_string = lua->symbol("lua_pushstring");
    using push_string_t = const char *(lua_State *, const char *);

    if (!push_string)
    {
        logger::get()->error("couldn't find \"lua_pushstring\"");
        return;
    }

    lime::hook<push_string_t>::create(push_string,
                                      [](auto *hook, lua_State *state, const char *name)
                                      {
                                          auto rtn = hook->original()(state, name);

                                          if (strcmp(name, "coronabaselib") == 0)
                                          {
                                              auto state_addr = reinterpret_cast<std::uintptr_t>(state);
                                              logger::get()->debug("lua state is at {0:#x}", state_addr, name);

                                              loader::manager::get().init(state);
                                              delete hook;
                                          }

                                          return rtn;
                                      });
}

void lime::unload()
{
    logger::get()->info("unload");
}
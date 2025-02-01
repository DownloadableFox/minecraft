#pragma once

#include <filesystem>

extern "C" {
    #include <lua/lua.hpp>
}

namespace engine {
    class LuaScript {
    private:
        lua_State* L;
    public:
        LuaScript(const std::filesystem::path& path);
        ~LuaScript();

        /* Delete copying */
        LuaScript(const LuaScript&) = delete;
        LuaScript& operator=(const LuaScript&) = delete;

        /* Move constructor */
        LuaScript(LuaScript&& other) noexcept;
        LuaScript& operator=(LuaScript&& other) noexcept;
    };
};
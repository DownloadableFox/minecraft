#include "scripts.h"
#include <iostream>

namespace engine {
    LuaScript::LuaScript(const std::filesystem::path& path) {
        L = luaL_newstate();
        luaL_openlibs(L);

        if (luaL_loadfile(L, path.string().c_str()) || lua_pcall(L, 0, 0, 0)) {
            std::cerr << "Error: script not loaded (" << path << ")" << std::endl;
        }
    }

    LuaScript::~LuaScript() {
        if (L != nullptr) {
            lua_close(L);
        }
    }

    LuaScript::LuaScript(LuaScript&& other) noexcept {
        L = other.L;
        other.L = nullptr;
    }

    LuaScript& LuaScript::operator=(LuaScript&& other) noexcept {
        if (this != &other) {
            if (L != nullptr) {
                lua_close(L);
            }
            
            L = other.L;
            other.L = nullptr;
        }

        return *this;
    }
};	
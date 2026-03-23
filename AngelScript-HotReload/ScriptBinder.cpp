#include "ScriptBinder.h"

#include <cstdio>
#include <cstdlib>
#include <format>
#include <iostream>
#include <ostream>
#include <string>

#include "HostTypes.h"
#include "RefWrapper.h"
#include "add_on/scriptarray/scriptarray.h"
#include "add_on/scriptstdstring/scriptstdstring.h"
#include "asbind20/bind/class.hpp"
#include "asbind20/bind/global.hpp"

namespace
{
    void MessageCallback(const asSMessageInfo* msg, void* param)
    {
        auto type = "[error]";
        if (msg->type == asMSGTYPE_WARNING)
            type = "[warn] ";
        else if (msg->type == asMSGTYPE_INFORMATION)
            type = "[info] ";

        const auto message = std::format("{} {}({},{}) {}", type, msg->section, msg->row, msg->col, msg->message);
        if (msg->type == asMSGTYPE_INFORMATION)
        {
            std::cout << message << std::endl;
        }
        else
        {
            std::cerr << message << std::endl;
        }
    }

    void script_print(const std::string& message)
    {
        printf("%s", message.c_str());
    }

    void script_println(const std::string& message)
    {
        printf("%s\n", message.c_str());
    }

    int rand_(int n)
    {
        return std::rand() % n;
    }
}

namespace MyProject
{
    void BindScript(asIScriptEngine* engine)
    {
        RegisterStdString(engine);
        RegisterScriptArray(engine, true);

        // -----------------------------------------------
        using namespace asbind20;

        asbind20::global(engine)
            .message_callback(&MessageCallback)
            .function("void print(const string& in message)", &script_print) // グローバル関数登録
            .function("void println(const string& in message)", &script_println)
            .function("int rand(int n)", &rand_);

        asbind20::value_class<Fighter>(engine, "Fighter")
            .behaviours_by_traits()
            .property("string name", &Fighter::name)
            .property("int life", &Fighter::life);

        asbind20::ref_class<RefWrapper<Fighter>>(engine, "FighterRef")
            .default_factory()
            .factory<Fighter>("Fighter fighter")
            .addref(&RefWrapper<Fighter>::addRef)
            .release(&RefWrapper<Fighter>::release)
            .method("Fighter& get()", overload_cast<>(&RefWrapper<Fighter>::get))
            .method("const Fighter& get() const", overload_cast<>(&RefWrapper<Fighter>::get, const_));
    }
}

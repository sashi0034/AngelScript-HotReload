#include <filesystem>
#include <iostream>
#include <iso646.h>

#include "ScriptBinder.h"
#include "ScriptPredefinedGenerator.h"
#include "add_on/scriptstdstring/scriptstdstring.h"
#include "add_on/datetime/datetime.h"
#include "add_on/scriptbuilder/scriptbuilder.h"
#include "asbind20/bind.hpp"
#include "asbind20/invoke.hpp"

namespace
{
    void waitForFileChange(const std::string& path)
    {
        namespace fs = std::filesystem;
        auto last_time = fs::last_write_time(path);

        while (true)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            auto current_time = fs::last_write_time(path);

            if (current_time != last_time)
            {
                break;
            }
        }
    }
}

int main()
{
    const auto engine = asbind20::make_script_engine();

    MyProject::BindScript(engine);

    MyProject::GenerateScriptPredefined(engine, "my_script/as.predefined");

    for (int step = 0; ; ++step)
    {
        CScriptBuilder builder{};

        std::string moduleName{"my_script/my_script.as"};

        if (step >= 1)
        {
            waitForFileChange(moduleName);
        }

        std::cout << "--- step " << step << " ---" << std::endl;

        int r;
        r = builder.StartNewModule(engine, moduleName.c_str());
        if (r < 0)
        {
            continue;
        }

        r = builder.AddSectionFromFile(moduleName.c_str());
        if (r < 0)
        {
            continue;
        }

        r = builder.BuildModule();
        if (r < 0)
        {
            continue;
        }

        asIScriptModule* module = engine->GetModule(moduleName.c_str());
        assert(module != nullptr);

        const asbind20::request_context ctx{engine};

        // my_script.as で定義された as_main を取得する
        asIScriptFunction* func =
            module->GetFunctionByDecl("int tick_frame()");

        // 引数を渡して実行する
        const auto result = asbind20::script_invoke<int>(ctx, func);
        if (result.has_value())
        {
            std::cout << "C++: result: " << result.value() << std::endl;
        }
        else
        {
            std::cerr << "Failed to execute the script: " << result.error() << std::endl;
            if (result.error() == asEXECUTION_EXCEPTION)
            {
                auto* func = ctx->GetExceptionFunction();

                // std::format で整形して出力
                std::cerr << std::format(
                    "[runtime exception]\n"
                    "{}\n"
                    "\tin {}\n"
                    "\tline: {} \n",
                    ctx->GetExceptionString(),
                    func ? func->GetDeclaration() : "unknown",
                    ctx->GetExceptionLineNumber()
                ) << std::endl;
            }
        }

        module->Discard();
    }
}

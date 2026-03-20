#include <iostream>

#include "ScriptBinder.h"
#include "ScriptPredefinedGenerator.h"
#include "add_on/scriptstdstring/scriptstdstring.h"
#include "add_on/datetime/datetime.h"
#include "add_on/scriptbuilder/scriptbuilder.h"
#include "asbind20/bind.hpp"
#include "asbind20/invoke.hpp"

int main()
{
    const auto engine = asbind20::make_script_engine();

    MyProject::BindScript(engine);

    MyProject::GenerateScriptPredefined(engine, "my_script/as.predefined");

    CScriptBuilder builder{};

    std::string moduleName{"my_script/my_script.as"};

    int r;
    r = builder.StartNewModule(engine, moduleName.c_str());
    assert(r >= 0);

    r = builder.AddSectionFromFile(moduleName.c_str());
    assert(r >=0);

    r = builder.BuildModule();
    assert(r >= 0);

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
    }

    module->Discard();
}

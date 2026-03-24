#include "ScriptSerializer.h"

namespace
{
    class Serializer_Array : public CUserType
    {
    public:
        void* AllocateUnitializedMemory(CSerializedValue* val) override
        {
            return {};
        }

        void Store(CSerializedValue* val, void* ptr) override
        {
        }

        void Restore(CSerializedValue* val, void* ptr) override
        {
        }
    };
}

namespace MyProject
{
    void RegisterScriptSerializer(CSerializer& serializer)
    {
        Serializer_Array serializer_array{};
        serializer.AddUserType(&serializer_array, "array");
    }
}

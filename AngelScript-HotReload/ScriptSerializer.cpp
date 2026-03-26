#include "ScriptSerializer.h"

#include <string>
#include <vector>

#include "angelscript.h"
#include "HostTypes.h"
#include "RefWrapper.h"
#include "add_on/scriptarray/scriptarray.h"
#include "add_on/scriptstdstring/scriptstdstring.h"

namespace
{
    using namespace MyProject;

    // Serializer for the AngelScript 'array' type (CScriptArray)
    class Serializer_Array : public CUserType
    {
    public:
        void* AllocateUnitializedMemory(CSerializedValue* val) override
        {
            asITypeInfo* type = val->GetType();
            return CScriptArray::Create(type);
        }

        void Store(CSerializedValue* val, void* ptr) override
        {
            CScriptArray* arr = static_cast<CScriptArray*>(ptr);
            for (asUINT i = 0; i < arr->GetSize(); ++i)
            {
                // Each element is stored as a child of the array serialized value
                auto* child = new CSerializedValue(val, "", "", arr->At(i), arr->GetElementTypeId());
                val->m_children.push_back(child);
            }
        }

        void Restore(CSerializedValue* val, void* ptr) override
        {
            CScriptArray* arr = static_cast<CScriptArray*>(ptr);
            arr->Resize(static_cast<asUINT>(val->m_children.size()));
            for (asUINT i = 0; i < arr->GetSize(); ++i)
            {
                val->m_children[i]->Restore(arr->At(i), arr->GetElementTypeId());
            }
        }
    };

    // Serializer for 'string' (std::string)
    class Serializer_String : public CUserType
    {
    public:
        void* AllocateUnitializedMemory(CSerializedValue* val) override
        {
            return new std::string();
        }

        void Store(CSerializedValue* val, void* ptr) override
        {
            std::string* s = static_cast<std::string*>(ptr);
            // Store the string content in user data for recovery
            val->SetUserData(new std::string(*s));
        }

        void Restore(CSerializedValue* val, void* ptr) override
        {
            std::string* s = static_cast<std::string*>(ptr);
            std::string* stored = static_cast<std::string*>(val->GetUserData());
            if (stored)
            {
                *s = *stored;
            }
        }

        void CleanupUserData(CSerializedValue* val) override
        {
            delete static_cast<std::string*>(val->GetUserData());
        }
    };

    // Serializer for the 'Fighter' value class
    class Serializer_Fighter : public CUserType
    {
    public:
        void* AllocateUnitializedMemory(CSerializedValue* val) override
        {
            return new Fighter();
        }

        void Store(CSerializedValue* val, void* ptr) override
        {
            Fighter* f = static_cast<Fighter*>(ptr);
            asITypeInfo* type = val->GetType();

            // Fighter has 'string name' (prop 0) and 'int life' (prop 1)
            int nameTypeId, lifeTypeId;
            type->GetProperty(0, nullptr, &nameTypeId);
            type->GetProperty(1, nullptr, &lifeTypeId);

            val->m_children.push_back(new CSerializedValue(val, "name", "", &f->name, nameTypeId));
            val->m_children.push_back(new CSerializedValue(val, "life", "", &f->life, lifeTypeId));
        }

        void Restore(CSerializedValue* val, void* ptr) override
        {
            Fighter* f = static_cast<Fighter*>(ptr);
            asITypeInfo* type = val->GetType();

            int nameTypeId, lifeTypeId;
            type->GetProperty(0, nullptr, &nameTypeId);
            type->GetProperty(1, nullptr, &lifeTypeId);

            val->m_children[0]->Restore(&f->name, nameTypeId);
            val->m_children[1]->Restore(&f->life, lifeTypeId);
        }
    };

    template <class T>
    class Serializer_RefWrapper : public CUserType
    {
    public:
        Serializer_RefWrapper(std::string typeDecl) : m_typeDecl(std::move(typeDecl))
        {
        }

        void* AllocateUnitializedMemory(CSerializedValue* val) override
        {
            // Create a new reference wrapper
            return new RefWrapper<T>();
        }

        void Store(CSerializedValue* val, void* ptr) override
        {
            RefWrapper<T>* ref = static_cast<RefWrapper<T>*>(ptr);
            asIScriptEngine* engine = val->GetType()->GetEngine();
            int typeId = engine->GetTypeIdByDecl(m_typeDecl.c_str());

            // Store the inner data as a child
            val->m_children.push_back(new CSerializedValue(val, "data", "", &ref->data(), typeId));
        }

        void Restore(CSerializedValue* val, void* ptr) override
        {
            RefWrapper<T>* ref = static_cast<RefWrapper<T>*>(ptr);
            asIScriptEngine* engine = val->GetType()->GetEngine();
            int typeId = engine->GetTypeIdByDecl(m_typeDecl.c_str());

            val->m_children[0]->Restore(&ref->data(), typeId);
        }

    private:
        std::string m_typeDecl{};
    };
}

namespace MyProject
{
    void RegisterScriptSerializer(CSerializer& serializer)
    {
        serializer.AddUserType(new Serializer_Array(), "array");
        serializer.AddUserType(new Serializer_String(), "string");
        serializer.AddUserType(new Serializer_Fighter(), "Fighter");
        serializer.AddUserType(new Serializer_RefWrapper<Fighter>("Fighter"), "FighterRef");
    }
}

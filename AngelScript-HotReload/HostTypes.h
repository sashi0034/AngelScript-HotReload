#pragma once
#include <string>
#include <vector>

namespace MyProject
{
    struct Fighter
    {
        std::string name{};

        int life{};
    };

    class FighterRef
    {
    public:
        FighterRef() = default;

        FighterRef(const Fighter& fighter) : m_fighter(fighter)
        {
        }

        void addRef()
        {
            ++m_refCount;
        }

        void release()
        {
            if (--m_refCount == 0)
            {
                delete this;
            }
        }

    private:
        int m_refCount{1};
        Fighter m_fighter{};
    };
}

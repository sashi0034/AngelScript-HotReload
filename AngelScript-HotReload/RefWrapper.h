#pragma once

namespace MyProject
{
    template <typename DataType>
    class RefWrapper
    {
    public:
        RefWrapper() = default;

        RefWrapper(DataType data) : m_data(std::move(data))
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

        DataType& data()
        {
            return m_data;
        }

        const DataType& data() const
        {
            return m_data;
        }

    private:
        int m_refCount{1};
        DataType m_data{};
    };
}

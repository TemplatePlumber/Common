#pragma once

namespace Tp
{
    class IdPool
    {
    private:
        std::set<size_t> _returnedIds = {};
        size_t _highestAllocatedId = 0;
    
    public:
        size_t get();
        void put(size_t id);
    };
}
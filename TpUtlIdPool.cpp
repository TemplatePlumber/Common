#include "TpUtlIdPool.h"

namespace Tp
{
    size_t IdPool::get()
    {
        if(!_returnedIds.empty())
        {
            size_t ret = *_returnedIds.begin();
            _returnedIds.erase(_returnedIds.begin());
            return ret;
        }
        
        return _highestAllocatedId++;
    }
    
    void IdPool::put(size_t id)
    {
        _returnedIds.insert(id);
        
        /*
            Clean up at the end of the returned list.
                Remove values until there is a gap between the
                highest returned id and the highest allocated id.
        */
        do
        {
            auto itr = _returnedIds.rbegin();
            if(*itr == _highestAllocatedId)
            {
                _highestAllocatedId--;
                _returnedIds.erase(itr.base());
            }
            else
            {
                break;
            }
        }
        while(!_returnedIds.empty());
    }
    
    bool IdPool::unput(size_t id)
    {
        if(id <= _highestAllocatedId)
        {
            auto itr = _returnedIds.find(id);
            if(itr == _returnedIds.end())
            {
                return false;
            }
            
            _returnedIds.erase(itr);
        }
        else while(id != _highestAllocatedId)
        {
            _returnedIds.insert(_highestAllocatedId);
            _highestAllocatedId++;
        }
        
        return true;
    }
}
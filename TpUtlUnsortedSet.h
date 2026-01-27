#pragma once

#include <stddef.h>
#include <vector>
#include <algorithm>

namespace Tp
{
    template<class T>
    class UnsortedSet
    {
        
        using _TData = std::vector<T>;
        _TData _data;
        
    public:
        using value_type             = T;
        using allocator_type         = std::allocator<T>;
        using size_type              = size_t;
        using difference_type        = std::ptrdiff_t;
        using reference              = value_type &;
        using const_reference        = const value_type &;
        using pointer                = std::allocator_traits<allocator_type>::pointer;
        using const_pointer          = std::allocator_traits<allocator_type>::const_pointer;
        using iterator               = typename _TData::iterator;
        using const_iterator         = typename _TData::const_iterator;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        using reverse_iterator       = std::reverse_iterator<iterator>;


        UnsortedSet() = default;
        UnsortedSet(const UnsortedSet & other) : _data(other._data) {}
        UnsortedSet(UnsortedSet && other) : _data(other._data) {}
        
        constexpr UnsortedSet & operator=(const UnsortedSet & other) { _data.operator=(other._data); return *this; }
        
        constexpr const_iterator          begin() const noexcept { return _data.begin();    }
        constexpr const_iterator            end() const noexcept { return _data.end();      }
        constexpr const_reverse_iterator rbegin() const noexcept { return _data.rbegin();   }
        constexpr const_reverse_iterator   rend() const noexcept { return _data.rend();     }
        constexpr size_type                size() const noexcept { return _data.size();     }
        constexpr size_type            max_size() const noexcept { return _data.max_size(); }
        constexpr bool                    empty() const noexcept { return _data.empty();    }

        constexpr const_reference operator[](size_type i) noexcept { return _data.operator[](i); }
        
        constexpr void insert(const value_type & v) 
        {
            if(!overlap(_data,v))
            {
                _data.push_back(v);                
            }
        }
        
        const_iterator find(const value_type & v)
        {
            return std::find(_data.begin(),_data.end(),v);
        }
        
        void erase(const_iterator v)
        {
            _data.erase(v);
        }
        
        void clear()
        {
            _data.clear();
        }
        
        operator _TData() const
        {
            return _data;
        }

        /* 
            Constraint breaking:
                constexpr reference operator[](size_type i) noexcept { return _data.operator[](i); }
                constexpr iterator begin() noexcept { return _data.begin(); }
                constexpr iterator end() noexcept { return _data.end(); }
                constexpr reverse_iterator rbegin() noexcept { return _data.rbegin(); }
                constexpr reverse_iterator rend() noexcept { return _data.rend(); }
        */
    };
}

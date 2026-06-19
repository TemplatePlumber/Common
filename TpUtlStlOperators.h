#pragma once

#include "TpUtlMetaTemplates.h"

namespace Tp
{
    template<typename T> requires CIterableContainer<T>
    T::value_type sum(const T & v1)
    {
        typename T::value_type ret = {};
        for(const auto & v2 : v1)
        {
            ret += v2;
        }
        return ret;
    }
    
    template<typename T> requires CInsertContainer<T>
    void append(T & v1,const typename T::value_type & v2)
    {
        v1.insert(v2);
    }
    
    template<typename T> requires CPushbackContainer<T>
    void append(T & v1,const typename T::value_type & v2)
    {
        v1.push_back(v2);
    }
    
    template<typename U, typename V> requires CIterableContainer<U> && CIterableContainer<V>
    void append(U & v1,const V & v2)
    {
        for(const auto & v3 : v2)
        {
            append(v1,v3);
        }
    }
    
    template<typename U, typename V> requires CIterableContainer<U>
    void merge(U & v1,const V & v2)
    {
        if constexpr(CIterableContainer<V>) for(const auto & v3 : v2)
        {
            append(v1,v3);
        }
        else
        {
            append(v1,v2);
        }
    }
    
    template<typename U, typename V> requires (!CIterableContainer<U>)
    void merge(const U & v1,V & v2)
    {
        if constexpr(CIterableContainer<V>)
        {
            merge(v2,v1);
        }
        else
        {
            static_assert(false,"Merging non-containers is not allowed.");
        }
    }

    template<typename T> requires CInsertContainer<T>    
    auto first(T & container)
    {
        return *container.begin();
    }
    
    template<typename T> requires CInsertContainer<T>
    bool overlap(T & v1,const typename T::value_type & v2)
    {
        return v1.find(v2) != v1.end();
    }
    
    template<typename T> requires CPushbackContainer<T>
    bool overlap(T & v1,const typename T::value_type & v2)
    {
        auto it = std::find(v1.begin(), v1.end(), v2);

        if (it != v1.end()) {
            return true;
        }
        return false;
    }
    
    template<typename T> requires (CFindEraseContainer<T> && CValueTypeContainer<T>)
    void remove(T & v1,const typename T::value_type & v2)
    {
        auto itr = v1.find(v2);
        if(itr != v1.end())
        {
            v1.erase(itr);
        }
    }
    
    template<typename T> requires (!CFindEraseContainer<T> && CValueTypeContainer<T>)
    void remove(T & v1,const typename T::value_type & v2)
    {
        auto it = std::find(v1.begin(), v1.end(), v2);

        if (it != v1.end()) {
            v1.erase(it);
        }
    }

    template<typename T,typename U> requires (!CFindEraseContainer<T> && !CValueTypeContainer<T> && CValueEraseContainer<T,U>)
    void remove(T & v1,const U & v2)
    {
        for(auto & v3 : v1)
        {
            if(v3 == v2)
            {
                v1.erase(v3);
                break;
            }
        }
    }
    
    template<typename U, typename V> requires CIterableContainer<U>
    void setDifference(U & v1,const V & v2)
    {
        if constexpr(CIterableContainer<V>) for(const auto & v3 : v2)
        {
            remove(v1,v3);
        }
        else
        {
            remove(v1,v2);
        }
    }
    
    template<typename U, typename V> requires (!CIterableContainer<U>)
    void setDifference(const U & v1,V & v2)
    {
        if constexpr(CIterableContainer<V>)
        {
            setDifference(v2,v1);
        }
        else
        {
            static_assert(false,"setDifference on non-containers is not allowed.");
        }
    }
}

template<typename U, typename V> requires Tp::CIterableContainer<U> || Tp::CIterableContainer<V>
U operator-(const U & v1, const V & v2)
{
    auto v3 = v1;
    Tp::setDifference(v3,v2);
    return v3;
}

template<typename T,typename U> requires (Tp::CIterableContainer<T>)
void operator-=(T & v1, const U & v2)
{
    Tp::setDifference(v1,v2);
}

template<typename U, typename V> requires Tp::CIterableContainer<U> || Tp::CIterableContainer<V>
U operator+(const U & v1, const V & v2)
{
    auto v3 = v1;
    Tp::merge(v3,v2);
    return v3;
}


template<typename T> requires Tp::CIterableContainer<T>
void operator+=(T & v1, const typename T::value_type & v2)
{
    Tp::merge(v1,v2);
}

template<typename U, typename V> requires Tp::CIterableContainer<U> && Tp::CIterableContainer<V>
U operator&(const U & v1, const V & v2)
{
    U ret = {};
    for(const auto & v3 : v1)
    {
        for(const auto & v4 : v2)
        {
            if(v3 == v4)
            {
                Tp::append(ret,v3);
            }
        }
    }
    return ret;
}


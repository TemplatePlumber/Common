#pragma once

namespace Tp
{
    template<typename T>
    concept CStdString = requires (T x){x.substr(0);};
 
    template<typename T>
    concept CIterableContainer = !CStdString<T> && requires (T x){x.begin();};
   
    template<typename T>
    concept CInsertContainer =  CIterableContainer<T> && requires (T x, typename T::value_type v){x.insert(v);};

    template<typename T>
    concept CPushbackContainer =  CIterableContainer<T> && requires (T x, typename T::value_type v){x.push_back(v);};

    template<typename T>
    concept CMapContainer = CIterableContainer<T> && requires (T x){ typename T::mapped_type; };

    template<typename T>
    concept CListContainer = CIterableContainer<T> && !CMapContainer<T> && requires (T x){ typename T::value_type; };
    
    template <typename T>
    concept CStringStreamConvertible = requires(std::ostream os, T value) {{ os << value };};
    
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
    
    template<typename T> requires CInsertContainer<T>
    void remove(T & v1,const typename T::value_type & v2)
    {
        auto itr = v1.find(v2);
        if(itr != v1.end())
        {
            v1.erase(itr);
        }
    }
    
    template<typename T> requires CPushbackContainer<T>
    void remove(T & v1,const typename T::value_type & v2)
    {
        auto it = std::find(v1.begin(), v1.end(), v2);

        if (it != v1.end()) {
            v1.erase(it);
        }
    }
}

template<typename T> requires Tp::CIterableContainer<T>
void operator-=(T & v1, const typename T::value_type & v2)
{
    Tp::remove(v1,v2);
}

template<typename U, typename V> requires Tp::CIterableContainer<U> || Tp::CIterableContainer<V>
U operator+(const U & v1, const V & v2)
{
    auto v3 = v1;
    Tp::append(v3,v2);
    return v3;
}


template<typename T> requires Tp::CIterableContainer<T>
void operator+=(T & v1, const typename T::value_type & v2)
{
    Tp::append(v1,v2);
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


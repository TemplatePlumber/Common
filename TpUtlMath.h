#pragma once

#include <type_traits>

#include "TpUtlMetaTemplates.h"

namespace Tp
{
    template<typename T>
    T max(T v1)
    {
        return v1;
    }
    
    template<typename T,typename ... Ts>
    T max(T v1, Ts ... vn)
    {
        auto maxVn = max(vn ...);
        if(v1 > maxVn)
        {
            return v1;
        }
        return maxVn;
    }
    
    template<typename T>
    T min(T v1)
    {
        return v1;
    }
    
    template<typename T,typename ... Ts>
    T min(T v1, Ts ... vn)
    {
        auto minVn = min(vn ...);
        if(v1 < minVn)
        {
            return v1;
        }
        return minVn;
    }
    
    template<typename T> 
    auto abs(const T & v1)
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            return std::fabs(v1);
        }
        else
        {
            return std::abs(v1);
        }
    }
    
    template<typename U,typename V> requires (std::is_floating_point_v<Tp::BaseType<U>> || std::is_floating_point_v<Tp::BaseType<V>>)
    bool areEqual(const U & v1, const V & v2, U epsilon = 0.0001)
    {
        return Tp::abs(v1 - v2) < epsilon;
    }
    
    template<typename U,typename V> requires (!(std::is_floating_point_v<Tp::BaseType<U>> || std::is_floating_point_v<Tp::BaseType<V>>))
    bool areEqual(const U & v1, const V & v2)
    {
        return v1 == v2;
    }
    
    template<typename T> requires (std::is_integral_v<T>)
    T pow(T base, T exp)
    {
        if (exp < 0) {
            if (base == 1) { return 1; }
            else if (base == -1)
            {
                if(exp % 2 == 0) 
                { 
                    return 1;
                } 
                else 
                {
                    return -1;
                }
            }
            return 0; // Integer division truncates 1 / (base^exp) to 0
        }
        
        T ret = 1;
        while (exp > 0)
        {
            if (exp & 1) { ret *= base; }
            base *= base;
            exp >>= 1;
        }
        return ret;
    }
    
    template<typename T>
    bool isClamped(const T & v1, const T & lo, const T & hi)
    {
        if(v1 >= lo && v1 <= hi)
        {
            return true;
        }
        
        return false;
    }
}
#pragma once

#include <type_traits>

#include "TpUtlMetaTemplates.h"

namespace Tp
{
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
}
#pragma once

#include <type_traits>
#include <ostream>

#define TEMPLATE_MEMBER(x,y) x::template y

namespace Tp
{
    template<typename T>
    using BaseType = typename std::remove_pointer< std::remove_cvref_t<T> >::type;

    template<typename U, typename V>
    concept CSameBaseType = std::is_same_v<BaseType<U>,BaseType<V>>;
    
    template<typename T>
    concept CStdString = requires (T x){x.substr(0);};

    template<typename T>
    concept COptionalContainer = !CStdString<T> && requires (T x){ x.value(); x.has_value(); };
 
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
    
    namespace MetaTypes
    {
        namespace Dt
        {
            //template<typename T>
            
        }
        
        // Primary template, defaults to false
        template <typename T1, typename T2>
        struct HaveSameTemplate : std::false_type {};
        
        template <template <typename...> typename Template, typename... Args1, typename... Args2>
        struct HaveSameTemplate<Template<Args1...>, Template<Args2...>> : std::true_type {};
        
        template <typename T1, typename T2>
        concept CHaveSameTemplate = HaveSameTemplate<T1, T2>::value;
        
        // 1. Primary template (default: false)
        template <template <typename...> class TEMPLATE, typename T>
        struct IsInstanceOfTemplate : std::false_type {};

        // 2. Partial specialization (match: true)
        template <template <typename...> class TEMPLATE, typename... Args>
        struct IsInstanceOfTemplate<TEMPLATE, TEMPLATE<Args...>> : std::true_type {};

        // Helper variable template (C++17)
        template <template <typename...> class TEMPLATE, typename T>
        concept CIsInstanceOfTemplate = IsInstanceOfTemplate<TEMPLATE, BaseType<T>>::value;

        
        template<template<typename ... VTs> typename TEMPLATE_T,typename ... ARG_Ts>
        class Template
        {
        public:
            template<typename ... SUBSTITUTED_Ts>
            using Substitute = TEMPLATE_T<SUBSTITUTED_Ts ...>;
            
        };
        
        template<typename T>
        class FunctionType;
        
        template<typename RET_T, typename ... ARG_Ts>
        class FunctionType<RET_T (*)(ARG_Ts...)>
        {
        public:
            using ReturnType = RET_T;
        };
    };
}
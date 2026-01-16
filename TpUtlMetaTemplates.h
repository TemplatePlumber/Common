#pragma once

#define TEMPLATE_MEMBER(x,y) x::template y

namespace Tp
{
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
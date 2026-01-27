#pragma once

#include <ranges>

#include "TpUtlStlOperators.h"
#include "TpUtlCommon.h"

namespace Tp
{
    /*
        Unroll a for-loop over each template parameter.
    */
    namespace Dt
    {
        template <typename LAMBDA_T, auto... value>
        struct ForEachHelper {};

        template <typename LAMBDA_T, auto value>
        struct ForEachHelper<LAMBDA_T, value>
        {
            constexpr static void call(LAMBDA_T fnc)
            {
                fnc.template operator()<value>();
            }
        };
        
        template <typename LAMBDA_T, auto value, auto... values>
        struct ForEachHelper<LAMBDA_T, value, values...>
        {
            constexpr static void call(LAMBDA_T fnc)
            {
                fnc.template operator()<value>();
                ForEachHelper<LAMBDA_T, values...>::call(fnc);
            }
        };
    }

    template <auto value, auto... values, typename LAMBDA_T>
    constexpr void forEach(LAMBDA_T fnc)
    {
        Dt::ForEachHelper<LAMBDA_T, value, values...>::call(fnc);
    }
    
    
    namespace Dt
    {
        template <typename LAMBDA_T, typename ... Ts>
        struct ForEachTypeHelper {};

        template <typename LAMBDA_T, typename T>
        struct ForEachTypeHelper<LAMBDA_T, T>
        {
            constexpr static void call(LAMBDA_T fnc){ fnc.template operator()<T>(); }
        };
        
        template <typename LAMBDA_T, typename T, typename ... Ts>
        struct ForEachTypeHelper<LAMBDA_T, T, Ts...>
        {
            constexpr static void call(LAMBDA_T fnc)
            {
                fnc.template operator()<T>();
                ForEachTypeHelper<LAMBDA_T, Ts...>::call(fnc);
            }
        };
    }
    
    template <typename T, typename ... Ts, typename LAMBDA_T>
    constexpr void forEachType(LAMBDA_T fnc)
    {
        Dt::ForEachTypeHelper<LAMBDA_T, T, Ts...>::call(fnc);
    }
    
    
    struct Pass{};
    template<typename T> void PassIf(const T & condition) { if(condition) { throw Pass{}; } };
    template<typename T> T * PassIfNull(T * ptr) { if(ptr == nullptr) { throw Pass{}; } return ptr; };

    template<typename T, typename FNC_T> requires CIterableContainer<T>
    Opt<typename T::value_type> minimize(const T & v1, FNC_T fnc)
    {
        using TValueType = typename T::value_type;
        using TMinValue = std::invoke_result_t<FNC_T,typename T::value_type>;
        if(v1.empty()){ return Pending; }
        
        Opt<TValueType> ret = Pending;
        TMinValue least;
        
        for(const auto & v2 : v1)
        {
            try
            {
                auto val = fnc(v2);
                if(ret == Pending || val < least)
                {
                    least = val;
                    ret = v2;
                }
            }
            catch(Pass pass){}
        }
        
        return ret;
    }
    
    template<typename T>
    auto enumerate(T & value)
    {
        return std::views::zip(std::views::iota(0), value);
    }

    template<template <typename> typename CONTAINER_T, typename VALUE_T, typename FNC_T>
    auto transform(const CONTAINER_T<VALUE_T> & container, FNC_T function)
    {
        using RET_VT = std::invoke_result_t<decltype(function), VALUE_T>;
        CONTAINER_T<RET_VT> ret;
        for(const auto & value : container)
        {
            try{ Tp::append(ret,function(value)); } catch(const Pass & pass){}
        }
        return ret;
    }

    template<typename CONTAINER_T, typename FNC_T> //requires CIterableContainer<CONTAINER_T>
    auto filter(const CONTAINER_T & container, FNC_T function)
    {
        if constexpr (CIterableContainer<CONTAINER_T>)
        {
            CONTAINER_T ret;
            for(const auto & value : container)
            {
                if(!function(value))
                {
                    Tp:append(ret,value);
                }
            }
            return ret;
        }
        else
        {
            Opt<CONTAINER_T> ret;
            if(!function(container))
            {
                ret = Tp::Pending;
            }
            else
            {
                ret = container;
            }
            return ret;
        }
    }
    
    //template<typename T> auto yieldFirstOrPass(const T & value);
    
    //template<typename CONTAINER_T> requires CIterableContainer<CONTAINER_T>
    //typename CONTAINER_T::value_type yieldFirstOrPass(const CONTAINER_T & value)
    //{
    //    if(!value.empty())
    //    {
    //        auto ret = *value.begin();
    //        return ret;
    //    }
    //    throw Pass{};
    //}
    
    template<typename T>
    auto yieldFirstOrPass(const Opt<T> & value)
    {
        if(!value)
        {
            throw Pass{};
        }
        return value.value();
    }
    
    template<typename CONTAINER_T,typename T>
    CONTAINER_T except(const CONTAINER_T & container, const T & value)
    {
        auto cpy = container;
        size_t oldSize;
        size_t newSize;
        do
        {
            oldSize = cpy.size();
            Tp::remove(cpy,value);
            newSize = cpy.size();
        }
        while(oldSize != newSize);
        return cpy;
    }
    
    struct Transformer{};

    #define TP_DECLARE_TRANSFORMER_0(NAME,FUNCTION) \
        template<typename T> \
        struct NAME : public Transformer \
        { \
            constexpr NAME(T v1) {} \
            \
            template<typename ... Ts> \
            constexpr auto activate(Ts ... args) const { return FUNCTION(args...); } \
        };
    
    #define TP_DECLARE_TRANSFORMER_1(NAME,FUNCTION) \
        template<typename T> \
        struct NAME : public Transformer \
        { \
            const T _v1; \
            constexpr NAME(T v1) : _v1(v1) {} \
            \
            template<typename ... Ts> \
            constexpr auto activate(Ts ... args) const { return FUNCTION(args...,_v1); } \
        };

    
    TP_DECLARE_TRANSFORMER_1(Transform,transform);
    TP_DECLARE_TRANSFORMER_1(Except,except);
    TP_DECLARE_TRANSFORMER_1(Filter,filter);
    TP_DECLARE_TRANSFORMER_0(YieldFirstOrPass,yieldFirstOrPass);
    TP_DECLARE_TRANSFORMER_0(Enumerate,enumerate);
    
    
    template<typename TRANSFORMER_T,typename CONTAINER_T> requires std::derived_from<TRANSFORMER_T,Transformer>
    constexpr auto operator|(const CONTAINER_T & container, TRANSFORMER_T transformer)
    {
        return transformer.activate(container);
    }

    /*
        Unroll a for-loop over the given range of [from,to).
    */
    namespace Dt
    {

        template <auto FROM, auto TO, typename LAMBDA_T>
        constexpr void forEachInRangeHelper(LAMBDA_T fnc)
        {
            if constexpr(FROM < TO)
            {
                fnc.template operator()<FROM>();
                forEachInRangeHelper<FROM+1,TO,LAMBDA_T>(fnc);
            }
            else if constexpr(FROM == TO)
            {
                return;
            }
            else
            {
                fnc.template operator()<FROM>();
                forEachInRangeHelper<FROM-1,TO,LAMBDA_T>(fnc);
            }
        }
    }

    template <auto FROM, auto TO, typename LAMBDA_T>
    constexpr void forEachInRange(LAMBDA_T fnc)
    {
        Dt::forEachInRangeHelper<FROM, TO, LAMBDA_T>(fnc);
    }


    struct AssertException{ const std::string & msg; };
}

#define TP_RETURN_IF(CONDITION,...) if(CONDITION){ return __VA_ARGS__; }
#define TP_ERR_RETURN_IF(CONDITION,...) if(CONDITION){ std::cout << "Condition failed: " #CONDITION << std::endl; return __VA_ARGS__; }
#define TP_LOG(FMT,...)
#define TP_ERROR_CHECKING_BGN try {
#define TP_CHECK(condition) if(!condition){throw Tp::AssertException{.msg = #condition}; }
#define TP_ERROR_CHECKING_END \
    }\
    catch(const Tp::AssertException & exception) \
    {\
        TP_LOG("Error: {}",exception.msg);\
    }

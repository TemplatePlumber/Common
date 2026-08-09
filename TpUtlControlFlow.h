#pragma once

#include <ranges>
#include <coroutine>
#include <random>
#include <chrono>

#include "TpUtlStlOperators.h"
#include "TpUtlCommon.h"
#include "TpUtlPreprocessor.h"

namespace Tp
{
    /* For non-error exceptions */
    struct Escape {};
    
    
    /*
        Type casting
    */
    template<typename TO,typename FROM>
    struct ExplicitCast {};

    template<template<typename> typename TO,template<typename> typename FROM,typename T>
    TO<T> adapt(const FROM<T> & v1)
    {
        return ExplicitCast<TO<T>,FROM<T>>::exec(v1);
    }
    
    template<typename TO,typename FROM>
    TO adapt(const FROM & v1)
    {
        return ExplicitCast<TO,FROM>::exec(v1);
    }
    
    // Keyword: Adapt
    struct _Adapt{ _Adapt() = default; };
    inline const _Adapt Adapt;

    // Syntax: var | Adapt()
    template<typename FROM_T>
    struct To_Or_Adapt {
        const FROM_T & _ref;
        To_Or_Adapt(const FROM_T & ref):_ref(ref){}
        
        template<typename TO_T>
        operator TO_T() const{
            return Tp::adapt<TO_T,FROM_T>(_ref);
        }
    };

    template<typename FROM_T>
    To_Or_Adapt<FROM_T> operator|(const FROM_T & from, _Adapt adapt) {
        return To_Or_Adapt<FROM_T>(from);
    }

    
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
        if(v1.empty()){ return Undefined; }
        
        TMinValue least = {};
        const TValueType * leastPtr = nullptr;
        
        for(const auto & v2 : v1)
        {
            try
            {
                auto val = fnc(v2);
                if(!leastPtr || val < least)
                {
                    least = val;
                    leastPtr = &v2;
                }
            }
            catch(Pass pass){}
        }
        
        if(leastPtr)
        {
            return *leastPtr;
        }
        return Undefined;
    }
    
    template<typename T>
    auto enumerateFnc(T & value)
    {
        return std::views::zip(std::views::iota(0), value);
    }

    template<typename CONTAINER_T, typename FNC_T>
    auto transform(const CONTAINER_T & container, FNC_T function)
    {
        using IN_VALUE_T = Tp::BaseType<decltype(*container.begin())>;
        using OUT_VALUE_T = std::invoke_result_t<decltype(function), IN_VALUE_T>;
        std::vector<OUT_VALUE_T> ret;
        for(const auto & value : container)
        {
            try{ Tp::append(ret,function(value)); } catch(const Pass & pass){}
        }
        
        return ret;
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
    
#define TP_TRANSFORM(FUNCTION) Tp::Transform([&](auto x){return FUNCTION(x);});

    //Filters on true
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
                    append(ret,value);
                }
            }
            return ret;
        }
        else
        {
            Opt<CONTAINER_T> ret;
            if(!function(container))
            {
                ret = container;
            }
            else
            {
                ret = Tp::Undefined;
            }
            return ret;
        }
    }
    
    template<typename CONTAINER_T>
    auto shuffle(const CONTAINER_T & container, size_t seed=std::chrono::system_clock::now().time_since_epoch().count())
    {
        Vec<typename CONTAINER_T::value_type> ret;
        Tp::merge(ret,container);
        static std::random_device rd;
        static std::mt19937 g(rd());
        g.seed(seed);
        
        std::shuffle(ret.begin(), ret.end(), g);
        return ret;
    }
    
    
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

    #define TP_DECLARE_TRANSFORMER_0(NAME,INSTANCE,FUNCTION) \
        struct NAME : public Transformer \
        { \
            constexpr NAME() {} \
            \
            template<typename ... Ts> \
            constexpr auto activate(Ts ... args) const { return FUNCTION(args...); } \
        };\
        inline constexpr const NAME INSTANCE;
    
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
//    TP_DECLARE_TRANSFORMER_0(Enumerate,enumerate,enumerateFnc);
    inline auto & enumerate = std::views::enumerate;
    
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


    struct CriticalException{ const char * file; const char * line; const std::string & msg; };
    
    /*
        Coroutine Objects
    */
    template<typename T>
    struct DeferredTask {
        struct promise_type {
            int final_result = 0; 
            DeferredTask get_return_object() { return DeferredTask{std::coroutine_handle<promise_type>::from_promise(*this)}; }
            std::suspend_always initial_suspend() { return {}; }
            std::suspend_always final_suspend() noexcept { return {}; }
            void return_value(T value) {}
            void unhandled_exception() { std::terminate(); }
        };
        
        DeferredTask & operator=(DeferredTask && other) noexcept
        {
            if (this != &other) {
                if (handle) handle.destroy();
                handle = other.handle;
                other.handle = nullptr;
            }
            return *this;
        }

        DeferredTask() = default;
        DeferredTask(std::coroutine_handle<promise_type> h) : handle(h) {}
        DeferredTask(DeferredTask && other) noexcept : handle(other.handle) { other.handle = nullptr; }
        ~DeferredTask() { if (handle) { handle.destroy(); } } // Clean up memory
        

        std::coroutine_handle<promise_type> handle;
        
        // await_ready - Declare that DeferredTask is awaitable, to allow nested coroutines
        bool await_ready()
        {
            std::cout << "READY" << std::endl;
            // Nothing to await - resume parent coroutine immediately
            if (!handle) { return true; }
            
            // Do not suspend the parent coroutine if the nested coroutine finishes
            return handle.done();
        } 
        
        // await_suspend - DeferredTask begins immediately if called inside of a coroutine.
        void await_suspend(std::coroutine_handle<> caller_handle)
        {
            std::cout << "RESUME" << std::endl;
            handle.resume(); 
        }
    };

    template<>
    struct DeferredTask<void> {
        // 2. The nested promise_type required by the compiler
        struct promise_type {
            DeferredTask get_return_object() { return DeferredTask{std::coroutine_handle<promise_type>::from_promise(*this)}; }
            std::suspend_always initial_suspend() { return {}; }
            std::suspend_always final_suspend() noexcept { return {}; }
            void return_void() {}
            void unhandled_exception() { std::terminate(); }
        };

        DeferredTask & operator=(DeferredTask && other) noexcept
        {
            if (this != &other) {
                if (handle) handle.destroy();
                handle = other.handle;
                other.handle = nullptr;
            }
            return *this;
        }

        DeferredTask() = default;
        DeferredTask(std::coroutine_handle<promise_type> h) : handle(h) {}
        DeferredTask(DeferredTask && other) noexcept : handle(other.handle) { other.handle = nullptr; }
        ~DeferredTask() { if (handle) { handle.destroy(); } } // Clean up memory
        

        std::coroutine_handle<promise_type> handle;
        
        // await_ready - Declare that DeferredTask is awaitable, to allow nested coroutines
        bool await_ready()
        {
            std::cout << "READY" << std::endl;
            // Nothing to await - resume parent coroutine immediately
            if (!handle) { return true; }
            
            // Do not suspend the parent coroutine if the nested coroutine finishes
            return handle.done();
        } 
        
        // await_suspend - DeferredTask begins immediately if called inside of a coroutine.
        void await_suspend(std::coroutine_handle<> caller_handle)
        {
            std::cout << "RESUME" << std::endl;
            handle.resume(); 
        }
    };
    
    
    // Rate limiting
    #define TP_RATE_LIMIT(LIMIT,IDX,...)\
    {\
        static Tp::RateLimiter TP_PP_CAT(RSVD_RL_,__LINE__)( LIMIT,[&](){ __VA_ARGS__ ; });\
        TP_PP_CAT(RSVD_RL_,__LINE__).exec(IDX);\
    }
    
    
    template<typename FUNC_T>
    struct RateLimiter
    {
        const u64 _period;
        FUNC_T _func;
        HMap<u64,u64> _invokationsByIndex;
        RateLimiter(u64 period,FUNC_T func) : _period(period+1),_func(func){}
        void exec(u64 index)
        {
            auto & invokations = _invokationsByIndex[index];
            if(invokations == 0)
            {
                _func();
            }
            invokations = (invokations + 1) % _period;
        }
    };
}

#define TP_RETURN_IF(CONDITION,...) if(CONDITION){ return __VA_ARGS__; }
#define TP_ERR_RETURN_IF(CONDITION,...) if(CONDITION){ std::cout << "Condition failed: " #CONDITION << std::endl; return __VA_ARGS__; }
#define TP_ERR_IF(CONDITION) if(CONDITION){ std::cout << "Condition failed: " #CONDITION << std::endl; }
#define TP_LOG(FMT,...)
#define TP_CHECK(condition) if(!(condition)){throw Tp::CriticalException{.file=__FILE__, .line=TP_PP_STR(__LINE__), .msg = #condition}; }


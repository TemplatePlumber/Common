#pragma once

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
#define TP_LOG(FMT,...)
#define TP_ERROR_CHECKING_BGN try {
#define TP_CHECK(condition) if(!condition){throw Tp::AssertException{.msg = #condition}; }
#define TP_ERROR_CHECKING_END \
    }\
    catch(const Tp::AssertException & exception) \
    {\
        TP_LOG("Error: {}",exception.msg);\
    }

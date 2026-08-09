#pragma once

#include <type_traits>
#include <ostream>

#define TEMPLATE_MEMBER(x,y) x::template y

namespace Tp
{
    // Stands for 'template value wrapper'.
    //   Templates are difficult to work with when mixing type and value parameters together.
    //   Instead, convert value parameters to type parameters within template declarations.
    template<auto V>
    struct TVWrapper {
        static constexpr const decltype(V) value = V;
    };
    
    template<typename T>
    struct TTWrapper {
        using Type = T;
    };
    
    template<typename T>
    using BaseType = typename std::remove_pointer< std::remove_cvref_t<T> >::type;

    template<typename U, typename V>
    concept CSameBaseType = std::is_same_v<BaseType<U>,BaseType<V>>;
    
    template<typename T>
    concept CStdString = requires (T x){x.substr(0);};

    template<typename T>
    concept COptionalContainer = !CStdString<T> && requires (T x){ x.value(); x.has_value(); };
 
    template<typename T>
    concept CValueTypeContainer = requires(T x){ typename T::value_type; };
 
    template<typename T>
    concept CIterableContainer = !CStdString<T> && requires (T x){x.begin();};
   
    template<typename T>
    concept CInsertContainer =  CIterableContainer<T> && requires (Tp::BaseType<T> x, typename T::value_type v){x.insert(v);};

    template<typename T>
    concept CFindEraseContainer =  CIterableContainer<T> && CValueTypeContainer<T> && requires (Tp::BaseType<T> x, typename T::value_type v){x.erase(x.find(v));};

    template<typename T>
    concept CPushbackContainer =  CIterableContainer<T> && requires (Tp::BaseType<T> x, typename T::value_type v){x.push_back(v);};

    template<typename T>
    concept CMapContainer = CIterableContainer<T> && requires (T x){ typename T::mapped_type; };

    template<typename T>
    concept CListContainer = CIterableContainer<T> && !CMapContainer<T> && requires (T x){ typename T::value_type; };
    
    template<typename T>
    concept CTupleLikeContainer = requires (T x){ std::get<0>(x); };

    template<typename T>
    concept CIsAggregate = (CIterableContainer<T> || CTupleLikeContainer<T>);
    
    template<typename U, typename V>
    concept CValueEraseContainer = requires(Tp::BaseType<U> u, V v){u.erase(v);};
    
    template <typename T>
    concept CStringStreamConvertible = requires(std::ostream os, T value) {{ os << value };};
    
    template <typename T>
    struct IsMethodPointer : std::false_type {};

    template <typename ReturnType, typename ClassName, typename... Args>
    struct IsMethodPointer<ReturnType (ClassName::*)(Args...)> : std::true_type {};

    template <typename ReturnType, typename ClassName, typename... Args>
    struct IsMethodPointer<ReturnType (ClassName::*)(Args...) const> : std::true_type {};

    template <typename T>
    concept CIsMethodPointer = IsMethodPointer<T>::value;
    
    template <typename T>
    struct IsMemberPointer : std::false_type {};

    template <typename HOLDER_T, typename MBR_T> requires (!std::is_function_v<MBR_T>)
    struct IsMemberPointer<MBR_T HOLDER_T::*> : std::true_type {};

    template <typename T>
    concept CIsMemberPointer = IsMemberPointer<T>::value;
    
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
        
        template <template <typename...> class TEMPLATE, typename T>
        struct IsInstanceOfTemplate : std::false_type {};

        template <template <typename...> class TEMPLATE, typename... Args>
        struct IsInstanceOfTemplate<TEMPLATE, TEMPLATE<Args...>> : std::true_type {};

        template <template <typename...> class TEMPLATE, typename T>
        concept CIsInstanceOfTemplate = IsInstanceOfTemplate<TEMPLATE, BaseType<T>>::value;

        
        template<template<typename ... VTs> typename TEMPLATE_T,typename ... ARG_Ts>
        struct Template
        {
        public:
            template<typename ... SUBSTITUTED_Ts>
            using Substitute = TEMPLATE_T<SUBSTITUTED_Ts ...>;
            
        };
        
        template<typename RET_T,typename HOLDER_T,typename ... ARG_Ts>
        struct FunctionPointerType
        {
            using Return_t = RET_T;
            RET_T (*pointer)(ARG_Ts ...) ;
        };  
              
        template<auto PTR>
        struct FunctionPointer
        {
            using Info_t = decltype(FunctionPointerType{.pointer=PTR});
            using Return_t = typename Info_t::Return_t;
        };
        
        template<typename RET_T,typename HOLDER_T,typename ... ARG_Ts>
        struct MethodPointerType
        {
            using Holder_t = HOLDER_T;
            using Return_t = RET_T;
            RET_T (HOLDER_T::*pointer)(ARG_Ts ...) ;
        };  
              
        template<auto PTR>
        struct MethodPointer
        {
            using Info_t = decltype(MethodPointerType{.pointer=PTR});
            using Holder_t = typename Info_t::Holder_t;
            using Return_t = typename Info_t::Return_t;
            using Pointer_t = decltype(Info_t::pointer);
        };
        
        template<typename T>
        struct MemberPointerType2;
    
        template<typename HOLDER_T, typename MBR_T>
        struct MemberPointerType2<MBR_T HOLDER_T::*>
        {
            using Holder_t = HOLDER_T;
            using Member_t = MBR_T;
        };
        
        template<typename HOLDER_T, typename MBR_T>
        struct MemberPointerType
        {
            using Holder_t = HOLDER_T;
            using Member_t = MBR_T;
            MBR_T HOLDER_T::* pointer;
        };
        
        template<auto PTR>
        struct MemberPointer
        {
            using Info_t = decltype(MemberPointerType{.pointer=PTR});
            using Holder_t = typename Info_t::Holder_t;
            using Member_t = typename Info_t::Member_t;
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
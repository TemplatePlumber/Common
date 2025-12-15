#pragma once

#include <cstddef>
#include <string>
#include <type_traits>
#include <iostream>

#include "TpUtlTypeTemplates.h"
#include "TpUtlControlFlow.h"
#include "TpUtlMetaTemplates.h"



namespace Tp
{
    namespace Dt
    {
        //Base case where the compiler searched for a specialization of DESCRIPTOR_INFO_TEMPLATE with the typename RSVD_COUNTING_SPECIALIZATIONS but did not find one.
        template <size_t INDEX, typename RESULT_IDENTIFIER, template <size_t, typename> typename DESCRIPTOR_INFO_TEMPLATE>
        struct CountPartialSpecializationsHelper
        {
            static constexpr size_t value = 0;
        };

        /*
            Partial specialization for the case where 'DESCRIPTOR_INFO_TEMPLATE' (aka RSVD_DESCRIPTOR_INFO_T) has a member called RSVD_COUNTING_SPECIALIZATIONS.
            
            This is the place where the magic lives.
            When this template instanced, the compiler will only know about partial specializations of DESCRIPTOR_INFO_TEMPLATE that are in scope.

            The compiler does not scan the whole compilation unit when looking for partial specializations.
            The compiler does not scan the whole compilation unit when looking for partial specializations.
            The compiler does not scan the whole compilation unit when looking for partial specializations.
            
            Example:
                line 0: declare template<typename T> Foo<T,0>
                line 1: declare template<typename T> Foo<T,1>
                line 2; cout << CountPartialSpecializationsHelper<0,Bar1,Foo>; //Does not see line 3, Foo<Bar,2> is an empty struct, prints 2
                line 3: declare template<typename T> Foo<T,2>
                line 4: cout << CountPartialSpecializationsHelper<0,Bar2,Foo>; //Foo<Bar,2> has a member called RSVD_COUNTING_SPECIALIZATIONS, prints 3
        */
        template <size_t INDEX, typename RESULT_IDENTIFIER, template <size_t, typename> typename DESCRIPTOR_INFO_TEMPLATE> requires requires { typename DESCRIPTOR_INFO_TEMPLATE<INDEX, RESULT_IDENTIFIER>::RSVD_COUNTING_SPECIALIZATIONS; }
        struct CountPartialSpecializationsHelper<INDEX, RESULT_IDENTIFIER, DESCRIPTOR_INFO_TEMPLATE>
        {
            static constexpr size_t value = 1 + CountPartialSpecializationsHelper<INDEX + 1, RESULT_IDENTIFIER, DESCRIPTOR_INFO_TEMPLATE>::value;
        };

        /*
            Not a general utility. Only works for one very specific template declaration.

            DESCRIPTOR_INFO_TEMPLATE - The template against which partial specializations will be either counted or remembered.
            RESULT_IDENTIFIER - May be any arbitrary typename. 
                If a unique 2-tuple of {DESCRIPTOR_INFO_TEMPLATE,RESULT_IDENTIFIER} is provided this function will simply return the number of partial specializations in scope for DESCRIPTOR_INFO_TEMPLATE.
                Otherwise, this function will recover the value of the previous invokation of this function with {DESCRIPTOR_INFO_TEMPLATE,RESULT_IDENTIFIER}.
        */
        template <typename RESULT_IDENTIFIER, template <size_t, typename> typename DESCRIPTOR_INFO_TEMPLATE>
        struct CountPartialSpecializations
        {
            static constexpr size_t value = CountPartialSpecializationsHelper<0, RESULT_IDENTIFIER, DESCRIPTOR_INFO_TEMPLATE>::value;
        };
    }


    namespace Reflect
    {
        template <typename HOLDER_T, size_t INDEX>
        using DescriptorHead_t = typename HOLDER_T::template RSVD_DESCRIPTOR_INFO_T<INDEX, struct RSVD_COUNTING_TOKEN>;

        template <typename HOLDER_T, size_t INDEX>
        auto & getOffset()
        {
            using Head_t = DescriptorHead_t<HOLDER_T,INDEX>;
            constexpr const auto & descriptor = Head_t::descriptor;
            return descriptor.common.template memberPointer<HOLDER_T>;
        }

        template <typename HOLDER_T,size_t INDEX>
        const auto & getMemberName()
        {
            return DescriptorHead_t<HOLDER_T,INDEX>::descriptor.common.memberName;
        }

        template <size_t INDEX, typename HOLDER_T>
        const auto & getMemberValue(const HOLDER_T & holder)
        {
            constexpr auto ptr = DescriptorHead_t<HOLDER_T,INDEX>::descriptor.common.template memberPointer<HOLDER_T>;
            return holder.*ptr;
        }

        template<typename HOLDER_T>
        constexpr size_t getDescriptorCount()
        {
            return ::Tp::Dt::CountPartialSpecializations<struct RSVD_RESULT_IDENTIFIER, TEMPLATE_MEMBER(HOLDER_T,RSVD_DESCRIPTOR_INFO_T)>::value;
        }

        template<typename HOLDER_T, typename DESCRIPTOR_T, typename LAMBDA_T>
        size_t forEachDescriptor(LAMBDA_T fnc)
        {
            constexpr auto numDescriptors = getDescriptorCount<HOLDER_T>();
            
            forEachInRange<0,numDescriptors>([&]<auto INDEX>(){
                using Head_t = DescriptorHead_t<HOLDER_T,INDEX>;
                constexpr const auto & descriptor = Head_t::descriptor;
                if constexpr(CSameBaseType<DESCRIPTOR_T,typename Head_t::UserDescriptor_t>)
                {
                    fnc.template operator()<descriptor>();
                }
            });

            return 0;
        }
    }
}

#define TP_FOR_EACH_DESCRIPTOR(HOLDER_T,DESCRIPTOR_T,DESCRIPTOR_ALIAS,...) \
    ::Tp::Reflect::forEachDescriptor<HOLDER_T,DESCRIPTOR_T>([&]<auto DESCRIPTOR_ALIAS>()
#define TP_DONE );

#define TP_DESCRIPTOR_MEMBER_POINTER(x)

#define TP_ADD_DESCRIPTOR(MBR_NAME, DESCRIPTOR_NAME, ...)                                                                                                 \
                                                                                                                                                        \
    /* Template class that holds the metadata - This line is possibly redundant - RSVD_DESCRIPTOR_INFO_T might already have been declared */            \
    template <size_t, typename>                                                                                                                         \
    struct RSVD_DESCRIPTOR_INFO_T;                                                                                                                      \
                                                                                                                                                        \
    static constexpr size_t Rsvd_MemberIndex_##MBR_NAME##DESCRIPTOR_NAME =                                                                              \
        Tp::Dt::CountPartialSpecializations<struct Rsvd_Arbitrary_##MBR_NAME##DESCRIPTOR_NAME, RSVD_DESCRIPTOR_INFO_T>::value;                          \
                                                                                                                                                        \
    template <typename HOLDER_T>                                                                                                                        \
    struct RSVD_DESCRIPTOR_INFO_T<Rsvd_MemberIndex_##MBR_NAME##DESCRIPTOR_NAME, HOLDER_T>   \
    {                                                                                       \
        using RSVD_COUNTING_SPECIALIZATIONS = int;                                          \
        static constexpr auto index = Rsvd_MemberIndex_##MBR_NAME##DESCRIPTOR_NAME;         \
        static constexpr std::string_view memberName = #MBR_NAME;                           \
                                                                                            \
        using type = decltype(MBR_NAME);                                                    \
                                                                                            \
        template <typename HOLDER__T>                                                       \
        static constexpr auto memberPointer = &HOLDER__T::MBR_NAME;                         \
                                                                                            \
        using UserDescriptor_t = DESCRIPTOR_NAME;                                           \
                                                                                            \
        struct CommonDescriptor_t                                                           \
        {                                                                                   \
            using Holder_t = HOLDER_T;                                                      \
            static constexpr auto index = Rsvd_MemberIndex_##MBR_NAME##DESCRIPTOR_NAME;     \
            static constexpr std::string_view memberName = #MBR_NAME;                       \
            using type = decltype(MBR_NAME);                                                \
                                                                                            \
            template <typename HOLDER__T>                                                   \
            static constexpr auto memberPointer = &HOLDER__T::MBR_NAME;                     \
        };                                                                                  \
        struct Descriptor_t                                                                 \
        {                                                                                   \
            static constexpr CommonDescriptor_t common = {};                                \
            static constexpr auto user = __VA_ARGS__;                                       \
        };                                                                                  \
        static constexpr Descriptor_t descriptor = {};                                      \
    };



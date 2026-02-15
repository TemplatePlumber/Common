#include <windows.h>
#include <dbghelp.h>
#include <iostream>
#include <tchar.h>
#include <ostream>
#include <iostream>
#include <fstream>

#include "TpUtlControlFlow.h"
#include "TpUtlReflection.h"

// Metadata struct used to mark up class members in reflection.
// PrintInfo would represent the use case of no-code generic object logging.
struct PrintInfo
{
    bool castToInteger = false;
};

// Metadata struct used to mark up class members in reflection.
// SerializeInfo would represent the use case of no-code generic object serialization.
struct SerializeInfo{};

template<typename T>
struct MetaTypeInfo{ using RelatedType = T; const char * relatedTypeName; };

// Example of a class with reflection enabled
struct MyStruct
{
    int myIntMember = 123;
    char myCharMember = 'a';
    std::string myStringMember = "abc";
    bool myBoolMember = true;
     
    TP_ADD_DESCRIPTOR(myIntMember, SerializeInfo{});
    TP_ADD_DESCRIPTOR(myIntMember, PrintInfo{});
    TP_ADD_DESCRIPTOR(myStringMember, SerializeInfo{});
    TP_ADD_DESCRIPTOR(myCharMember, PrintInfo{.castToInteger = true});

    TP_ADD_DESCRIPTOR(myCharMember, MetaTypeInfo<bool>{.relatedTypeName = "bool"});
    TP_ADD_DESCRIPTOR(myBoolMember, MetaTypeInfo<uint8_t>{.relatedTypeName = "uint8_t"});
};

int main()
{
    MyStruct structInstance = {123};

    
    constexpr auto numDescriptors = Tp::Reflect::getDescriptorCount<MyStruct>();
    std::cout << "There are "<< numDescriptors << " descriptors." << std::endl;
    /*
        Output:
            There are 4 descriptors.
    */

    
    /*
        Iterate over each descriptor, printing the name and value of the corresponding data member.
        Note that members that are marked with multiple descriptors will be printed twice.
    */
    Tp::forEachInRange<0,numDescriptors>([&]<auto i>(){
        std::cout << Tp::Reflect::getMemberName<MyStruct,i>() << "=" << Tp::Reflect::getMemberValue<i>(structInstance) << std::endl;
    });

    /*
        Output:
            myIntMember=123
            myIntMember=123
            myStringMember=abc
            myCharMember=a
            myCharMember=a
            myBoolMember=1
    */
    

    //Iterate over every class member marked with 'PrintInfo'.
    TP_FOR_EACH_DESCRIPTOR(MyStruct,PrintInfo,descriptor)
    {
        std::cout << descriptor.common.memberName 
                  << " is printable, castToInteger="
                  << descriptor.user.castToInteger << std::endl;
    }
    TP_DONE
    /*
        Output:
            myIntMember is printable, castToInteger=0
            myCharMember is printable, castToInteger=1
    */    
    
    //Iterate over every class member that is marked with both 'PrintInfo' and 'SerializeInfo'.
    TP_FOR_EACH_DESCRIPTOR(MyStruct,PrintInfo,printDescriptor)
    {
        TP_FOR_EACH_DESCRIPTOR(MyStruct,SerializeInfo,serializeDescriptor)
        {
            if constexpr(printDescriptor.common.memberName == serializeDescriptor.common.memberName)
            {
                std::cout << printDescriptor.common.memberName << " is both printable and serializable" << std::endl;
            }
        }
        TP_DONE
    }
    TP_DONE
    /*
        Output:
            myIntMember is both printable and serializable
    */
    

    TP_FOR_EACH_DESCRIPTOR(MyStruct,MetaTypeInfo,metaTypeDescriptor)
    {
        using RT = decltype(metaTypeDescriptor.user)::RelatedType;
        std::cout << metaTypeDescriptor.common.memberName << " has type member " 
                  << metaTypeDescriptor.user.relatedTypeName << "(" << typeid(RT).name() << ")" << std::endl;
    }
    TP_DONE
    /*
        Output:
            myCharMember has type member bool
            myBoolMember has type member uint8_t
    */
}

#pragma once

namespace Tp
{
    struct Serializable{ const bool ignorePointer = true; };
    
    template<typename T>
    std::string toString(const T & value);
    
    template<typename T> requires (CStringStreamConvertible<T>)
    std::string toString(const T & value)
    {
        std::stringstream ss;
        ss << value;
        std::string ret = ss.str();
        return ret;
    }
    
    template<typename T>
    void toJsonImpl(const T & v1, std::string & ret)
    {
        if constexpr(std::is_pointer_v<T>)
        {
            if(v1 == nullptr)
            {
                ret += "null";
                return;
            }
            
            toJsonImpl(*v1,ret);
        }
        else if constexpr(Tp::Reflect::CHasDescriptorInstance<T,Serializable>)
        {
            ret += "{";
            int count = 0;
            TP_FOR_EACH_DESCRIPTOR(T,Tp::Serializable,descriptor)
            {
                count++;
                ret += descriptor.common.memberName;
                ret += ":";
                const auto & value = Tp::Reflect::getMemberValue<descriptor.common.index>(v1);
                
                if constexpr( descriptor.user.ignorePointer && std::is_pointer_v<decltype(value)> && (requires (T x){x->id;}))
                {
                    // When told to, use the ID instead of traversing the pointer
                    
                }
                else
                {
                    toJsonImpl(value,ret);
                }
                ret += ",";
            }
            TP_DONE
            if(count)
            {
                ret.pop_back();
            }
            ret += "}";
        }
        else if constexpr(Tp::CListContainer<T>)
        {
            ret += "[";
            if(v1.size())
            {
                for(const auto & v2 : v1)
                {
                    toJsonImpl(v2,ret);
                    ret += ",";
                }
                ret.pop_back();
            }
            ret += "]";
        }
        else if constexpr(Tp::CMapContainer<T>)
        {
            ret += "{";
            if(v1.size())
            {
                for(const auto & [key,value] : v1)
                {
                    toJsonImpl(key,ret);
                    ret += ":";
                    toJsonImpl(value,ret);
                    ret += ",";
                }
                ret.pop_back();
            }
            ret += "}";
        }
        else
        {
            ret += toString(v1);
        }
    }
    
    template<typename T>
    std::string toJson(const T & v1)
    {
        std::string ret;
        toJsonImpl(v1,ret);
        return ret;
    }
}
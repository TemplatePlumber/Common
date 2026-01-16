#pragma once

namespace Tp
{
    enum class JsonFormatMode
    {
        DEFAULT,                     // A newline will be printed after every comma and opening brace.
        
        LINE_PER_ELEMENT,            // Each element of an array/dictionary will be written 1 line

        LINE_PER_AGGREGATE,          // All elements of an array or dictionary will be on the same line as the opening brace

        LINE_PER_TRIVIAL_AGGREGATE   // Same as LINE_PER_AGGREGATE, but only if one of the following is true.
                                     //    1. Lists of non-aggregate types
                                     //    2. Dictionaries where both key and value are trivial aggregates
    };
    
    struct SerializeToJson
    {
        const bool ptr2id = true;
        const JsonFormatMode formatMode = JsonFormatMode::DEFAULT; 
    };
    
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
    
    namespace Dt
    {
        struct SerializeState
        {
            const bool format = false;
            const SerializeToJson descriptor = {};
            const size_t tab = 0;
            const bool suspendFormat = false;
        };
    }
    
    
#define TP_DT_MAKE_NEXT_CFG(DESCRIPTOR,SUSPEND_FORMAT) Dt::SerializeState{.format=cfg.format,.descriptor=DESCRIPTOR,.tab=nextTab, .suspendFormat=SUSPEND_FORMAT};
    
    template<Dt::SerializeState cfg,typename T>
    void toJsonImpl(const T & v1, std::string & ret)
    {
        constexpr auto isDictType = Tp::CMapContainer<T> || Tp::Reflect::CHasDescriptorInstance<T,SerializeToJson>;
        constexpr auto isListType = Tp::CListContainer<T>;
        constexpr auto isAggregateType = isDictType || isListType;
        constexpr auto isReflectedType = Tp::Reflect::CHasDescriptorInstance<T,SerializeToJson>;
        
        constexpr auto cfgPtr2Id = cfg.descriptor.ptr2id;
        constexpr auto cfgFmtEn = cfg.format;
        constexpr auto cfgTab = cfg.tab;
        constexpr auto cfgFmtMode = cfg.descriptor.formatMode;
        constexpr auto cfgSuspendFormat = cfg.suspendFormat;
        
        constexpr auto nextSuspendFormat = [&](){
            if constexpr(cfgSuspendFormat)
            {
                return true;
            }
            else if constexpr(cfgFmtMode == JsonFormatMode::LINE_PER_ELEMENT)
            {
                return true;
            }
            return false;
        }();
        
        constexpr auto nextTab = [&]() -> size_t{
            if constexpr(nextSuspendFormat)
            {
                return 0;
            }
            else if constexpr(isAggregateType && cfgFmtEn)
            {
                return cfgTab + 1;
            }
            return cfgTab;
        }();
        

        auto LPutTab = [&](){
            for(auto i=0;i<nextTab;i++){ret += "    ";}
        };
        
        auto LPutComma = [&](){
            ret += ",";
            if constexpr (cfgFmtEn)
            {
                if constexpr(nextSuspendFormat){return;}
                ret += "\n";
                for(auto i=0;i<nextTab;i++){ret += "    ";}
            }
        };

        auto LPutNotComma = [&](){
            if constexpr (cfgFmtEn)
            {
                if constexpr(nextSuspendFormat){return;}
                ret += "\n";
                for(auto i=0;i<cfgTab;i++){ret += "    ";}
            }
        };
        
        auto LPutOpenBrace = [&](const char * character){
            ret += character;
            if constexpr (cfgFmtEn)
            {
                if constexpr(nextSuspendFormat){return;}
                ret += "\n";
                for(auto i=0;i<nextTab;i++){ret += "    ";}
            }
        };
        
        auto LPutCloseBrace = [&](const char * character){
            
            ret += character;
            if constexpr (cfgFmtEn && !cfgSuspendFormat && !nextSuspendFormat)
            {
                ret += "\n";
                for(auto i=0;i<cfgTab;i++){ret += "    ";}
            }
        };
        

        
        

        if constexpr(isDictType)
        {
            auto LPrintDictPair = [&]<auto nextCfg,typename K_T,typename V_T>(const K_T & k,const V_T & v,size_t i, size_t n)
            {
                toJsonImpl<nextCfg>(k,ret);
                ret += ":";
                toJsonImpl<nextCfg>(v,ret);
                if(i < n-1) {LPutComma();} else {LPutNotComma();}
            };
            
            LPutOpenBrace("{");
            if constexpr(isReflectedType)
            {
                constexpr size_t n = Tp::Reflect::countDescriptorInstances<T,Tp::SerializeToJson>();
                TP_FOR_EACH_DESCRIPTOR(T,Tp::SerializeToJson,descriptor)
                {
                    constexpr auto newCfg = TP_DT_MAKE_NEXT_CFG(descriptor.user,nextSuspendFormat);
                    constexpr size_t i = descriptor.common.index;
                    const auto & k = descriptor.common.memberName;
                    const auto & v = Tp::Reflect::getMemberValue<descriptor.common.index>(v1);
                    LPrintDictPair.template operator()<newCfg>(k,v,i,n);
                }
                TP_DONE
            }
            else /* if constexpr(Tp::CMapContainer<T>) */
            {
                const auto n = v1.size();
                constexpr auto newCfg = TP_DT_MAKE_NEXT_CFG(cfg.descriptor,nextSuspendFormat);
                for(const auto & [i,kv] : v1 | enumerate)
                {
                    const auto & [k,v] = kv;
                    LPrintDictPair.template operator()<newCfg>(k,v,i,n);
                }
            }
            LPutCloseBrace("}");
        }
        else if constexpr(isListType)
        {
            LPutOpenBrace("[");
            const size_t n = v1.size();
            for(const auto & [i,v2] : v1 | enumerate)
            {
                constexpr auto newCfg = TP_DT_MAKE_NEXT_CFG(cfg.descriptor,nextSuspendFormat);
                toJsonImpl<newCfg>(v2,ret);
                
                if(i < n-1) {LPutComma();} else {LPutNotComma();}
            }
            LPutCloseBrace("]");
        }
        else if constexpr(std::is_pointer_v<T> && cfgPtr2Id && requires (T x){x->_id;})
        {
            if(v1 == nullptr)
            {
                ret += "-1";
                return;
            }
            
            ret += Tp::toString(v1->_id);
        }
        else if constexpr(std::is_pointer_v<T>)
        {
            if(v1 == nullptr)
            {
                ret += "null";
                return;
            }
            
            toJsonImpl<cfg>(*v1,ret);
        }
        else
        {
            ret += toString(v1);
        }
    }
    
    template<SerializeToJson s2j,typename T>
    std::string toJson(const T & v1)
    {
        std::string ret;
        constexpr auto st = Dt::SerializeState{.format=false,.descriptor=s2j};
        toJsonImpl<st,T>(v1,ret);
        return ret;
    }
    
    template<typename T>
    std::string toJson(const T & v1)
    {
        std::string ret;
        constexpr auto st = Dt::SerializeState{.format=false,.descriptor={}};
        toJsonImpl<st,T>(v1,ret);
        return ret;
    }
}
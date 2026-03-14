#pragma once

#include "TpUtlIdPool.h"

namespace Tp
{
    template<typename ... ARG_Ts>
    struct Topic
    {
        using AgentId_t = size_t;
        using Function_t = std::function<void(const ARG_Ts & ...)>;
        HMap<AgentId_t,std::function<void(const ARG_Ts & ...)>> callbacksByAgentId;
        Tp::IdPool idPool;
        
        Topic(){std::cout << "Creating Topic: " << (void *)this << std::endl;}
        ~Topic(){std::cout << "Deleting Topic: " << (void *)this << std::endl;}

        void publish(const ARG_Ts & ... args)
        {
            for(const auto & [agentId,cb] : callbacksByAgentId)
            {
                cb(args ...);
            }
        }
        
        void subscribe(const AgentId_t agentId, const std::function<void(const ARG_Ts & ...)> & cb)
        {
            idPool.unput(agentId);
            callbacksByAgentId[agentId]=cb;
        }
        
        AgentId_t subscribe(const std::function<void(const ARG_Ts & ...)> & cb)
        {
            auto agentId = idPool.get();
            callbacksByAgentId[agentId]=cb;
            return agentId;
        }
        
        void unsubscribe(const AgentId_t agentId)
        {
            idPool.put(agentId);
            auto itr = callbacksByAgentId.find(agentId);
            if(itr != callbacksByAgentId.end())
            {
                callbacksByAgentId.erase(itr);
            }
        }
    };
    
    template<typename ... ARG_Ts>
    struct Subscription
    {
        u64 agentId = 0;
        Topic<ARG_Ts ...> * topic = nullptr;
        
        Subscription() = delete;
        
        Subscription(Topic<ARG_Ts ...> * topic, std::function<void(const ARG_Ts & ...)> function)
        {
            std::cout << "CREATED SUB WITH " << (void *)topic << std::endl;
            this->topic = topic;
            if(topic)
            {
                agentId = topic->subscribe(function);
            }
        }
        
        Subscription(Subscription &) = delete;
        Subscription(Subscription && other) = delete;

        ~Subscription()
        {
            std::cout << "DELETE SUB WITH " << (void *)topic << std::endl;
            if(topic)
            {
                topic->unsubscribe(agentId);
            }
        }
    };
}
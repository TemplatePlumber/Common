#pragma once

#include <chrono>

namespace Tp
{
    using Microseconds = std::chrono::microseconds;
    using Milliseconds = std::chrono::milliseconds;
    using Seconds = std::chrono::seconds;
    
    template<typename TIME_T = Microseconds>
    auto getTimeStamp()
    {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        auto timeUnits = std::chrono::duration_cast<TIME_T>(duration).count();
        return timeUnits;
    }
    
    
    template<typename TIME_T = Milliseconds>
    class CountdownTimer
    {
        u64 startTimestamp;
        u64 ticksRemaining;
        u64 ticksInitial;
    
    public:
        CountdownTimer(u64 ticks)
        {
            startTimestamp = getTimeStamp<TIME_T>();
            ticksRemaining = ticks;
            ticksInitial = ticks;
        }
        
        
        void reset()
        {
            ticksRemaining = ticksInitial;
        }
        
        
        bool done()
        {
            u64 delta = getTimeStamp<TIME_T>() - startTimestamp;
            if(delta > ticksRemaining)
            {
                ticksRemaining = 0;
                return true;
            }
            ticksRemaining -= delta;
            return false;
        }
    };
    
    
    template<typename TIME_T = Milliseconds>
    class StopWatch
    {
        u64 startTimestamp;
        u64 ticks;
    public:
        
        StopWatch(const bool go=false)
        {
            if(go)
            {
                start();
            }
            else
            {
                startTimestamp = (u64)-1;
                ticks = 0;
            }
        }
        
        void start()
        {
            ticks = 0;
            startTimestamp = getTimeStamp<TIME_T>();
        }
        
        void stop()
        {
            ticks += timeElapsed();
            startTimestamp = (u64)-1;
        }
        
        auto timeElapsed()
        {
            u64 elapsed = 0;
            if(startTimestamp != (u64)-1)
            {
                elapsed += (getTimeStamp<TIME_T>() - startTimestamp);
            }
            elapsed += ticks;
            return elapsed;
        }
    };
}
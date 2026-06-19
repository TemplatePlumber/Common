#pragma once

#include <stddef.h>
#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <assert.h>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <optional>
#include <memory>

namespace Tp
{
    using std::cout;
    using std::endl;
    
    inline constexpr auto & Undefined = std::nullopt;
    
    template<typename ... Ts>
    auto makeTuple(Ts && ... values) { return std::make_tuple(std::forward<Ts>(values) ...); };
    
    template<typename T> requires (std::is_integral_v<T>)
    T pow(T base, T exp)
    {
        if (exp < 0) {
            if (base == 1) { return 1; }
            else if (base == -1)
            {
                if(exp % 2 == 0) 
                { 
                    return 1;
                } 
                else 
                {
                    return -1;
                }
            }
            return 0; // Integer division truncates 1 / (base^exp) to 0
        }
        
        T ret = 1;
        while (exp > 0)
        {
            if (exp & 1) { ret *= base; }
            base *= base;
            exp >>= 1;
        }
        return ret;
    }
}

enum class Cardinal8 {N,NW,W,SW,S,SE,E,NE};
enum class Cardinal4 {N,W,S,E};

using u64 = uint64_t;
using u32 = uint32_t;
using i64 = int64_t;
using i32 = int32_t;
using ch8 = char;
using u8 = uint8_t;
using i8 = int8_t;
using str8 = std::string;

template<typename T>
using SPtr = std::shared_ptr<T>;

//template<typename T>
//auto createShared(){return std::make_shared<T>();}

template<size_t i,typename T> auto get(T && value) { return std::get<i>(std::forward(value)); };
template<typename U, typename V> using Pair = std::pair<U,V>;
template<typename ... Ts> using Tup = std::tuple<Ts ...>;
template<typename T> using Vec = std::vector<T>;
template<typename T> using BSet = std::set<T>;
template<typename T> using HSet = std::unordered_set<T>;
template<typename T,size_t I> using Block = std::array<T,I>;
template<typename U, typename V> using BMap = std::map<U,V>;
template<typename U, typename V> using HMap = std::unordered_map<U,V>;
template<typename T> using Opt = std::optional<T>;

#define CONTINUE_IF(x) if(x) { continue; };
#define INVALID_ID (u64)-1

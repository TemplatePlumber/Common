#pragma once

#include <stddef.h>
#include <string>
#include <type_traits>
#include <iostream>
#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <assert.h>
#include <ranges>
#include <fstream>
#include <cstdint>
#include <sstream>
#include <random>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <stack>
#include <optional>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <queue>

namespace Tp
{
    using std::cout;
    using std::endl;
    
    inline constexpr auto & Pending = std::nullopt;
    
    template<typename ... Ts>
    auto makeTuple(Ts && ... values) { return std::make_tuple(std::forward<Ts>(values) ...); };
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

//#include "TpUtlMetaTemplates.h"
//#include "TpUtlControlFlow.h"
//#include "TpUtlPreprocessor.h"


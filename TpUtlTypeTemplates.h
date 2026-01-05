#pragma once

namespace Tp
{
    /*
        Removes all decoration around the type like const qualifiers, pointers, and references.
    */
    template<typename T>
    using BaseType = typename std::remove_pointer< std::remove_cvref_t<T> >::type;

    template<typename U, typename V>
    concept CSameBaseType = std::is_same_v<BaseType<U>,BaseType<V>>;
}
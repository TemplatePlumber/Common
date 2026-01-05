#pragma once

namespace Tp
{
    template <typename LAMBDA_T>
    class ScopeGuard
    {
        LAMBDA_T _fnc;
        bool _enabled = true;
    public:
        ScopeGuard(LAMBDA_T fnc) : _fnc(fnc) {}
        
        ~ScopeGuard()
        {
            _fnc();
        }
    };
}
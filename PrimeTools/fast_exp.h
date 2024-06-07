#ifndef FAST_EXP_H
#define FAST_EXP_H

#include <iostream>
#include <cstdint>

__int64 fastExp(__int64 b, __int64 e, __int64 m)
{
    __int64 result = e & 1 ? b : 1;
    while (e)
    {
        e >>= 1;
        b = (b * b) % m;
        if (e & 1)
            result = (result * b) % m;
    }
    return result;
}

#endif
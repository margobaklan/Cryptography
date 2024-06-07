#ifndef MILLER_RABIN_H
#define MILLER_RABIN_H

#include "fast_exp.h"

bool trial_composite(__int64 a, __int64 d, __int64 n, __int64 r)
{
    if (fastExp(a, d, n) == 1)
        return false;
    for (__int64 i = 0; i < r; i++)
    {
        if (fastExp(a, (1 << i) * d, n) == n - 1)
            return false;
    }
    return true;
}

bool miller_rabin(__int64 n, __int64 k = 40)
{
    if (n <= 1)
        return false;
    if (n <= 3)
        return true;
    if (n % 2 == 0)
        return false;

    __int64 d = n - 1;
    __int64 r = 0;
    while (d % 2 == 0)
    {
        d /= 2;
        r += 1;
    }

    __int64 a;

    for (__int64 j = 0; j < k; j++)
    {
        a = 2 + rand() % (n - 3);
        if (trial_composite(a, d, n, r))
            return false;
    }
    return true;
}

#endif
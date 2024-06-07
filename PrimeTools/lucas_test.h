#ifndef LUCAS_TEST_H
#define LUCAS_TEST_H

#include <iostream>
#include <random>
#include <algorithm>
#include <cassert>
#include <vector>
#include "fast_exp.h"

using namespace std;

void primeFactors(__int64 n, vector<__int64> &factors)
{
    if (n % 2 == 0)
        factors.push_back(2);
    while (n % 2 == 0)
        n = n / 2;

    for (int i = 3; i <= sqrt(n); i += 2)
    {
        if (n % i == 0)
            factors.push_back(i);
        while (n % i == 0)
            n = n / i;
    }
    if (n > 2)
        factors.push_back(n);
}

bool lucas_test(__int64 n)
{
    if (n == 1)
        return false;
    if (n == 2)
        return true;
    if (n % 2 == 0)
        return false;

    vector<__int64> factors;
    primeFactors(n - 1, factors);

    std::vector<__int64> random;
    for (__int64 i = 2; i < n; i++)
        random.push_back(i);

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(random.begin(), random.end(), g);

    for (int i = 0; i < n - 2; i++)
    {
        __int64 a = random[i];
        if (fastExp(a, n - 1, n) != 1)
            return false;

        bool flag = true;
        for (int k = 0; k < factors.size(); k++)
        {
            if (fastExp(a, (n - 1) / factors[k], n) == 1)
            {
                flag = false;
                break;
            }
        }
        if (flag)
            return true;
    }
    return false;
}

#endif
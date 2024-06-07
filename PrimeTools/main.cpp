#include <iostream>
#include <random>

#include "miller_rabin.h"
#include "lucas_test.h"
#include <algorithm>

using namespace std;

bool baillie_psw(__int64 n)
{
    return miller_rabin(n, 2) && lucas_test(n);
}

vector<int64_t> generatePrimes(int64_t bit_len, int64_t count)
{
    vector<int64_t> primes;
    random_device rd;  
    mt19937 gen(rd()); 
    
    int64_t upper = (1 << bit_len) - 1; 

    uniform_int_distribution<> dis(1, upper); 

    while (primes.size() < count)
    {
        int64_t candidate = dis(gen);
        if (baillie_psw(candidate))
        {
            primes.push_back(candidate);
        }
    }
    return primes;
}

string toBase2(int64_t n)
{
    string binary;
    bool leading_zero = true;

    for (int i = 63; i >= 0; --i)
    {
        if (n & (1LL << i))
        {
            leading_zero = false;
            binary += '1';
        }
        else if (!leading_zero)
            binary += '0';
    }
    if (binary.empty())
        binary = "0";
    return binary;
}

vector<uint8_t> toBytes(int64_t n)
{
    vector<uint8_t> bytes(sizeof(int64_t));
    for (size_t i = 0; i < sizeof(int64_t); ++i)
    {
        bytes[i] = (n >> (8 * i)) & 0xFF;
    }
    return bytes;
}

string toBase64(int64_t num)
{
    const string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    if (num == 0)
        return "A";

    string result;
    while (num > 0)
    {
        result = base64_chars[num % 64] + result;
        num /= 64;
    }

    return result;
}

void coutBytes(vector<int64_t> primes)
{
    for (const auto &n : primes)
    {
        cout << "\n"
             << n << "   ";
        cout << "\nBase2: " << toBase2(n) << "   ";
        cout << "Base10: " << (int64_t)n << "   ";
        cout << "Base64: " << toBase64(n) << "   ";
        vector<uint8_t> bytes = toBytes(n);
        cout << "Bytes: [ ";
        for (uint8_t byte : bytes)
        {
            cout << (int)byte << " ";
        }
        cout << "]" << "   ";
    }
    cout << endl;
}

int main(int argc, const char *argv[])
{
    vector<int64_t> primes = generatePrimes(10, 3);
    cout << "\nPrimes: ";
    coutBytes(primes);

    int64_t f = fastExp(4, 13, 497);
    cout << "4^13 (mod 497) = " << f << endl;

    cout << "Miller Rabin: 1000000007 " << (miller_rabin(1000000007, 10) ? "is prime" : "is not prime") << endl;

    cout << "Baillie PSW: 514229 " << (baillie_psw(514229) ? "is prime" : "is not prime") << endl;

    return 0;
}
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include "message_conversion.h"

using namespace std;

class RSA {
public:
    RSA(int64_t bit_len) {
        auto primes = generatePrimes(bit_len, 2);
        p = primes[0];
        q = primes[1];
        n = p * q;
        lambda_n = lcm(p - 1, q - 1);
        e = findE(lambda_n);
        d = modInverse(e, lambda_n);
        cout << " p=" << p << " q=" << q << " lam=" << lambda_n << " e=" << e << " d=" << d << endl;
    }

    pair<int64_t, int64_t> getPublicKey() {
        return {e, n};
    }

    int64_t encrypt(int64_t msg, const pair<int64_t, int64_t>& publicKey) {
        return fastExp(msg, publicKey.first, publicKey.second);
    }

    int64_t decrypt(int64_t ciphertext) {
        return crt(ciphertext);
    }

    int64_t sign(int64_t msg) {
        return fastExp(msg, d, n);
    }

    bool verify(int64_t msg, int64_t signature, const pair<int64_t, int64_t>& publicKey) {
        return msg == fastExp(signature, publicKey.first, publicKey.second);
    }
    

private:
    int64_t p, q, n, e, d, lambda_n;

    static vector<int64_t> generatePrimes(int64_t bit_len, int64_t count) {
        vector<int64_t> primes;
        random_device rd;
        mt19937 gen(rd());
        int64_t upper = (1LL << bit_len) - 1;
        uniform_int_distribution<> dis(1, upper);

        while (primes.size() < count) {
            int64_t candidate = dis(gen);
            if (baillie_psw(candidate)) {
                primes.push_back(candidate);
            }
        }
        return primes;
    }

    static __int64 fastExp(__int64 b, __int64 e, __int64 m)
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

    static bool trial_composite(__int64 a, __int64 d, __int64 n, __int64 r)
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

    static bool miller_rabin(__int64 n, __int64 k = 40)
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

    static void primeFactors(__int64 n, vector<__int64> &factors)
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

    static bool lucas_test(__int64 n)
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
    
    static bool baillie_psw(__int64 n)
    {
        return miller_rabin(n, 2) && lucas_test(n);
    }

    static int64_t gcd(int64_t a, int64_t b) {
        while (b) {
            a %= b;
            swap(a, b);
        }
        return a;
    }

    static int64_t lcm(int64_t a, int64_t b) {
        return (a / gcd(a, b)) * b;
    }

    static int64_t modInverse(int64_t a, int64_t m) {
        int64_t m0 = m, t, q;
        int64_t x0 = 0, x1 = 1;
        if (m == 1) return 0;
        while (a > 1) {
            q = a / m;
            t = m;
            m = a % m, a = t;
            t = x0;
            x0 = x1 - q * x0;
            x1 = t;
        }
        if (x1 < 0) x1 += m0;
        return x1;
    }

    int64_t crt(int64_t c) {
        int64_t m1 = p, m2 = q;
        int64_t a1 = fastExp(c, d % (p - 1), p);
        int64_t a2 = fastExp(c, d % (q - 1), q);
        int64_t m1_inv = modInverse(m1, m2);
        return (a1 + m1 * ((a2 - a1) * m1_inv % m2 + m2)) % n;
    }

    int64_t findE(int64_t lambda_n) {
        int64_t e = 3;
        while (gcd(e, lambda_n) != 1) {
            e += 2;
        }
        return e;
    }
};

class Client {
public:
    Client(const string& name, int64_t bit_len) : name(name), rsa(bit_len) {
        publicKey = rsa.getPublicKey();
    }

    pair<int64_t, int64_t> getPublicKey() {
        return publicKey;
    }

     void sendMessage(Client& recipient, const string& message) {
        vector<int64_t> numericMessage = getNumMessage(message);
        vector<int64_t> encryptedMessage;
        vector<int64_t> signatures;

        cout << name << " numeric message parts: ";
        for (int64_t part : numericMessage) {
            cout << part << " ";
        }
        cout << endl;

        for (int64_t part : numericMessage) {
            int64_t signature = rsa.sign(part);
            int64_t encryptedPart = rsa.encrypt(part, recipient.getPublicKey());
            signatures.push_back(signature);
            encryptedMessage.push_back(encryptedPart);
        }

        cout << name << " sends encrypted message to " << recipient.name << endl;
        for (int64_t part : encryptedMessage) {
            cout << part << " ";
        }
        cout << endl;
        cout << endl;
        recipient.receiveMessage(encryptedMessage, signatures, publicKey);
    }

    void receiveMessage(const vector<int64_t>& encryptedMessage, const vector<int64_t>& signatures, const pair<int64_t, int64_t>& senderPublicKey) {
        vector<int64_t> decryptedMessage;
        bool allVerified = true;

        for (size_t i = 0; i < encryptedMessage.size(); ++i) {
            int64_t decryptedPart = rsa.decrypt(encryptedMessage[i]);
            bool isVerified = rsa.verify(decryptedPart, signatures[i], senderPublicKey);
            if (!isVerified) allVerified = false;
            decryptedMessage.push_back(decryptedPart);
        }

        string message = getStrMessage(decryptedMessage);
        cout << name << " decrypted message parts: ";
        for (int64_t part : decryptedMessage) {
            cout << part << " ";
        }
        cout << endl;

        cout << name << " received and decrypted message: \"" << message << "\". Signature verification: " << (allVerified ? "successful" : "failed") << endl;
    }

private:
    string name;
    RSA rsa;
    pair<int64_t, int64_t> publicKey;
};

int main() {
    int64_t bit_len = 15; 
    Client alice("Alice", bit_len);
    Client bob("Bob", bit_len);

    string message = "Hello, Bob!";

    alice.sendMessage(bob, message);

    return 0;
}

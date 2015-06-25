#include <iostream>
#include <unordered_map>
#include <algorithm>

/*
为了便于分析，我们将问题限定在以下条件中：
    * 1.假设有n模式串
    * 2.假设模式串的长度均为m
    * 3.每个字符出现概率都是相等的
    * 4.选定s个字节作为最小对比单位 (s=1或2)
则, 每一个节点至少需要256^s个Next指针.

下面我们使用概率论的方法，逐层分析AC自动机的树:
    根节点所在的层我们定义为第0层，以此类推
设, 第n层包含的Next指针数为S(n), 包含的指向子节点的Next指针数为F(n)
    S(0) = 256^s
    F(0) = S(1)*(1 - (1 - 1/S(1))^n)

    S(1) = F(0)*S(0)
    F(1) = S(0)*(1 - (1 - 1/S(0))^n)

    S(2) = F(1)*S(1)
    F(2) = S(1)*(1 - (1 - 1/S(1))^n)

    ...

    以此类推, 得到以下两个方程:
        S(m) = F(m-1) * S(m-1)
        F(m) = S(m)*(1 - (1 - 1/S(m))^n)

    总指针数:
        N(m) = (Sum(k=0->m/s) S(m/s))

    总内存量:                                                                   
        M(m) = N(m) * 8 Bytes. 

*/

long double fast_pow(long double a, int b)
{
    if (a == 0 || a == 1) return a;
    else if (b == 1) return a;
    else if (b == 2) return a * a;
    else if (b == 3) return a * a * a;
    else if (b == 4) {
        long double c = a * a;
        return c * c;
    }
    else {
        typedef std::unordered_map<long double, std::unordered_map<int, long double>> Cache;
        static Cache cache;

        auto it = cache.find(a);
        if (it != cache.end()) {
            auto it2 = it->second.find(b);
            if (it2 != it->second.end()) {
                return it2->second;
            }
        }

        auto result = fast_pow(a, b/2) * fast_pow(a, b - b/2);
        cache[a][b] = result;
        return result;
    }
}

static int n, s;
long long F(int m);
long long S(int m)
{
    if (m == 0) return fast_pow(256, s);

    typedef std::unordered_map<int, long double> Cache;
    static Cache cache;
    auto it = cache.find(m);
    if (it != cache.end()) {
        return it->second;
    }

    auto f = F(m - 1);
    std::cout << "F(" << m - 1 << ")=" << f << std::endl;
    long long result = f * S(0);
    cache[m] = result;
    return result;
}

long long F(int m)
{
    typedef std::unordered_map<int, long double> Cache;
    static Cache cache;
    auto it = cache.find(m);
    if (it != cache.end()) {
        return it->second;
    }

    long double k = S(m);
    std::cout << "S(" << m << ")=" << k << std::endl;
    long long result = (long long)(k * (1 - fast_pow(1 - 1/k, n)));
    cache[m] = result;
    return result;
}

long long N(int m)
{
    typedef std::unordered_map<int, long double> Cache;
    static Cache cache;
    auto it = cache.find(m);
    if (it != cache.end()) {
        return it->second;
    }

    long long result = 0;
    for (int k = 0; k <= m; ++k)
    {
        result += S(k);
    }
    cache[m] = result;
    return result;
}

long long M(int m)
{
    return N(m) * 8;
}

int main()
{
    int m;
    //std::cout << fast_pow(1.000001, 1024*1024*1024) << std::endl;
    std::cin >> n >> m >> s;
    m = m/s + (m % s ? 1 : 0);
    std::cout << "Begin compute..." << std::endl;
    std::cout << "S(" << m << ")=" << S(m) << std::endl;
    std::cout << "N(" << m << ")=" << N(m) << std::endl;
    std::cout << "M(" << m << ")=" << M(m) << " bytes" << std::endl;
    return 0;
}


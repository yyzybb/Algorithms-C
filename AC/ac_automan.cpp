#include <iostream>
#include <fstream>
#include "ac_automan.h"
#include <string.h>
#include "skip_ac.h"
#include <chrono>

//typedef AC_Automan TestAC;
typedef SkipAC TestAC;

void dump(int pos, std::string const& pattern) {
    std::cout << "match [" << pos << "]. keyword: [" << pattern << "]" << "size:" << pattern.size() << std::endl;
}

void test1()
{
    const char* patterns[] = {
        "she",
        "he",
        "say",
        "shr",
        "her"
    };

    std::string source = "yasherhs";
    TestAC ac;
    for (unsigned long i = 0; i < sizeof(patterns) / sizeof(const char*); ++i)
        ac.AddPattern(patterns[i]);
    ac.CreateFailPointer();
    ac.CreateGrah();
    std::cout << "MoveCnt:" << ac.Match(source, dump) << std::endl;

    // replace
    std::string r = ac.Replace(source);
    std::cout << "replace result:" << r << std::endl;
}

void test2()
{
    const char* patterns[] = {
        "的",
        "朋友",
        "好朋友"
    };

    std::string source = "我们的好是朋友";
    TestAC ac;
    for (unsigned long i = 0; i < sizeof(patterns) / sizeof(const char*); ++i)
        ac.AddPattern(patterns[i]);
    ac.CreateFailPointer();
    ac.CreateGrah();
    std::cout << "MoveCnt:" << ac.Match(source, dump) << std::endl;

    // replace
    std::string r = ac.Replace(source);
    std::cout << "replace result:" << r << std::endl;
}

void test3()
{
    const char* patterns[] = {
        "aa"
    };

    std::string source = "aaa";
    TestAC ac;
    for (unsigned long i = 0; i < sizeof(patterns) / sizeof(const char*); ++i)
        ac.AddPattern(patterns[i]);
    ac.CreateFailPointer();
    ac.CreateGrah();
    std::cout << "MoveCnt:" << ac.Match(source, dump) << std::endl;

    // replace
    std::string r = ac.Replace(source);
    std::cout << "replace result:" << r << std::endl;
}

void test4(char *pattern_file, char *source_file)
{
    std::ifstream pif(pattern_file), sif(source_file, std::ios_base::binary);

    TestAC ac;
    std::string line;
    while (pif >> line) {
        if (!ac.AddPattern(line)) {
            std::cout << "AddPattern error, line:" << line << std::endl;
        } else {
            std::cout << "AddPattern success: " << line << std::endl;
        }
    }
    
    std::string source;
    while (std::getline(sif, line)) {
        source += line;
    }
    sif >> source;
    std::cout << "source length:" << source.size() << std::endl;

    ac.CreateFailPointer();
    ac.CreateGrah();
    std::cout << "MoveCnt:" << ac.Match(source, NULL) << std::endl;

    // 预热
    for (int i = 0; i < 1000; i++) {
        ac.Match(source, NULL);
    }

    std::chrono::high_resolution_clock clocks;
    auto ts = clocks.now();
    for (int i = 0; i < 10000; i++) {
        ac.Match(source, NULL);
    }
    auto te = clocks.now();
    auto dur = te - ts;
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
    std::cout << "cost " << ms.count() << " ms" << std::endl;
}

int main(int argc, char** argv)
{
    if (argc >= 3)
        test4(argv[1], argv[2]);
    else {
        test1();
        test2();
        test3();
    }

    return 0;
}


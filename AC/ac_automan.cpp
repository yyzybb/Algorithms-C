#include <iostream>
#include "ac_automan.h"
#include <string.h>

void dump(int pos, std::string const& pattern) {
    std::cout << "match [" << pos << "]. keyword: [" << pattern << "]" << "size:" << pattern.size() << std::endl;
}

std::string replace(std::string const& pattern) {
    int c = utf8_char_c(pattern);
    std::string result;
    result.resize(c);
    memset(&result[0], '*', c);
    //std::cout << "replace c=" << c << ", result=" << result << std::endl;
    return result;
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
    AC_Automan ac;
    for (int i = 0; i < sizeof(patterns) / sizeof(const char*); ++i)
        ac.AddPattern(patterns[i]);
    ac.CreateFailPointer();
    ac.CreateMatrix();
    ac.Match(source, dump);

    // replace
    std::string r = ac.Replace(source, replace);
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
    AC_Automan ac;
    for (int i = 0; i < sizeof(patterns) / sizeof(const char*); ++i)
        ac.AddPattern(patterns[i]);
    ac.CreateFailPointer();
    ac.CreateMatrix();
    ac.Match(source, dump);

    // replace
    std::string r = ac.Replace(source, replace);
    std::cout << "replace result:" << r << std::endl;
}

int main()
{
    test1();
    test2();
    return 0;
}


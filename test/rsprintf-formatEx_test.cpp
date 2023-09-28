#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <string>

char* rsprintf(char* buffer, const char* format, ... )
{
    va_list args;

    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

    return buffer;
}

std::string formatEx(const char* format, ... )
{
    char buffer[100];
    va_list args;

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    return buffer;
}

double a[] = {1.0, 0.0, 2.0, 0.0, 0.0};
double b[] = {-1.0, 3.0, -4.0, 0.0, 0.0};

int main()
{
    char buffer[100];

    std::cout << "a: ";
    for (auto x : a)
    {
        std::cout << " " << rsprintf(buffer, "%+7.3f", x);
    }
    std::cout << '\n';

    std::cout << "b: ";
    for (auto x : b)
    {
        std::cout << " " << formatEx("%+7.3f", x);
    }
    std::cout << '\n';
}

#pragma once
#include <windows.h>
#include <string>

class SymbolOptions
{
public:

    static void setOptions(DWORD flags);
    static DWORD getOptions();

    static void addOptions(DWORD flags);
    static void removeOptions(DWORD flags);

    static std::string toString(DWORD flags);
    static std::string toString();

};












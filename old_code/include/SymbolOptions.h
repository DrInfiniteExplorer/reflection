#pragma once
#include <windows.h>
#include <string>

class SymbolOptions
{
public:
	SymbolOptions();
	SymbolOptions(DWORD flags);

    void setOptions(DWORD flags);
    DWORD getOptions();

    void addOptions(DWORD flags);
    void removeOptions(DWORD flags);

    std::string toString(DWORD flags);
    std::string toString();

private:
	DWORD m_flags;
};












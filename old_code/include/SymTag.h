#pragma once
#include <windows.h>
#include <string>

class SymTag
{
    const ULONG& m_tag;
public:
    SymTag(const ULONG& tag);

    std::string toString() const;
};


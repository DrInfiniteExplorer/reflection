#include "cvconst.h"
#include "Symbol.h"
#include "itostr.h"
#include "SymFlags.h"
#include "SymTag.h"
#include <algorithm>
#include <numeric>
#include <memory.h>
#include "Type.h"
#include "function.h"


Symbol::Symbol()
    : m_symbolInfo()
    , m_symbolSize(0)
    , m_symbolName("<uninitialized>")
{
}

Symbol::Symbol(PSYMBOL_INFO pSymbolInfo, size_t symbolSize) 
    : m_symbolInfo(*pSymbolInfo)
    , m_symbolSize(symbolSize)
    , m_symbolName(pSymbolInfo->Name)
{
}

Symbol::Symbol(ULONG64 imageBase, ULONG symbolIdx)
    : m_symbolInfo()
    , m_symbolSize(0)
    , m_symbolName()
{

    if (!getSymbolName(symbolIdx, imageBase, m_symbolName, &m_symbolInfo))
    {
        throw std::runtime_error("Could not getSymbolName from " __FUNCTION__);
    }
    m_symbolSize = m_symbolInfo.Size;
}


std::string Symbol::getSymbolFileAndLine() const
{
    DWORD dwDisplacement;
    IMAGEHLP_LINE64 line64;
    line64.SizeOfStruct = sizeof(line64);
    if (!SymGetLineFromAddr64(GetCurrentProcess(), m_symbolInfo.Address, &dwDisplacement, &line64))
    {
        return "<no source>:0";
    }
    if (line64.SizeOfStruct != sizeof(line64))
    {
        throw std::runtime_error("line64.SizeOfStruct mismatch in Symbol::getSymbolFileAndLine");
    }
    return std::string(line64.FileName) + ":" + itostr::dec(line64.LineNumber);
}

std::string Symbol::getNameOriginal() const
{
    return m_symbolName;
}

std::string Symbol::toString() const
{
    std::string ret;
    strprintf(ret, "================================\n");
    strprintf(ret, " Symbol\n\n");
    strprintf(ret, "Decorated   name: %s\n", getNameDecorated().c_str());
    strprintf(ret, "Undecorated name: %s\n", getNameUnDecorated().c_str());
    strprintf(ret, "Origin          : %s\n", getSymbolFileAndLine().c_str());
    strprintf(ret, "Size            : %d\n", m_symbolSize);
    strprintf(ret, "Flags           : %s\n", itostr::dec(m_symbolInfo.Flags).c_str());
    strprintf(ret, "as SymFlags     : %s\n", SymFlags(&m_symbolInfo).toString().c_str());
    strprintf(ret, "TypeIndex       : %s\n", itostr::dec(m_symbolInfo.TypeIndex).c_str());
    strprintf(ret, "Size            : %s\n", itostr::dec(m_symbolInfo.Size).c_str());
    strprintf(ret, "ModBase         : 0x%s\n", itostr::hex(m_symbolInfo.ModBase).c_str());
    strprintf(ret, "Value           : %s\n", itostr::dec(m_symbolInfo.Value).c_str());
    strprintf(ret, "Address         : 0x%s\n", itostr::hex(m_symbolInfo.Address).c_str());
    strprintf(ret, "Register        : %s\n", itostr::dec(m_symbolInfo.Register).c_str());
    strprintf(ret, "Scope           : %s\n", itostr::dec(m_symbolInfo.Scope).c_str());
    strprintf(ret, "Tag             : %s (%s)\n", itostr::dec(m_symbolInfo.Tag).c_str(), getTagString().c_str());


    return ret;
}


std::string Symbol::getNameUnDecorated() const
{
    static char buffer[443];
    if (!UnDecorateSymbolName(getNameDecorated().c_str(), buffer, sizeof(buffer), UNDNAME_COMPLETE))
    {
        throw std::runtime_error("UnDecorateSymbolName failed in Symbol::getNameUnDecorated for decorated symbol " + getNameDecorated());
    }
    return std::string(buffer);
}

std::string Symbol::getNameDecorated() const
{
    // afaik we load with no undecoration enabled
    return getNameOriginal();
}

SymFlags Symbol::getFlags() const
{
    return SymFlags(&m_symbolInfo);
}

ULONG Symbol::getTag() const
{
    return m_symbolInfo.Tag;
}

std::string Symbol::getTagString() const
{
    return SymTag(m_symbolInfo.Tag).toString();
}

bool Symbol::getSymbolName(ULONG symbolIdx, ULONG64 imageBase, std::string& symbolName, SYMBOL_INFO* pSymbolInfoOut /*= nullptr*/)
{
    size_t bufferSize = sizeof(SYMBOL_INFO) + sizeof(TCHAR) * MAX_SYM_NAME;
    static SYMBOL_INFO* ptr = nullptr;
    ptr = reinterpret_cast<SYMBOL_INFO*>(malloc(bufferSize));
    struct release{ ~release(){ free(ptr); } } release_dummy;
    
    memset(ptr, 0, bufferSize);

    ptr->SizeOfStruct = sizeof(SYMBOL_INFO);
    ptr->MaxNameLen = MAX_SYM_NAME;
    if (!SymFromIndex(GetCurrentProcess(), imageBase, symbolIdx, ptr))
    {
        throw std::runtime_error("Could not SymFromIndex from " __FUNCTION__);
    }
    symbolName = ptr->Name;
    if (pSymbolInfoOut)
    {
        *pSymbolInfoOut = *ptr;
    }
    return true;
}


size_t Symbol::getSize() const
{
    return m_symbolSize ? m_symbolSize : m_symbolInfo.Size;
}

ULONG64 Symbol::getAddress() const
{
    return m_symbolInfo.Address;
}

Type Symbol::getType() const
{
    return Type(m_symbolInfo.ModBase, m_symbolInfo.TypeIndex);
}

Function Symbol::getFunction() const
{
    if (getTag() != SymTagFunction)
    {
        throw std::runtime_error("Error, not a function!");
    }
    return Function(m_symbolInfo.ModBase, m_symbolInfo.TypeIndex);
}

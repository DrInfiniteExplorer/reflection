#pragma once
#include <windows.h>
#include <string>
#include <memory>
#include "SymFlags.h"
#include <vector>

class Type;
class Function;
class Symbol
{
protected:
    std::string m_symbolName;
    size_t m_symbolSize;
    SYMBOL_INFO m_symbolInfo;

public:
    typedef std::shared_ptr<Symbol> SharedPtr;
    typedef std::vector<SharedPtr> SymbolVector;

    Symbol(PSYMBOL_INFO pSymbolInfo, size_t symbolSize);
    Symbol(ULONG64 imageBase, ULONG symbolIdx);

    std::string getSymbolFileAndLine() const;
    std::string getNameOriginal() const;

    virtual std::string toString() const;
    std::string getNameUnDecorated() const;
    std::string getNameDecorated() const;

    SymFlags getFlags() const;

    ULONG getTag() const;
    std::string getTagString() const;

    static bool getSymbolName(ULONG symbolIdx, ULONG64 imageBase, std::string& m_symbolName, SYMBOL_INFO* m_symbolInfo = nullptr);

    size_t getSize() const;
    ULONG64 getAddress() const;

    Type getType() const;
    std::shared_ptr<Function> getFunction() const;
};

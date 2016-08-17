#pragma once
#include <windows.h>
#include <memory>
#include <vector>
#include "dbghelp/dbghelp.h"


class Symbol;
class Function;

class Type
{
    std::string m_typeName;
    ULONG64 m_imageBase;
    ULONG m_typeIndex;
public:
    template <typename T>
    BOOL GetProperty(IMAGEHLP_SYMBOL_TYPE_INFO info, T& t, ULONG* typeIndexPtr = nullptr) const;

    typedef std::vector<Type> TypeVector;

    Type() : m_typeName("<uinitialized>"), m_imageBase(0), m_typeIndex(0) {};
    Type(ULONG64 imageBase, ULONG typeIndex);

    Type getType() const;
    ULONG getTypeIndex() const { return m_typeIndex; };

    DWORD Type::getBaseType() const;
    std::string getBaseTypeString(DWORD baseType, ULONG64 size) const;
    bool operator==(const Type& other) const;
    bool operator!=(const Type& other) const;

    ULONG64 getSize() const;
    DWORD getCount() const;
    ULONG getTag() const;
    DWORD getCallConvention() const;

    std::vector<ULONG> getChildren() const;
    std::vector<ULONG> getChildrenWithTag(ULONG tagType) const;

    TypeVector getBaseClasses() const;

    std::vector<Function> getMethodMembers() const;
    std::vector<Function> getConstructors(bool ignoreZeroAddress = true) const;
    std::vector<Symbol> getDataMembers() const;
    Type::TypeVector getParameters() const;
    Type getReturnType() const;

    static std::string printTypes(const TypeVector& sv);

    bool isData() const;
    bool isUDT() const;
    bool isEnum() const;
    bool isFunctionType() const;

    bool isPointer() const;
    bool isArray() const;
    bool isBaseType() const;
    bool isBaseClass() const;
    bool isVirtualBaseClass() const;
    std::string getName() const;
    bool hasBaseType(const Type& baseType, bool recurse = true) const;

    std::string toString() const;
};



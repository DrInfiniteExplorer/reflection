#pragma once
#include <windows.h>
#include <string>
#include "dbghelp/dbghelp.h"

class SymFlags
{
    const ULONG& m_flags;
    SymFlags(const ULONG& flags);
public:

    SymFlags(const SYMBOL_INFO * const pInfo);

    bool isNull() const;
    bool isFuncNoReturn() const;
    bool isSyntheticZeroBase() const;
    bool isPublicCode() const;

    bool isClrToken() const;
    bool isConstant() const;
    bool isExport() const;
    bool isForwarder() const;
    bool isFrameRel() const;
    bool isFunction() const;
    bool isIlRel() const;
    bool isLocal() const;
    bool isMetadata() const; //managed
    bool isParameter() const;
    bool isRegister() const;
    bool isRegRel() const;
    bool isSlot() const; // managed code slot
    bool isThunk() const;
    bool isTLSRel() const;
    bool isValuePresent() const;
    bool isVirtualSymbol() const;

    std::string toString() const;

    ULONG value() const;
};



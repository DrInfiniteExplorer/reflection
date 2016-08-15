#pragma once
#include "dbghelp/dbghelp.h"

class SymType
{
    SYM_TYPE& m_symType;
public:
    SymType(SYM_TYPE& symType) : m_symType(symType) {}

    std::string toString() const {
        switch (m_symType)
        {
        case SymNone:     return "SymNone";
        case SymCoff:     return "SymCoff";
        case SymCv:       return "SymCv";
        case SymPdb:      return "SymPdb";
        case SymExport:   return "SymExport";
        case SymDeferred: return "SymDeferred";
        case SymSym:      return "SymSym";
        case SymDia:      return "SymDia";
        case SymVirtual:  return "SymVirtual";
        case NumSymTypes: return "NumSymTypes";
        default:
            throw std::logic_error("Unrecognized enum for SYM_TYPE");
        }
    }
};



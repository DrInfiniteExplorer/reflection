#include "SymFlags.h"




std::string SymFlags::toString() const
{
    auto ret = std::string();
    if (isClrToken())       ret.append("isClrToken ");
    if (isConstant())       ret.append("isConstant ");
    if (isExport())         ret.append("isExport ");
    if (isForwarder())      ret.append("isForwarder ");
    if (isFrameRel())       ret.append("isFrameRel ");
    if (isFunction())       ret.append("isFunction ");
    if (isIlRel())          ret.append("isIlRel ");
    if (isLocal())          ret.append("isLocal ");
    if (isMetadata())       ret.append("isMetadata ");
    if (isParameter())      ret.append("isParameter ");
    if (isRegister())       ret.append("isRegister ");
    if (isRegRel())         ret.append("isRegRel ");
    if (isSlot())           ret.append("isSlot ");
    if (isThunk())          ret.append("isThunk ");
    if (isTLSRel())         ret.append("isTLSRel ");
    if (isValuePresent())   ret.append("isValuePresent ");
    if (isVirtualSymbol())  ret.append("isVirtualSymbol ");
    if (isPublicCode())  ret.append("isPublicCode ");
    if (isSyntheticZeroBase())  ret.append("isSyntheticZeroBase ");
    if (isFuncNoReturn())  ret.append("isFuncNoReturn ");
    if (isNull())  ret.append("isNull ");
    return ret;
}


bool SymFlags::isPublicCode() const
{
    return 0 != (m_flags & SYMFLAG_PUBLIC_CODE);
}

bool SymFlags::isSyntheticZeroBase() const
{
    return 0 != (m_flags & SYMFLAG_SYNTHETIC_ZEROBASE);
}

bool SymFlags::isFuncNoReturn() const
{
    return 0 != (m_flags & SYMFLAG_FUNC_NO_RETURN);
}

bool SymFlags::isNull() const
{
    return 0 != (m_flags & SYMFLAG_NULL);
}

bool SymFlags::isVirtualSymbol() const
{
    return 0 != (m_flags & SYMFLAG_VIRTUAL);
}

bool SymFlags::isValuePresent() const
{
    return 0 != (m_flags & SYMFLAG_REGISTER);
}

bool SymFlags::isTLSRel() const
{
    return 0 != (m_flags & SYMFLAG_TLSREL);
}

bool SymFlags::isThunk() const
{
    return 0 != (m_flags & SYMFLAG_THUNK);
}

bool SymFlags::isSlot() const
{
    return 0 != (m_flags & SYMFLAG_SLOT);
}

bool SymFlags::isRegRel() const
{
    return 0 != (m_flags & SYMFLAG_REGREL);
}

bool SymFlags::isRegister() const
{
    return 0 != (m_flags & SYMFLAG_REGISTER);
}

bool SymFlags::isParameter() const
{
    return 0 != (m_flags & SYMFLAG_PARAMETER);
}

bool SymFlags::isMetadata() const
{
    return 0 != (m_flags & SYMFLAG_METADATA);
}

bool SymFlags::isLocal() const
{
    return 0 != (m_flags & SYMFLAG_LOCAL);
}

bool SymFlags::isIlRel() const
{
    return 0 != (m_flags & SYMFLAG_ILREL);
}

bool SymFlags::isFunction() const
{
    return 0 != (m_flags & SYMFLAG_FUNCTION);
}

bool SymFlags::isFrameRel() const
{
    return 0 != (m_flags & SYMFLAG_FRAMEREL);
}

bool SymFlags::isForwarder() const
{
    return 0 != (m_flags & SYMFLAG_FORWARDER);
}

bool SymFlags::isExport() const
{
    return 0 != (m_flags & SYMFLAG_EXPORT);
}

bool SymFlags::isConstant() const
{
    return 0 != (m_flags & SYMFLAG_CONSTANT);
}

bool SymFlags::isClrToken() const
{
    return 0 != (m_flags & SYMFLAG_CLR_TOKEN);
}

SymFlags::SymFlags(const SYMBOL_INFO * const pInfo) : SymFlags(pInfo->Flags)
{

}

SymFlags::SymFlags(const ULONG& flags) : m_flags(flags)
{

}

ULONG SymFlags::value() const
{
    return m_flags;
}

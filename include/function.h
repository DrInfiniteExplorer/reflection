#pragma once
#include "callingconvention.h"

#include "Symbol.h"
#include "Type.h"
#include "reflection.h"

class Function : public Symbol
{
public:
    using Symbol::Symbol;
    Function(ULONG64 imageBase, ULONG functionTypeIndex)
        : Symbol(imageBase, functionTypeIndex) {}
    typedef std::shared_ptr<Function> SharedPtr;
    typedef std::vector<SharedPtr> FunctionVector;
private:

    template <typename RetType, typename... T>
    struct CallTypes
    {
        typedef RetType (__stdcall    *pfnStdCall)(T...);
        typedef RetType (__cdecl      *pfnCdeclCall)(T...);
        typedef RetType (__fastcall   *pfnFastCall)(T...);
        //typedef RetType (__clrcall    *pfnClrCall)(T...);
        typedef RetType (__vectorcall *pfnVectorCall)(T...);
    };

	

    template <typename FuncPtrType>
    void compareFunctionSignature(const Type& functionTypeInput)
    {
		auto functionTypeLocal = getFunctionType<FuncPtrType>();
		if (functionTypeLocal != functionTypeInput)
		{
			throw std::runtime_error("Invalid function types!");
		}
    }

    template <typename ReturnType, typename funcPtrType, typename... Args>
    ReturnType call(const Type& functionType, ULONG64 addr, Args&&... args)
    {
        compareFunctionSignature<funcPtrType>(functionType);

        funcPtrType fPtr;
        fPtr = reinterpret_cast<funcPtrType>(addr);
        return (*fPtr)(std::forward<Args>(args)...);
    }

    template <typename RetType, typename ClassPtr, typename ...T>
    struct ThisCallType
    {
        typedef typename std::remove_pointer<ClassPtr>::type ClassType;
        typedef RetType(ClassType::*pfnThisCall)(T...);
    };

    template <typename ReturnType, typename ThisType, typename ...Args>
    ReturnType thisCall(const Type& functionType, ULONG64 addr, ThisType thisPtr, Args&&... args)
    {
        typedef ThisCallType<ReturnType, std::remove_reference<ThisType>::type , Args...>::pfnThisCall pfnType;
        compareFunctionSignature<pfnType>(functionType);
        union {
            volatile pfnType fPtr;
            volatile ULONG64 addrConv;
        };
        //fPtr = reinterpret_cast<pfnType>(addr);
		// Extra reading/writing through pointers because ... (aliasing?) through unions dont guarantee writethrough, isch.
        *(&addrConv) = addr;
        return ((*thisPtr).*(*(&fPtr)))(std::forward<Args>(args)...);
    }
    template <typename ReturnType>
    ReturnType thisCall(const Type& functionType, ULONG64 addr)
    {
        throw std::runtime_error("No u no no no");
    }

public:
	template <typename FuncPtrType>
	static Type getFunctionType()
	{
		static FuncPtrType functionPointer = nullptr;
		ULONG64 functionPointerAddr = reinterpret_cast<ULONG64>(&functionPointer);
		auto functionPointerSymbol = reflection::getSymbolFromAddress(functionPointerAddr);
		auto functionPointerType = functionPointerSymbol->getType();
		auto functionTypeLocal = functionPointerType.getType();
		return functionTypeLocal;
	}

	template<typename ReturnType, typename ...T>
    ReturnType callFunction(T&&... t)
    {
        auto functionAddress = getAddress();
        if (functionAddress == 0)
        {
            throw std::runtime_error("Function has address 0; " + getNameOriginal());
        }

        auto functionType = getType();
        auto callingConvention = functionType.getCallConvention();

        switch (callingConvention)
        {
        case CV_CALL_NEAR_C:
        case CV_CALL_FAR_C:
            return call<ReturnType, typename CallTypes<ReturnType, T...>::pfnCdeclCall, T...>(functionType, functionAddress, std::forward<T>(t)...);
        case CV_CALL_NEAR_FAST:
        case CV_CALL_FAR_FAST:
            return call<ReturnType, typename CallTypes<ReturnType, T...>::pfnFastCall, T...>(functionType, functionAddress, std::forward<T>(t)...);
        case CV_CALL_NEAR_STD:
        case CV_CALL_FAR_STD:
            return call<ReturnType, typename CallTypes<ReturnType, T...>::pfnStdCall, T...>(functionType, functionAddress, std::forward<T>(t)...);
        case CV_CALL_THISCALL:
        case CV_CALL_GENERIC:
            return thisCall<ReturnType, T...>(functionType, functionAddress, std::forward<T>(t)...);
            //return compareAndCall<CallTypes<ReturnType, T>::pfnThisCall(), ReturnType>(std::forward(t));
            //break;
        case CV_CALL_MIPSCALL:
        case CV_CALL_ALPHACALL:
        case CV_CALL_PPCCALL:
        case CV_CALL_SHCALL:
        case CV_CALL_ARMCALL:
        case CV_CALL_AM33CALL:
        case CV_CALL_TRICALL:
        case CV_CALL_SH5CALL:
        case CV_CALL_M32RCALL:
        case CV_CALL_NEAR_PASCAL:
        case CV_CALL_FAR_PASCAL:
        case CV_CALL_SKIPPED:
        case CV_CALL_NEAR_SYS:
        case CV_CALL_FAR_SYS:
        default:
            throw std::runtime_error("Can not handle the calling convention");
        }
    }

    std::string toString() const override {
        return getType().toString() + " FUNCTION!!!!! " + __super::toString();
    }
};


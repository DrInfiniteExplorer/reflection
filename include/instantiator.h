#pragma once
#include <windows.h>
#include "SymModule.h"
#include "function.h"

template <typename T>
struct instantiate
{
    typedef T* Tptr;

    template <typename ...Args>
    static Tptr doit(Args&&... args)
    {
        static Tptr obtainTypeInfoFromThis = nullptr;
        auto obtainTypeInfoFromThisPtr = reinterpret_cast<DWORD64>(&obtainTypeInfoFromThis);
        //auto& typeInfo = typeid(T);
        //auto baseTypeName = typeInfo.name();
        //if (baseTypeName == strstr(baseTypeName, "class "))
        //    baseTypeName = baseTypeName + strlen("class ");
        //auto baseTypes = reflection::getTypes(baseTypeName);
        //auto baseType = baseTypes[0];
        auto baseTypePointerSymbol = reflection::getSymbolFromAddress(obtainTypeInfoFromThisPtr);
        auto baseTypePointerType = baseTypePointerSymbol.getType();
        auto baseType = baseTypePointerType.getType();

        auto types = reflection::getTypes("*");
        auto found = std::find_if(types.begin(), types.end(), [&baseType](const Type& type)
        {
            if (!type.isUDT()) return false;
            if (type.hasBaseType(baseType))
            {
                return true;
            }
            return false;
        });

        if (found == types.end())
        {
            throw std::runtime_error(std::string("Could not find thing that instantiates ") + std::string(typeid(T).name()));
        }

        auto typeToInstantiate = *found;

        auto size = static_cast<size_t>(typeToInstantiate.getSize());
        Tptr ptr = reinterpret_cast<T*>(malloc(size));

        auto constructors = typeToInstantiate.getConstructors();
        if (constructors.size() == 0) {
            throw std::runtime_error(strprintf("Could not find any constructors to instantiate type %s which implements %s", typeToInstantiate.getName().c_str(), typeid(T).name()));
        }
        typedef void(*ConstructorSignature)(Args...);
        auto desiredFunctionSignature = Function::getFunctionType<ConstructorSignature>();
        //printf("%s\n", desiredFunctionSignature.toString().c_str());

        auto filter = [&desiredFunctionSignature](const Function& con)
        {
            auto constructorSignature = con.getType();
            //printf("%s\n", constructorSignature.toString().c_str());
            return constructorSignature != desiredFunctionSignature;
        };
        auto end = std::remove_if(constructors.begin(), constructors.end(), filter);
        constructors.resize(std::distance(constructors.begin(), end));

        if (constructors.size() != 1)
        {
            throw std::runtime_error("Got to many or no matching constructors");
        }
        auto constructor = constructors[0];

        constructor.callFunction<void, Tptr, Args...>(static_cast<Tptr&&>(ptr), std::forward<Args>(args)...);

        //typedef void(__stdcall *pfnConstructor)();
        //pfnConstructor fn;
        //ULONG64 addr = constructor->getAddress();
        //if (addr == 0)
        //{
        //    throw std::runtime_error(strprintf("No address for constructor to instantiate type %s which implements %s", typeToInstantiate->getName().c_str(), typeid(T).name()));
        //}
        //fn = reinterpret_cast<pfnConstructor>(addr);
        //auto b = ptr;
        //__asm mov ecx, b;
        //fn();
        return ptr;
    }

    template <typename... argTypes>
    struct specify
    {
        template <typename... calledWithArgs>
        static Tptr call(calledWithArgs&&... args)
        {
            return doit<argTypes...>(static_cast<std::remove_reference<calledWithArgs>::type&&...>(args)...);
        }
    };
};




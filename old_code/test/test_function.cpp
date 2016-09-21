#include <windows.h>
#include <string>
#include <inttypes.h>
#include <memory>
#include "SymModule.h"
#include "SymbolOptions.h"
#include <numeric>
#include <algorithm>
#include "instantiator.h"
#include "reflection.h"


#pragma comment(lib, "dbghelp.lib")

#include "interface.h"

int* functionToTest(char arg1, int arg2, uint16_t arg3, void* arg4, wchar_t& arg5)
{
    printf("functionToTest called with arguments %c %d %d %p %d\n", arg1, arg2, arg3, arg4, arg5);
    return (int*)42;
}

class ISymbol
{
	virtual ~ISymbol() = 0;

	// Returns the location where the symbol can be found.
	virtual intptr_t getSymbolAddress() = 0;

	// Returns the size of the symbol
	virtual std::ptrdiff_t getSymbolSize() = 0;

	virtual std::string getSymbolName() = 0;

	// getFileAndLine(std::string& filename, int& line)
	// Returns true on success
	// filename and line are set to "" and -1, then filled in if found.
	virtual bool getFileAndLine(std::string& filename, int& line) = 0;
};

class ISymbolLoader
{
	virtual ~ISymbolLoader() = 0;

	virtual void setLoaderOptions(const SymbolOptions&) = 0;

	virtual bool addSymbolSource(const std::string& symbolSource) = 0;

	virtual std::shared_ptr<ISymbol> getSymbolFromAddress(intptr_t address) = 0;
	virtual std::shared_ptr<ISymbol> getSymbolFromName(const std::string& symbolName) = 0;
};

class SymbolLoaderDIA : public ISymbolLoader
{
public:
	void setLoaderOptions(const SymbolOptions& loaderOptions) override
	{
		m_loaderOptions = loaderOptions;
	}

	bool addSymbolSource(const std::string& symbolSource) override
	{
		// Load symbol source
	}

	std::shared_ptr<ISymbol> getSymbolFromAddress(intptr_t address) override
	{
	}

	std::shared_ptr<ISymbol> getSymbolFromName(const std::string& symbolName) override
	{
	}

private:
	SymbolOptions m_loaderOptions;
};

int main(int argc, const char *const  argv[])
{
    try {
		SymbolOptions symbolOptions;
        symbolOptions.addOptions(SYMOPT_DEBUG);
        symbolOptions.addOptions(SYMOPT_LOAD_LINES);
        symbolOptions.addOptions(SYMOPT_ALLOW_ZERO_ADDRESS);
        symbolOptions.addOptions(SYMOPT_INCLUDE_32BIT_MODULES);
        symbolOptions.removeOptions(SYMOPT_UNDNAME);
        symbolOptions.removeOptions(SYMOPT_NO_CPP);
        symbolOptions.removeOptions(SYMOPT_DEFERRED_LOADS);

		auto symbolCache = SymbolLoader(symbolOptions);

        BOOL bInvade = FALSE;
		auto status = SymInitialize(GetCurrentProcess(), nullptr, bInvade);
		status = SymInitialize(GetCurrentProcess(), nullptr, bInvade);
		if (status == FALSE)
        {
            throw std::runtime_error("could not SymInitialize");
        }
        class cleanup { public: ~cleanup() { SymCleanup(GetCurrentProcess()); } } _cleanup;

        reflection::init();
        SymRefreshModuleList(GetCurrentProcess());

        auto funcSymbol = reflection::getSymbolFromAddress(reinterpret_cast<ULONG64>(&functionToTest));
        auto&& type = funcSymbol.getType();
        printf("%s\n%s\n"
            , funcSymbol.toString().c_str()
            , type.toString().c_str());

        auto asFunction = funcSymbol.getFunction();
        try {
            asFunction.callFunction<void>();
        }
        catch (const std::runtime_error& e)
        {
            printf("Exception: %s\n", e.what());
        }
    }
    catch (const std::exception& e)
    {
        printf("Exception caught:\n%s\n", e.what());
        return 1;
    }
    return 0;

}

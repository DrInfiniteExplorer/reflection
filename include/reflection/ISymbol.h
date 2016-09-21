#pragma once

class ISymbol
{
public:
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

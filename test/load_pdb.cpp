#include <stdio.h>

#include <gtest/gtest.h>

#include "reflection\SymbolLoaderDIA.h"
#include "reflection\ISymbol.h"


TEST(LoadPDB, LoadPDB)
{
	SymbolLoaderDIA loader;

	auto result = loader.addSymbolSource("ReflectionTestData.pdb");
	//auto result = loader.addSymbolSource("c:/stuff/Kod/reflection/pdbInspector/build-vc/test/Debug/ReflectionTestData.pdb");
	EXPECT_EQ(true, result);
}

TEST(LoadPDB, NoSuchFile)
{
	SymbolLoaderDIA loader;

	ASSERT_THROW(loader.addSymbolSource("qwertyuiop"), std::runtime_error);
}

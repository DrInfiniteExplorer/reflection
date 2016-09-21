#include <cstdio>
#include "interface.h"

class __declspec(dllexport) TestClass : public ITestInterface
{
public:
    int m_member;

    TestClass() : m_member(5) {}
    TestClass(int init) : m_member(init)
	{
		new TestClass();
	}

    void funcA() override
    {
        printf("in " __FUNCTION__ ", m_member=%d\n", m_member);
    }
    void funcB() override
    {
        printf("in " __FUNCTION__ ", m_member=%d\n", m_member);
    }
};


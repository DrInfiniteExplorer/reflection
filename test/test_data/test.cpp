
typedef char CharTypedef;

struct SimpleStruct
{
   char CharMember;
   int IntMember;
   void* VoidPtrMember;
};

void VoidFunctionVoid()
{
}

int IntFunctionInt(int)
{
   return 0;
}

char* CharPointerFunctionVoid()
{
   return nullptr;
}

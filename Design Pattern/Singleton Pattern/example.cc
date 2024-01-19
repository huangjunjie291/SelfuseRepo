#include "singleton.h"

void MyFunction()
{
    Singleton::instance().test();
}

int main()
{
    MyFunction();
    return 0;
}

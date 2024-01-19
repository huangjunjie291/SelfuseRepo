#pragma once
#include <iostream>

class Singleton
{
private:
    Singleton() = default;
    ~Singleton() = default;
public:
    Singleton(const Singleton& s) = delete;
    Singleton(Singleton&& s) = delete;
public:
/*
* instance函数返回一个静态局部变量INSTANCE的引用，确保了只有一个实例被创建。
* 这种方法是线程安全的，因为C++11标准要求静态局部变量的初始化是线程安全的。
*/
    static Singleton& instance()
    {
        static Singleton INSTANCE;
        return INSTANCE;
    }

    void test()
    {
        std::cout<<"call test()"<<std::endl;
    }
};

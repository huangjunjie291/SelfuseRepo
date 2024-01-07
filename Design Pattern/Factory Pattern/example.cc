/*
在这个示例中，我们定义了产品接口 Product，
并创建了两个具体产品类 ConcreteProductA 和 ConcreteProductB。
然后，我们定义了工厂接口 Factory，并创建了两个具体工厂类 ConcreteFactoryA 和 ConcreteFactoryB。
在 main 函数中，我们使用具体工厂来创建具体产品，并调用产品的操作方法。

这个示例展示了工厂方法设计模式的用法，它允许我们在不暴露对象创建逻辑的情况下创建不同类型的产品对象。*/

#include "factory_pattern.h"

int main()
{
    Factory* factoryA = new ConcreteFactoryA();
    Product* productA = factoryA->createProduct();
    productA->operation();

    Factory* factoryB = new ConcreteFactoryB();
    Product* productB = factoryB->createProduct();
    productB->operation();

    delete factoryA;
    delete productA;
    delete factoryB;
    delete productB;

    return 0;
}
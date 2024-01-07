#pragma once

#include <iostream>
#include <string>

//产品接口
class Product
{
public:
    virtual void operation() = 0;
    virtual ~Product(){}
};

//具体产品类A
class ConcreteProductA : public Product
{
public:
    void operation() override
    {
        std::cout<<"Concrete Product A operation\n";
    }
};

//具体产品类B
class ConcreteProductB : public Product
{
public:
    void operation() override
    {
        std::cout<<"Concrete Product B operation\n";
    }
};


//当我们谈到工厂方法设计模式时，我们通常会创建一个接口，然后让子类实现该接口以创建具体的对象。
class Factory
{
public:
    virtual Product* createProduct() = 0;
    virtual ~Factory(){}
};
//具体工厂类A
class ConcreteFactoryA : public Factory
{
public:
    Product* createProduct() override
    {
        return new ConcreteProductA();
    }
};
//具体工厂类B
class ConcreteFactoryB : public Factory
{
public:
    Product* createProduct() override
    {
        return new ConcreteProductB();
    }
};


#include <iostream>
#include <list>
#include <string>

class IObserver
{
public:
    virtual ~IObserver(){};
    virtual void Update(const std::string& message_from_subject) = 0;
};

class ISubject
{
public:
    virtual ~ISubject(){};
    virtual void Attach(IObserver* observer) = 0;
    virtual void Detach(IObserver* observer) = 0;
    virtual void Notify() = 0;
};

class Subject : public ISubject
{
public:
    virtual ~Subject()
    {
        std::cout<< "Goodbye, I was the Subject.\n";
    }

    void Attach(IObserver* observer) override
    {
        list_observer_.push_back(observer);
    }

    void Detach(IObserver* observer) override
    {
        list_observer_.remove(observer);
    }

    void Notify() override
    {
        auto iterator = list_observer_.begin();
        while(iterator != list_observer_.end())
        {
            (*iterator)->Update(message_);
            ++iterator;
        }
    }

    void CreateMessage(std::string message = "Empty")
    {
        this->message_ = message;
        Notify();
    }
private:
    std::list<IObserver*> list_observer_;
    std::string message_;
};

class Observer : public IObserver
{
public:
    Observer(Subject& subject) 
        : subject_(subject)
    {
        this->subject_.Attach(this);
        std::cout<< "Hi, I'm the Observer.\n";
    }

    virtual ~Observer()
    {
        this->subject_.Detach(this);
        std::cout<< "Goodbye, I was the Observer.\n";
    }

    void Update(const std::string& message_form_subject) override
    {
        this->message_from_subject_ = message_form_subject;
        PrintInfo();
    }

    void PrintInfo()
    {
        std::cout<< "A new message is availabe: "<< message_from_subject_ << std::endl;
    }
private:
    std::string message_from_subject_;
    Subject& subject_;
};


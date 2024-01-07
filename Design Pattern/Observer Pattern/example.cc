/*
* 在这个示例中，Subject类充当主题，Observer类充当观察者。
* 当Subject的状态发生变化时，它会通知所有观察者进行更新。
* 在main函数中，我们创建了一个Subject对象和两个Observer对象，
* 并通过CreateMessage方法来模拟Subject状态的变化，
* 从而触发观察者的更新。
*/

#include "observer_pattern.h"

int main()
{
    Subject subject;
    Observer observer1(subject);
    Observer observer2(subject);

    subject.CreateMessage("Hello World!");

    return 0;
}

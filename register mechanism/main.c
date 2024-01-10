/* 
* 在这个示例中，我们main函数中先进行node注册的初始化，剩下的为pipline执行框架，调用注册node的函数
* 用户只需要在自己的.c文件中调用REGISTER_NODE(x)宏定义即可添加新的node，而不需要该任何其他代码
*/
#include "globalfunc.h"
#include "registnode.h"

node_main_t main_node;

void node_registion()
{
    struct node_registration_t* r;
    node_main_t* m = get_main();

    r = m->node_registion;
    while(r)
    {
        r->init();
        r = r->next_node_registion;
    }
}

int main()
{
    node_registion();
    
    // pipeline
    struct node_registration_t* r;
    node_main_t* m = get_main();
    r = m->node_registion;
    while(r)
    {
        r->func();
        r = r->next_node_registion;
    }

    return 0;
}



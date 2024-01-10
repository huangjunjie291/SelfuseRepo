// 仿照VPP的注册机制
#ifndef __LEARN_REGISTER_H__
#define __LEARN_REGISTER_H__

#include <stdlib.h>
#include "globalfunc.h"

typedef void (*init_cb)(void);
typedef void (*func_cb)(void);

typedef struct node_registration_t
{
    unsigned int    nodeidx;
    init_cb         init;
    func_cb         func;
    struct node_registration_t* next_node_registion;
}node_registration_t;


#define REGISTER_NODE(x,...)                                       \
    __VA_ARGS__ node_registration_t x;                             \
static void __add_node_registration_##x (void)                     \
    __attribute__((__constructor__)) ;                             \
static void __add_node_registration_##x (void)                     \
{                                                                  \
    node_main_t* m = get_main();                                   \
    x.next_node_registion = m->node_registion;                     \
    m->node_registion = &x;                                        \
}                                                                  \
__VA_ARGS__ node_registration_t x


#endif /*__LEARN_REGISTER_H__*/
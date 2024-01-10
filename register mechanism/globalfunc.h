#ifndef __GLOBAL_FUNC_H__
#define __GLOBAL_FUNC_H__

typedef struct node_main_t
{
    //其他属性
    struct node_registration_t* node_registion;
}node_main_t;

extern node_main_t main_node;

#define always_inline static inline __attribute__ ((__always_inline__))

always_inline node_main_t* get_main()
{
    return &main_node;
}

#endif /*__GLOBAL_FUNC_H__*/


#include <stdio.h>
#include "registnode.h"


static void user1_init()
{
    printf("user1 init\n");
}

static void user1_func()
{
    printf("user1 process\n");
}

REGISTER_NODE (user1_node) = {
    .nodeidx = 0,
    .init    = user1_init,
    .func    = user1_func,
};
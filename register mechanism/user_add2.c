#include <stdio.h>
#include "registnode.h"


static void user2_init()
{
    printf("user2 init\n");
}

static void user2_func()
{
    printf("user2 process\n");
}

REGISTER_NODE (user2_node) = {
    .nodeidx = 1,
    .init    = user2_init,
    .func    = user2_func,
};
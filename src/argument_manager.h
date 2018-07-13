#ifndef __ARGUMENT_MANAGER__
#define __ARGUMENT_MANAGER__

#include <stdbool.h>


struct argument {
    int ip_addr;
    int port;
    bool daemonized;
};


void argument_manager(int argc, char **argv, struct argument *args);


#endif /* __ARGUMENT_MANAGER__ */

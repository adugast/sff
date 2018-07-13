#ifndef __PROCESS_MANAGER_H__
#define __PROCESS_MANAGER_H__

#include <unistd.h>

#include "list.h"


enum e_status {
    TASK_RUNNING,
    TASK_STOPPED,
    TASK_ASLEEP,
    TASK_UNKNOWN
};


struct pm_process {
    pid_t pid;
    int argc;
    char **argv;
    enum e_status status;
    // add uptime since the process has been launched
    struct list_head node;
};


int pm_add_process(int argc, char **argv);
int pm_launch();
int pm_launch_process(struct pm_process *p);
int pm_get_list_head(struct list_head **l);
void pm_dump_process(struct pm_process *p);


#endif /* __PROCESS_MANAGER_H__ */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/wait.h>

#include "process_manager.h"


LIST_HEAD(process_head); // head of process list


void pm_dump_process(struct pm_process *p)
{
    printf("argc[%d]\n", p->argc);
    int i;
    for (i = 0; i < p->argc; i++)
        printf("argv[%d][%s]\n", i, p->argv[i]);
    printf("pid[%d]\n", p->pid);
    switch (p->status) {
        case TASK_RUNNING: printf("running\n"); break;
        case TASK_STOPPED: printf("stopped\n"); break;
        case TASK_ASLEEP: printf("asleep\n"); break;
        default: printf("unknown\n"); break;
    }
}


int pm_add_process(int argc, char **argv)
{
    struct pm_process *p = calloc(1, sizeof(struct pm_process));
    if (p == NULL)
        return -1;

    p->argc = argc;
    p->argv = calloc(argc + 1, sizeof(char *));

    int i;
    for (i = 0; i < argc; i++)
        p->argv[i] = strdup(argv[i]);
    p->argv[i] = NULL;

    p->status = TASK_UNKNOWN;

    list_add_tail(&(p->node), &process_head);

    return 0;
}


int pm_launch_process(struct pm_process *p)
{
    pid_t ret_child;
    pid_t pid;
    int status;
    switch (pid = fork()) {
        case -1:
            break;
        case 0: //child
            if (execvp(p->argv[0], p->argv) != -1) // execute process in child
            break;
        default: // parent
            p->pid = pid; // set pid of child process to supervise
            p->status = TASK_RUNNING; // set new task status

            // set wait to catch child signals
            ret_child = waitpid(0, &status, WNOHANG);
            // ret_child => of the terminated child
            // then set the status of the process to TASK_STOPPED!
            if (ret_child == -1) {
                printf("waitpid failed\n");
                return -1;
            }

            pm_dump_process(p);

            break;
    }

    return 0;
}


int pm_launch()
{
    struct pm_process *p;
    list_for_each_entry(p, &process_head, node) {
        // do callback here or simple fct ?
        if (p->status != TASK_RUNNING) {
            pm_launch_process(p);
        }
    }

    return 0;
}


int pm_get_list_head(struct list_head **l)
{
    *l = calloc(1, sizeof(struct list_head));
    if (*l == NULL)
        return -1;
    *l = &process_head;
    return 0;
}


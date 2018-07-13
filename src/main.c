#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "argument_manager.h"
#include "process_manager.h"
#include "server_manager.h"
#include "daemon_manager.h"
#include "string_switch.h"


// pointer to supervised process list
struct list_head *l;


const char *server_help =
"Usage: command [optional params]\n"
"Supported command\n"
" ls                    : Print list and status of supervised apps\n"
" add APP_NAME [ARGS]   : Add a new task in the list of supervised apps\n"
" run APP_NAME          : Run the task in the list of supervised apps\n"
" stop APP_NAME         : Stop the task in the list of supervised apps\n"
" ?                     : Show this help text\n"
"Supported optional params:\n"
;


struct cmd {
    int argc;
    char **argv;
};


#include <ctype.h>
static void clean_str(char *str)
{
    int flag = 1;
    char *r_ptr, *w_ptr;

    for (r_ptr = w_ptr = str; *r_ptr != '\0'; r_ptr++) {
        if (isspace(*r_ptr) == 0 || flag == 0)
            isspace(*r_ptr) != 0 ? (*w_ptr++ = ' ') : (*w_ptr++ = *r_ptr);
        flag = isspace(*r_ptr);
    }

    flag ? (*--w_ptr = '\0') : (*r_ptr = '\0');
}


static struct cmd *get_cmd_args(char *buffer)
{
    clean_str(buffer);

    unsigned int count = 0;
    char *ite;
    for (ite = buffer; *ite; ite++)
        (*ite == ' ') ? count++ : 0;
    count++;

    struct cmd *c = calloc(1, sizeof(struct cmd));
    if (c == NULL)
        return NULL;

    c->argc = count;
    c->argv = calloc(count + 1, sizeof(char *));

    unsigned int i = 0;
    char *saveptr;
    char *token = strtok_r(buffer, " ", &saveptr);
    while (token != NULL && i < count) {
        c->argv[i] = strdup(token);
        printf("argv[%d][%s]\n", i, c->argv[i]);
        token = strtok_r(NULL, " ", &saveptr);
        i++;
    }
    c->argv[count] = NULL;

    return c;
}

/* commands to implement
 * st [APP_NAME] -> print status of process
 * stop [APP_NAME]
 * etc ...
 */
void read_cb(int c_sock, char *buffer, int nread)
{
    nread = nread;

    struct cmd *c = get_cmd_args(buffer);
    if (c == NULL)
        return;

    /*
       printf("cmd_argc[%d]\n", c->argc);
       printf("cmd_argc[%s]\n", c->argv[0]);
       */

    // switchs args[0] ?
    switchs(buffer) {
        cases("?") {
            dprintf(c_sock, "%s", server_help);
            break;
        }
        cases("ls") {
            if (list_empty(l)) {
                dprintf(c_sock, "The process list is empty\n");
                break;
            }
            struct pm_process *p;
            list_for_each_entry(p, l, node) {
                int o;
                for (o = 0; o < p->argc; o++)
                    dprintf(c_sock, "%s ", p->argv[o]);
                dprintf(c_sock, "pid[%d]\n", p->pid);
            }
            break;
        }
        cases("add") {
            pm_add_process(c->argc, c->argv);
            dprintf(c_sock, "Process added\n");
            break;
        }
        cases("run") {
            /*
               if (c->argc > 1 && c->argv[1] != NULL) {
               struct pm_process *p;
               list_for_each_entry(p, l, node) {
               if (c->argv[1][0] == p->argv[1][0]) {
               pm_launch_process(p);
               dprintf(c_sock, "Process launched\n");
               }
               }
               }
               */
            break;
        }
        defaults {
            break;
        }
    } switchs_end;

    dprintf(c_sock, ">>");
}


void accept_cb(int c_sock, int errnum)
{
    dprintf(c_sock, "New client %d:%s\n>>", c_sock, strerror(errnum));
}


void close_cb(int c_sock)
{
    dprintf(c_sock, "Stop client %d\n", c_sock);
    close(c_sock);
}


static int launch_server(int ip_addr, int port)
{
        struct server *s = srv_init(ip_addr, port, accept_cb, read_cb, close_cb);
        if (s == NULL)
            return -1;

        // launch the server
        srv_launch(s);

        return 0;
}


void launch_daemon_loop_cb(void *ctx)
{
    struct argument *args = (struct argument *)ctx;
    launch_server(args->ip_addr, args->port);
}


int main(int argc, char *argv[])
{
    struct argument args;

    // get program configuration
    argument_manager(argc, argv, &args);

    // set a pointer to external module
    // points to the head of the supervized process list
    pm_get_list_head(&l);

    if (args.daemonized == true)
        daemonizer(launch_daemon_loop_cb, &args);
    else
        launch_server(args.ip_addr, args.port);

    return 0;
}


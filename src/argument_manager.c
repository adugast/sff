#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "argument_manager.h"


static void print_usage(const char *binary_name)
{
    printf("Usage: %s [params]\n", binary_name);
    printf("Available params:\n");
    printf(" -h, --help     : Display this help and exit\n");
    printf(" -v, --version  : Show version information\n");
    printf(" -i, --ip       : Set server ip address\n");
    printf(" -p, --port     : Set server port\n");
    printf(" -d, --daemon   : Daemonize the server\n");
}


static void print_version()
{
    printf("sff: version \?.\?.\?\?(\?)-release\n");
    printf("Supervisor For Fun\n");
    printf("Copyright (C) 2018 adugast\n");
    printf("MIT License: <https://opensource.org/licenses/MIT>\n");
    printf("Git Repository: <https://github.com/adugast/sff>\n");
    printf("\n");
    printf("This is free software; you are free to change and redistribute it.\n");
    printf("There is NO WARRANTY, to the extent permitted by law.\n");
}


static void set_default_configuration(struct argument *args)
{
    args->ip_addr = 0x7f000001;
    args->port = 9999;
    args->daemonized = false;
}


void argument_manager(int argc, char *argv[], struct argument *args)
{
    set_default_configuration(args);

    static struct option long_options[] = {
        {"help",        no_argument, 0, 'h'},
        {"version",     no_argument, 0, 'v'},
        {"ip",          required_argument, 0, 'i'},
        {"port",        required_argument, 0, 'p'},
        {"daemon",      no_argument, 0, 'd'},
        {0, 0, 0, 0}
    };

    const char *option_string = "hvipd";
    int c;
    while ((c = getopt_long(argc, argv, option_string, long_options, NULL)) != -1) {
        switch (c) {
            case 'h': print_usage(argv[0]); exit(EXIT_SUCCESS);
            case 'v': print_version(); exit(EXIT_SUCCESS);
            case 'i': args->ip_addr = strtol(optarg, NULL, 10); break;
            case 'p': args->port = strtol(optarg, NULL, 10); break;
            case 'd': args->daemonized = true; break;
            default: print_usage(argv[0]); exit(EXIT_SUCCESS);
        }
    }
}


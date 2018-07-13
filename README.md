# Supervisor For Fun [![Language: C](https://img.shields.io/badge/Language-C-brightgreen.svg)](https://en.wikipedia.org/wiki/C_(programming_language)) [![Builder: CMake](https://img.shields.io/badge/Builder-CMake-brightgreen.svg)](https://cmake.org/)  [![License: MIT](https://img.shields.io/badge/License-MIT-brightgreen.svg)](https://opensource.org/licenses/MIT)

Try to do a Supervisor For Fun

## Program description:   

SFF is an attempt to recode a supervisor similar to the Linux-supervisor.
It allows the user to control a number of processes and execute command on them.

Available commands:    
"Usage: command [optional params]\n"                                          
"Supported command\n"                                                        
" ls                    : Print list and status of supervised apps\n"        
" add APP_NAME [ARGS]   : Add a new task in the list of supervised apps\n"   
" run APP_NAME          : Run the task in the list of supervised apps\n"     
" stop APP_NAME         : Stop the task in the list of supervised apps\n"    
" ?                     : Show this help text\n"                             
"Supported optional params:\n"  
"...\n"   

## Usage:

Launch ssf:     
```
$>./sff   
```

By default, the server is launched on ip:127.0.0.1 port:9999.   

Once the program is launched, you can connect to it with netcat:
```  
$>nc 127.0.0.1 9999
``` 

/!\ The program is not clean yet. If the program is launched as a daemon. Kill the process after use.

```
ps -ef | grep sff
kill -9
```

Daemonized option will be set ON by default in the future (At the moment set to OFF for debug)    

## Help:   

```
Usage: ./ssf [params]   
Available params:  
 -h, --help     : Display this help and exit   
 -v, --version  : Show version information     
 -i, --ip       : Set server ip address   
 -p, --port     : Set server port  
 -d, --daemon   : Daemonize the server  
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details     

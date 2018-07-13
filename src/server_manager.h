#ifndef __SERVER_MANAGER_H__
#define __SERVER_MANAGER_H__


/* Signature of user specified accept callback function */
typedef void (*srv_accept_cb_t)(int c_sock, int errnum);
/* Signature of user specified read callback function */
typedef void (*srv_read_cb_t)(int c_socket, char *buffer, int nread);
/* Signature of user specified close callback function
 * The user needs to close the client socket from this callback
 */
typedef void (*srv_close_cb_t)(int c_sock);


struct server {
    int s_socket;   // server socket
    int ip_adress;  // server ip adress
    int port;       // server port
    int poll_max;   // nbr max of managed connection. default:32. use srv_set_poll_max to change it.
    srv_accept_cb_t accept_cb;  // called for each new client accepted
    srv_read_cb_t   read_cb;    // called for each received bytes
    srv_close_cb_t  close_cb;   // called for each socket closed
};


struct server *srv_init(int ip_addr, int port,
        srv_accept_cb_t accept_cb,
        srv_read_cb_t read_cb,
        srv_close_cb_t close_cb);


int srv_launch(struct server *s);


int srv_set_poll_max(struct server *s, unsigned int poll_max);


#endif /* __SERVER_MANAGER_H__ */


#ifndef __DAEMON_MANAGER_H__
#define __DAEMON_MANAGER_H__


/**
 * Signature of the function callback called in the new
 * daemonized instance. A loop is need in this callback
 * to make the daemon run.
 */
typedef void (*dm_loop_cb_t)(void *ctx);


/**
 * Daemonize actions contained in the callback function
 */
int daemonizer(dm_loop_cb_t cb, void *ctx);


#endif /* __DAEMON_MANAGER_H__ */


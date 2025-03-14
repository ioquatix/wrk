#ifndef WRK_H
#define WRK_H

#include "config.h"
#include <pthread.h>
#include <inttypes.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <lua.h>

#include "stats.h"
#include "ae.h"
#include "http_parser.h"

#define RECVBUF  8192

#define MAX_THREAD_RATE_S   10000000
#define SOCKET_TIMEOUT_MS   2000
#define RECORD_INTERVAL_MS  100

extern const char *VERSION;

typedef struct config {
    uint64_t connections;
    uint64_t duration;
    uint64_t threads;
    uint64_t timeout;
    uint64_t pipeline;
    bool     delay;
    bool     dynamic;
    bool     latency;
    bool     verbose;
    char    *host;
    char    *script;
    SSL_CTX *ctx;
} config;

typedef struct {
    pthread_t thread;
    aeEventLoop *loop;
    struct addrinfo *addr;

    // Total number of connections:
    uint64_t connections;

    // Number of stopped connections:
    uint64_t stopped;

    // Number of complete requests:
    uint64_t complete;
    uint64_t requests;
    uint64_t bytes;

    // Timestamps:
    uint64_t start;
    uint64_t stop;

    // How long to wait until killing connections.
    uint64_t timeout;

    lua_State *L;
    errors errors;
    struct connection *cs;
} thread;

typedef struct {
    char  *buffer;
    size_t length;
    char  *cursor;
} buffer;

typedef struct connection {
    thread *thread;
    http_parser parser;
    enum {
        FIELD, VALUE
    } state;
    int fd;
    SSL *ssl;
    bool delayed;
    uint64_t start;
    uint64_t complete;
    bool finished;
    char *request;
    size_t length;
    size_t written;
    uint64_t pending;
    buffer headers;
    buffer body;
    char buf[RECVBUF];
} connection;

#endif /* WRK_H */

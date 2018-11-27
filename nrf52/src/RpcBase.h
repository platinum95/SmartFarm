
#ifndef RPCBASE_H
#define RPCBASE_H

// Base struct representing an RPC type
typedef struct RpCallback {
    char * ( *callback )( char*, int );
    const char * name; 
} RpCallback;

#endif

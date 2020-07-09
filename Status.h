#ifndef STATUS_H
#define STATUS_H

/** all possible candy colors **/
enum Status {
    IDLE,
    WAIT_PAT,
    SERVE,
    MAINTENANCE,
    SHUTDOWN,
    ERR
};

#endif

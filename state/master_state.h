#ifndef MASTER_STATE_H
#define MASTER_STATE_H

#include "base_state.h"
#include "server.h"
#include <boost/shared_ptr.hpp>

namespace agv {
    struct master_state : base_state {
        master_state(local_actor *selfptr) : base_state(selfptr) {
            S_PRINT("master_state");
        }

        ~master_state() {
            g_ptr_worker_actor.reset();
            g_ptr_raw_actor.reset();
            g_ptr_serialport_actor.reset();
        }

        boost::shared_ptr<server>   ptr_manageserver;
        boost::shared_ptr<server>   ptr_screenserver;
    };

    behavior Master(stateful_actor<master_state> *self);
}

#endif // MASTER_STATE_H

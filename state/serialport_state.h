//
// Created by waqi on 1/19/17.
//

#ifndef CAR_SERIALPORT_STATE_H
#define CAR_SERIALPORT_STATE_H


#include "base_state.h"
#include "packet_session.h"
#include <map>


namespace agv {
    struct serialport_state : base_state {
        serialport_state(local_actor *selfptr) : base_state(selfptr) {
            S_PRINT("serialport_state");
        }
    };

    behavior Serialport(stateful_actor<serialport_state> *self);
}


#endif //CAR_SERIALPORT_STATE_H

//
// Created by waqi on 1/12/17.
//

#ifndef CAR_RAW_STATE_H
#define CAR_RAW_STATE_H


#include "base_state.h"
#include "packet_session.h"
#include <map>


namespace agv {
    struct raw_state : base_state {
        raw_state(local_actor *selfptr) : base_state(selfptr) {
            S_PRINT("raw_state");
        }

        boost::weak_ptr<session>    ptrSession_;

        std::map<DataProcesser, boost::shared_ptr<Command>>    commands_;
    };

    behavior Raw(stateful_actor<raw_state> *self);
}


#endif //CAR_RAW_STATE_H

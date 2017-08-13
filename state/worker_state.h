//
// Created by waqi on 1/6/17.
//

#ifndef CAR_WORKER_STATE_H
#define CAR_WORKER_STATE_H


#include "base_state.h"
#include "JobStartCommand.h"
#include "JobDoneCommand.h"


namespace agv {
    struct worker_state : base_state {
        worker_state(local_actor *selfptr) : base_state(selfptr) {
            S_PRINT("worker_state");
        }

        boost::shared_ptr<JobDoneCommand>   ptrCurrentDoneCommand_;
        boost::shared_ptr<JobStartCommand>  ptrCurrentJobCommand_;
        unsigned int                        jobActionIndex_;
        boost::weak_ptr<session>            ptrSession_;
    };

    behavior Worker(stateful_actor<worker_state> *self);
}


#endif //CAR_WORKER_STATE_H

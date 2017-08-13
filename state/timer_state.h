//
// Created by waqi on 12/12/16.
//

#ifndef CAR_TIMER_STATE_H
#define CAR_TIMER_STATE_H


#include <map>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/shared_ptr.hpp>
#include "base_state.h"
#include "timer_object.h"


struct timer_state : base_state {
    timer_state(local_actor *selfptr) : base_state(selfptr) {
        S_PRINT("timer_state");
    }

    std::multimap<boost::posix_time::ptime, boost::shared_ptr<timer_object>>  Objects_;
};

behavior Timer(stateful_actor<timer_state> *self);

void Init_Timer(actor_system& system);
void Exit_Timer();
void Destory_Timer();
void Add_Timer(unsigned int timeout, boost::shared_ptr<timer_object> ptrObject);
int Add_Timer_Sync(unsigned int timeout, boost::shared_ptr<timer_object> ptrObject);
void Update_Timer(unsigned int timeout, boost::shared_ptr<timer_object> ptrObject);
int Update_Timer_Sync(unsigned int timeout, boost::shared_ptr<timer_object> ptrObject);
void Remove_Timer(boost::shared_ptr<timer_object> ptrObject);


#endif //CAR_TIMER_STATE_H

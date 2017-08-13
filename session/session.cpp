//
// Created by waqi on 12/6/16.
//

#include "session.h"
#include "timer_state.h"
#include "session_timer.h"


namespace agv {
    void session::add_timer(unsigned int timeout) {
        LOG_SESSION_INFO("begin");

        boost::shared_ptr<timer_object> ptrObject(new session_timer(shared_from_this()));
        Add_Timer(timeout, ptrObject);

        LOG_SESSION_INFO("end");
    }

    void session::update_timer(unsigned int timeout) {
        LOG_SESSION_INFO("begin");

        boost::shared_ptr<timer_object> ptrObject(new session_timer(shared_from_this()));
        Update_Timer(timeout, ptrObject);

        LOG_SESSION_INFO("end");
    }

    void session::remove_timer() {
        LOG_SESSION_INFO("begin");

        boost::shared_ptr<timer_object> ptrObject(new session_timer(shared_from_this()));
        Remove_Timer(ptrObject);

        LOG_SESSION_INFO("end");
    }
}

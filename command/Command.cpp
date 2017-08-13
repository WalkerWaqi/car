//
// Created by waqi on 12/15/16.
//

#include "Command.h"
#include "common.h"
#include "timer_state.h"
#include "session_timer.h"
#include "DataProcesser.h"


namespace agv {
    unsigned int Command::getCommandChannel() const {
        return ptrProcesser_->getChannel();
    }

    unsigned int Command::getCommand() const {
        return ptrProcesser_->getCommand();
    }

    unsigned int Command::getId() const {
        return ptrProcesser_->getId();
    }

    /*CAF_ALLOW_UNSAFE_MESSAGE_TYPE(boost::posix_time::ptime);
    CAF_ALLOW_UNSAFE_MESSAGE_TYPE(boost::shared_ptr<session>);

    void Command::add_timer(boost::shared_ptr<session> new_session)
    {
        LOG_COMMAND_INFO("begin");

        boost::shared_ptr<timer_object> ptrObject(new session_timer(new_session));
        Add_Timer(CONNECTION_TIMEOUT, ptrObject);

        LOG_COMMAND_INFO("end");
    }

    void Command::update_timer(boost::shared_ptr<session> new_session)
    {
        LOG_COMMAND_INFO("begin");

        boost::shared_ptr<timer_object> ptrObject(new session_timer(new_session));
        Update_Timer(CONNECTION_TIMEOUT, ptrObject);

        LOG_COMMAND_INFO("end");
    }*/
}

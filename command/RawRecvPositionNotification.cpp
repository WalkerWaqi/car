//
// Created by waqi on 2/15/17.
//

#include "RawRecvPositionNotification.h"


namespace agv {
    CAF_ALLOW_UNSAFE_MESSAGE_TYPE(boost::shared_ptr<Command>);

    int RawRecvPositionNotification::Exec(boost::shared_ptr<session> ptrSession) {
        LOG_COMMAND_INFO("begin");

        int ret = -1;

        anon_send(*g_ptr_worker_actor, position_nty_atom::value, shared_from_this());

        ret = 0;

        LOG_COMMAND_INFO("end ret = " << ret);

        return ret;
    }
}
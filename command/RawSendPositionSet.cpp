//
// Created by walker on 17-2-18.
//

#include "RawSendPositionSet.h"


namespace agv {
    CAF_ALLOW_UNSAFE_MESSAGE_TYPE(boost::shared_ptr<Command>);

    int RawSendPositionSet::Done(boost::shared_ptr<Command> ptrChild, boost::shared_ptr<session> ptrSession) {
        LOG_COMMAND_INFO("begin");

        int ret = -1;

        anon_send(*g_ptr_worker_actor, set_position_atom::value, getPtrParentCommand());

        ret = 0;

        LOG_COMMAND_INFO("end ret = " << ret);

        return ret;
    }
}
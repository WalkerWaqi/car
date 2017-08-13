//
// Created by waqi on 1/16/17.
//

#include "RawRecvAckCommand.h"


namespace agv {
    CAF_ALLOW_UNSAFE_MESSAGE_TYPE(boost::shared_ptr<Command>);

    int RawRecvAckCommand::Exec(boost::shared_ptr<session> ptrSession) {
        LOG_COMMAND_INFO("begin");

        int ret = -1;

        boost::shared_ptr<RawProcesser> ptrRaw = boost::dynamic_pointer_cast<RawProcesser>(getPtrProcesser());
        if (((PCOMM_BODY)(((unsigned char*)ptrRaw->getBuffer()->data()) + sizeof(COMM_HEAD)))->error != ERROR_NONE)
        {
            setError(abs(ntohl(((PCOMM_BODY)(((unsigned char*)ptrRaw->getBuffer()->data()) + sizeof(COMM_HEAD)))->error)));
        }

        anon_send(*g_ptr_raw_actor, recv_ack_atom::value, shared_from_this());

        ret = 0;

        LOG_COMMAND_INFO("end ret = " << ret);

        return ret;
    }
}
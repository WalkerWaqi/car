//
// Created by walker on 17-3-1.
//

#include "RawSendStatusGet.h"


namespace agv {
    CAF_ALLOW_UNSAFE_MESSAGE_TYPE(boost::shared_ptr<Command>);

    int RawSendStatusGet::Done(boost::shared_ptr<Command> ptrChild, boost::shared_ptr<session> ptrSession) {
        LOG_COMMAND_INFO("begin");

        int ret = -1;

        if (ptrChild)
        {
            boost::shared_ptr<RawRecvAckCommand> ptrRawAck = boost::dynamic_pointer_cast<RawRecvAckCommand>(ptrChild);
            if (ptrRawAck)
            {
                boost::shared_ptr<RawProcesser> ptrRaw = boost::dynamic_pointer_cast<RawProcesser>(ptrRawAck->getPtrProcesser());
                if (ptrRaw)
                {
                    boost::shared_ptr<StatusGet> ptrStatus = boost::dynamic_pointer_cast<StatusGet>(getPtrParentCommand());
                    if (ptrStatus)
                    {
                        STATUS_ACK& rawStatus = GetRawStatus(ptrRaw->getBuffer());
                        g_status.set_battery(rawStatus.battery);
                        g_status.set_blocking(rawStatus.blocking);
                        g_status.set_empty(rawStatus.empty);

                        ptrStatus->SetStatus(g_status);

                        anon_send(*g_ptr_worker_actor, get_status_atom::value, getPtrParentCommand());

                        ret = 0;
                    }
                }
            }
        }

        LOG_COMMAND_INFO("end ret = " << ret);

        return ret;
    }
}
//
// Created by waqi on 2/17/17.
//

#include "RawSendPositionGet.h"


namespace agv {
    CAF_ALLOW_UNSAFE_MESSAGE_TYPE(boost::shared_ptr<Command>);

    int RawSendPositionGet::Done(boost::shared_ptr<Command> ptrChild, boost::shared_ptr<session> ptrSession) {
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
                    boost::shared_ptr<PositionGet> ptrPosition = boost::dynamic_pointer_cast<PositionGet>(getPtrParentCommand());
                    if (ptrPosition)
                    {
                        POSITION_ACK& rawPosition = GetRawPosition(ptrRaw->getBuffer());

                        g_position.set_station_id(std::string((char*)rawPosition.rfid, sizeof(rawPosition.rfid)).c_str());
                        g_position.set_angle(ntohl(rawPosition.angle));
                        g_position.set_speed(ntohl(rawPosition.speed));

                        ptrPosition->AddPosition(g_position);

                        anon_send(*g_ptr_worker_actor, get_position_atom::value, getPtrParentCommand());

                        ret = 0;
                    }
                }
            }
        }

        LOG_COMMAND_INFO("end ret = " << ret);

        return ret;
    }
}
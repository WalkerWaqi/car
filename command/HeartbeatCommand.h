//
// Created by waqi on 12/16/16.
//

#ifndef CAR_HEARTBEATCOMMAND_H
#define CAR_HEARTBEATCOMMAND_H


#include "Command.h"
#include "packet_session.h"
#include "common.h"


namespace agv {
    class HeartbeatCommand : public Command {
    public:
        HeartbeatCommand(boost::shared_ptr<DataProcesser> ptrProcesser) : Command(ptrProcesser)
        {
        }

        virtual int Exec(boost::shared_ptr<session> ptrSession) override {
            LOG_COMMAND_INFO("begin");

            int ret = -1;

            boost::shared_ptr<packet_session> ptrPacketSession = boost::dynamic_pointer_cast<packet_session>(ptrSession);
            boost::shared_ptr<PacketProcesser> ptrPacket = boost::dynamic_pointer_cast<PacketProcesser>(ptrProcesser_);

            ((Packet&) ptrPacket->getPacket()).set_command(CMD_ACK);
            ((Packet&) ptrPacket->getPacket()).clear_resp();
            ((HeartBeat&)((Packet&) ptrPacket->getPacket()).heartbeat()).set_expire(CONNECTION_TIMEOUT);

            ret = ptrPacketSession->send_packet(*ptrPacket);

            LOG_COMMAND_INFO("end ret = " << ret);

            return ret;
        }
    };
}


#endif //CAR_HEARTBEATCOMMAND_H

//
// Created by walker on 17-3-1.
//

#ifndef CAR_STATUSGETCOMMAND_H
#define CAR_STATUSGETCOMMAND_H


#include "StatusCommand.h"


namespace agv {
    class StatusGet : public StatusCommand {
    public:
        StatusGet(boost::shared_ptr<DataProcesser> ptrProcesser) : StatusCommand(ptrProcesser) {
        }

        virtual int Exec(boost::shared_ptr<session> ptrSession) override;

        virtual int Done(boost::shared_ptr<Command> ptrChild = nullptr, boost::shared_ptr<session> ptrSession = nullptr) override {
            LOG_COMMAND_INFO("begin");

            int ret = -1;

            boost::shared_ptr<packet_session> ptrPacketSession = boost::dynamic_pointer_cast<packet_session>(ptrSession);
            boost::shared_ptr<PacketProcesser> ptrPacket = boost::dynamic_pointer_cast<PacketProcesser>(ptrProcesser_);
            //((Packet &) ptrPacket->getPacket()).set_id(ptrPacket->getId());
            ((Packet &) ptrPacket->getPacket()).set_command(CMD_ACK);
            ((Packet &) ptrPacket->getPacket()).mutable_resp()->set_code((ResponseCode)getError());
            //((Packet &) ptrPacket->getPacket()).set_allocated_positions(GetPositions());
            ((Packet &) ptrPacket->getPacket()).mutable_status()->CopyFrom(GetStatus());

            ret = ptrPacketSession->send_packet(*ptrPacket);

            LOG_COMMAND_INFO("end ret = " << ret);

            return ret;
        }

    private:

    };
}


#endif //CAR_STATUSGETCOMMAND_H

//
// Created by waqi on 2/17/17.
//

#ifndef CAR_POSITIONSET_H
#define CAR_POSITIONSET_H


#include "PositionCommand.h"


namespace agv {
    class PositionSet : public PositionCommand {
    public:
        PositionSet(boost::shared_ptr<DataProcesser> ptrProcesser) : PositionCommand(ptrProcesser) {
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
            ((Packet &) ptrPacket->getPacket()).clear_positions();

            ret = ptrPacketSession->send_packet(*ptrPacket);

            LOG_COMMAND_INFO("end ret = " << ret);

            return ret;
        }

    private:

    };
}


#endif //CAR_POSITIONSET_H

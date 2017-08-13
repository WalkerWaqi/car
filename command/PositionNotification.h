//
// Created by waqi on 2/15/17.
//

#ifndef CAR_POSITIONNOTIFICATION_H
#define CAR_POSITIONNOTIFICATION_H


#include "PositionCommand.h"


namespace agv {
    class PositionNotification : public PositionCommand {
    public:
        PositionNotification(boost::shared_ptr<DataProcesser> ptrProcesser) : PositionCommand(ptrProcesser) {
        }

        virtual int Exec(boost::shared_ptr<session> ptrSession) override {
            LOG_COMMAND_INFO("begin");

            int ret = -1;

            boost::shared_ptr<packet_session> ptrPacketSession = boost::dynamic_pointer_cast<packet_session>(ptrSession);
            boost::shared_ptr<PacketProcesser> ptrPacket = boost::dynamic_pointer_cast<PacketProcesser>(ptrProcesser_);
            ((Packet &) ptrPacket->getPacket()).set_id(ptrPacket->getId());
            ((Packet &) ptrPacket->getPacket()).set_command(POSITION_NTY);
            //((Packet &) ptrPacket->getPacket()).set_allocated_positions(GetPositions());
            ((Packet &) ptrPacket->getPacket()).mutable_positions()->CopyFrom(GetPositions());

            ret = ptrPacketSession->send_packet(*ptrPacket);

            LOG_COMMAND_INFO("end ret = " << ret);

            return ret;
        }

    private:

    };
}


#endif //CAR_POSITIONNOTIFICATION_H

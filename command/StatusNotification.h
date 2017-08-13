//
// Created by walker on 17-2-28.
//

#ifndef CAR_STATUSNOTIFICATION_H
#define CAR_STATUSNOTIFICATION_H


#include "StatusCommand.h"


namespace agv {
    class StatusNotification : public StatusCommand {
    public:
        StatusNotification(boost::shared_ptr<DataProcesser> ptrProcesser) : StatusCommand(ptrProcesser) {
        }

        virtual int Exec(boost::shared_ptr<session> ptrSession) override {
            LOG_COMMAND_INFO("begin");

            int ret = -1;

            boost::shared_ptr<packet_session> ptrPacketSession = boost::dynamic_pointer_cast<packet_session>(ptrSession);
            boost::shared_ptr<PacketProcesser> ptrPacket = boost::dynamic_pointer_cast<PacketProcesser>(ptrProcesser_);
            ((Packet &) ptrPacket->getPacket()).set_id(ptrPacket->getId());
            ((Packet &) ptrPacket->getPacket()).set_command(STATUS_NTY);
            ((Packet &) ptrPacket->getPacket()).mutable_status()->CopyFrom(GetStatus());

            ret = ptrPacketSession->send_packet(*ptrPacket);

            LOG_COMMAND_INFO("end ret = " << ret);

            return ret;
        }

    private:

    };
}


#endif //CAR_STATUSNOTIFICATION_H

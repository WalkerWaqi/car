//
// Created by walker on 17-2-4.
//

#ifndef CAR_TRIGGERCOMMAND_H
#define CAR_TRIGGERCOMMAND_H


#include "Command.h"
#include "packet_session.h"
#include "common.h"
#include "car.h"


namespace agv {
    class TriggerCommand : public Command {
    public:
        TriggerCommand(boost::shared_ptr<DataProcesser> ptrProcesser) : Command(ptrProcesser) {
        }

        virtual int Exec(boost::shared_ptr<session> ptrSession) override {
            LOG_COMMAND_INFO("begin");

            int ret = -1;

            boost::shared_ptr<packet_session> ptrPacketSession = boost::dynamic_pointer_cast<packet_session>(ptrSession);
            boost::shared_ptr<PacketProcesser> ptrPacket = boost::dynamic_pointer_cast<PacketProcesser>(ptrProcesser_);
            ((Packet &) ptrPacket->getPacket()).set_id(ptrPacket->getId());
            ((Packet &) ptrPacket->getPacket()).set_command(TRIGGER);
            ((Packet &) ptrPacket->getPacket()).mutable_trigger();
            ((Trigger &) ptrPacket->getPacket().trigger()).set_type(type_);
            ret = ptrPacketSession->send_packet(*ptrPacket);

            LOG_COMMAND_INFO("end ret = " << ret);

            return ret;
        }

        void setType(TriggerType type) {
            TriggerCommand::type_ = type;
        }

    private:
        TriggerType type_;

    };
}


#endif //CAR_TRIGGERCOMMAND_H

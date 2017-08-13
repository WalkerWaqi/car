//
// Created by walker on 17-2-19.
//

#include "CancelCommand.h"


namespace agv {
    CAF_ALLOW_UNSAFE_MESSAGE_TYPE(boost::shared_ptr<Command>);

    int CancelCommand::Exec(boost::shared_ptr<session> ptrSession) {
        LOG_COMMAND_INFO("begin");

        int ret = -1;

        boost::shared_ptr<packet_session> ptrPacketSession = boost::dynamic_pointer_cast<packet_session>(ptrSession);
        boost::shared_ptr<PacketProcesser> ptrPacket = boost::dynamic_pointer_cast<PacketProcesser>(ptrProcesser_);

        ((Packet&) ptrPacket->getPacket()).set_command(CMD_ACK);
        ((Packet&) ptrPacket->getPacket()).mutable_resp()->set_code((ResponseCode)getError());

        ret = ptrPacketSession->send_packet(*ptrPacket);

        if (ret > 0)
        {
            anon_send(*g_ptr_worker_actor, job_cancel_atom::value);
        }

        LOG_COMMAND_INFO("end ret = " << ret);

        return ret;
    }
}
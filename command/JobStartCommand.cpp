//
// Created by root on 1/19/17.
//

#include "JobStartCommand.h"


namespace agv {
    CAF_ALLOW_UNSAFE_MESSAGE_TYPE(boost::shared_ptr<Command>);

    int JobStartCommand::Exec(boost::shared_ptr<session> ptrSession) {
        LOG_COMMAND_INFO("begin");

        int ret = -1;

        boost::shared_ptr<packet_session> ptrPacketSession = boost::dynamic_pointer_cast<packet_session>(ptrSession);
        boost::shared_ptr<PacketProcesser> ptrPacket = boost::dynamic_pointer_cast<PacketProcesser>(ptrProcesser_);

        PacketProcesser processer(PROTOCOL_VERSION, ptrPacket->getChannel(), ptrPacket->getId());
        ((Packet&)processer.getPacket()).set_id(processer.getId());
        ((Packet&)processer.getPacket()).set_command(CMD_ACK);
        ((Packet&)processer.getPacket()).mutable_job()->set_id(((Job&)((Packet&) ptrPacket->getPacket()).job()).id());
        ret = ptrPacketSession->send_packet(processer);

        if (ret > 0)
        {
            anon_send(*g_ptr_worker_actor, job_start_atom::value, shared_from_this());
        }

        LOG_COMMAND_INFO("end ret = " << ret);

        return ret;
    }

    int JobStartCommand::Done(boost::shared_ptr<Command> ptrChild, boost::shared_ptr<session> ptrSession) {
        LOG_COMMAND_INFO("begin");

        anon_send(*g_ptr_worker_actor, done_action_atom::value, shared_from_this());

        LOG_COMMAND_INFO("end");
    }
}
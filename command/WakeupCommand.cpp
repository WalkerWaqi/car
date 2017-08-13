//
// Created by waqi on 12/15/16.
//

#include "WakeupCommand.h"


namespace agv {
    CAF_ALLOW_UNSAFE_MESSAGE_TYPE(boost::shared_ptr<session>);

    int WakeupCommand::Exec(boost::shared_ptr<session> ptrSession) {
        LOG_COMMAND_INFO("begin");

        int ret = -1;

        boost::shared_ptr<packet_session> ptrPacketSession = boost::dynamic_pointer_cast<packet_session>(ptrSession);
        boost::shared_ptr<PacketProcesser> ptrPacket = boost::dynamic_pointer_cast<PacketProcesser>(ptrProcesser_);

        ((Packet &) ptrPacket->getPacket()).set_command(CMD_ACK);
        ((Packet &) ptrPacket->getPacket()).clear_resp();
        ((Wakeup & )((Packet &) ptrPacket->getPacket()).wakeup()).set_expire(CONNECTION_TIMEOUT);
        ((Wakeup & )((Packet &) ptrPacket->getPacket()).wakeup()).set_first_time(g_first);
        if (g_first) {
            g_first = false;
        }
        ((Wakeup & )((Packet &) ptrPacket->getPacket()).wakeup()).set_mode(g_mode);
        if (g_mode & CAR_MODE_AUTO)
        {
            g_mode = CAR_MODE_AUTO;
        }

        ret = ptrPacketSession->send_packet(*ptrPacket);
        if (ret > 0) {
            ptrPacketSession->setWeakup(true);

            anon_send(*g_ptr_worker_actor, set_session_atom::value, ptrSession);
        }

        LOG_COMMAND_INFO("end ret = " << ret);

        return ret;
    }
}
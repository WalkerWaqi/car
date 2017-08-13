//
// Created by waqi on 2/3/17.
//

#ifndef CAR_JOBDONECOMMAND_H
#define CAR_JOBDONECOMMAND_H


#include "Command.h"
#include "packet_session.h"
#include "common.h"
#include "car.h"


namespace agv {
    class JobDoneCommand : public Command {
    public:
        JobDoneCommand(boost::shared_ptr<DataProcesser> ptrProcesser) : Command(ptrProcesser) {
        }

        virtual int Exec(boost::shared_ptr<session> ptrSession) override {
            LOG_COMMAND_INFO("begin");

            int ret = -1;

            boost::shared_ptr<packet_session> ptrPacketSession = boost::dynamic_pointer_cast<packet_session>(ptrSession);
            boost::shared_ptr<PacketProcesser> ptrPacket = boost::dynamic_pointer_cast<PacketProcesser>(ptrProcesser_);
            ((Packet &) ptrPacket->getPacket()).set_id(ptrPacket->getId());
            ((Packet &) ptrPacket->getPacket()).set_command(JOB_DONE);
            ((Packet &) ptrPacket->getPacket()).mutable_job();
            ((Job &) ptrPacket->getPacket().job()).set_id(jboId_);
            ((Job &) ptrPacket->getPacket().job()).set_action_index(jobActionIndex_);
            ((Job &) ptrPacket->getPacket().job()).mutable_position()->CopyFrom(g_position);
            ((Job &) ptrPacket->getPacket().job()).mutable_status()->CopyFrom(g_status);
            if (getError() != 0)
            {
                LOG_COMMAND_INFO("error = " << getError());
                ((Packet &) ptrPacket->getPacket()).mutable_resp()->set_code((ResponseCode)getError());
            }

            ret = ptrPacketSession->send_packet(*ptrPacket);

            LOG_COMMAND_INFO("end ret = " << ret);

            return ret;
        }

        void setJobInfo(const std::string &jboId_, unsigned int index) {
            JobDoneCommand::jboId_ = jboId_;
            jobActionIndex_ = index;
        }

    private:
        std::string     jboId_;
        unsigned int    jobActionIndex_ = 0;

    };
}


#endif //CAR_JOBDONECOMMAND_H

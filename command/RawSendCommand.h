//
// Created by waqi on 1/16/17.
//

#ifndef CAR_RAWGETSTATUSCOMMAND_H
#define CAR_RAWGETSTATUSCOMMAND_H


#include "Command.h"
#include "packet_session.h"
#include "car.h"


namespace agv {
    class RawSendCommand : public Command {
    public:
        RawSendCommand(boost::shared_ptr<DataProcesser> ptrProcesser) : Command(ptrProcesser)
        {
        }

        virtual int Exec(boost::shared_ptr<session> ptrSession) override {
            LOG_COMMAND_INFO("begin");

            int ret = -1;

            COMM_BODY body = {};
            body.command = htonl(getCommand());
            body.error = getError();

            ret = SendData(ptrSession, (char*)&body, sizeof(body));

            LOG_COMMAND_INFO("end ret = " << ret);

            return ret;
        }

        int SendData(boost::shared_ptr<session>& ptrSession, char* pBuffer, unsigned int nSize, unsigned int timeout = SEND_TIMEOUT) {
            LOG_COMMAND_INFO("begin");

            int ret = -1;

            boost::shared_ptr<packet_session> ptrPacketSession = boost::dynamic_pointer_cast<packet_session>(ptrSession);
            ret = ptrPacketSession->send_command(ptrProcesser_, pBuffer, nSize, timeout);

            LOG_COMMAND_INFO("end ret = " << ret);

            return ret;
        }

    };
}


#endif //CAR_RAWGETSTATUSCOMMAND_H

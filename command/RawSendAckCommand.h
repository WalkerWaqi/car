//
// Created by waqi on 1/17/17.
//

#ifndef CAR_RAWSENDACKCOMMAND_H
#define CAR_RAWSENDACKCOMMAND_H


#include "RawSendCommand.h"
#include "packet_session.h"
#include "car.h"


namespace agv {
    class RawSendAckCommand : public RawSendCommand {
    public:
        RawSendAckCommand(boost::shared_ptr<DataProcesser> ptrProcesser) : RawSendCommand(ptrProcesser)
        {
        }

        virtual int Exec(boost::shared_ptr<session> ptrSession) override {
            LOG_COMMAND_INFO("begin");

            int ret = -1;

            COMM_BODY body = {};
            body.command = COMMAND_ACK;
            body.error = getError();

            ret = SendData(ptrSession, (char*)&body, sizeof(body));

            LOG_COMMAND_INFO("end ret = " << ret);

            return ret;
        }
    };
}


#endif //CAR_RAWSENDACKCOMMAND_H

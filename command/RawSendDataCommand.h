//
// Created by waqi on 1/17/17.
//

#ifndef CAR_RAWSENDALEXMOVECOMMAND_H
#define CAR_RAWSENDALEXMOVECOMMAND_H


#include "RawSendCommand.h"
#include "packet_session.h"
#include "car.h"
#include <string>


namespace agv {
    class RawSendDataCommand : public RawSendCommand {
    public:
        RawSendDataCommand(boost::shared_ptr<DataProcesser> ptrProcesser) : RawSendCommand(ptrProcesser)
        {
        }

        virtual int Exec(boost::shared_ptr<session> ptrSession) override {
            LOG_COMMAND_INFO("begin");

            int ret = -1;

            COMM_BODY body = {};
            body.command = htonl(getCommand());
            body.error = getError();

            std::string strSend((char*)&body, sizeof(COMM_BODY));

            if (!data_.empty())
            {
                strSend.append(data_);
            }

            ret = SendData(ptrSession, (char*)strSend.data(), strSend.size());

            LOG_COMMAND_INFO("end ret = " << ret);

            return ret;
        }

        void setData(const std::string &data) {
            data_ = data;
        }

    protected:
        std::string data_;

    };
}


#endif //CAR_RAWSENDALEXMOVECOMMAND_H

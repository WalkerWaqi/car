//
// Created by waqi on 1/11/17.
//

#ifndef CAR_RAWWAKEUPCOMMAND_H
#define CAR_RAWWAKEUPCOMMAND_H


#include "RawSendDataCommand.h"
#include "packet_session.h"
#include "car.h"


namespace agv {
    class RawRecvWakeupCommand : public RawSendDataCommand {
    public:
        RawRecvWakeupCommand(boost::shared_ptr<DataProcesser> ptrProcesser) : RawSendDataCommand(ptrProcesser)
        {
        }

        virtual int Exec(boost::shared_ptr<session> ptrSession) override;
    };
}


#endif //CAR_RAWWAKEUPCOMMAND_H

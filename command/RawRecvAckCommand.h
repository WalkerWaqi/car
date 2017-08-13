//
// Created by waqi on 1/16/17.
//

#ifndef CAR_RAWACKCOMMAND_H
#define CAR_RAWACKCOMMAND_H


#include "Command.h"
#include "packet_session.h"
#include "car.h"


namespace agv {
    class RawRecvAckCommand : public Command {
    public:
        RawRecvAckCommand(boost::shared_ptr<DataProcesser> ptrProcesser) : Command(ptrProcesser)
        {
        }

        virtual int Exec(boost::shared_ptr<session> ptrSession) override;
    };
}


#endif //CAR_RAWACKCOMMAND_H

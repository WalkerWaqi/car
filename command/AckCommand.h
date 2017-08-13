//
// Created by walker on 17-3-11.
//

#ifndef CAR_ACKCOMMAND_H
#define CAR_ACKCOMMAND_H


#include "Command.h"
#include "packet_session.h"
#include "common.h"
#include "car.h"


namespace agv {
    class AckCommand : public Command {
    public:
        AckCommand(boost::shared_ptr<DataProcesser> ptrProcesser) : Command(ptrProcesser)
        {
        }

        virtual int Exec(boost::shared_ptr<session> ptrSession) override;

    };
}


#endif //CAR_ACKCOMMAND_H

//
// Created by waqi on 12/15/16.
//

#ifndef CAR_WAKEUPCOMMAND_H
#define CAR_WAKEUPCOMMAND_H


#include "Command.h"
#include "packet_session.h"
#include "common.h"
#include "car.h"


namespace agv {
    class WakeupCommand : public Command {
    public:
        WakeupCommand(boost::shared_ptr<DataProcesser> ptrProcesser) : Command(ptrProcesser)
        {
        }

        virtual int Exec(boost::shared_ptr<session> ptrSession) override;
    };
}


#endif //CAR_WAKEUPCOMMAND_H

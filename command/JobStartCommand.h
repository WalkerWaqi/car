//
// Created by root on 1/19/17.
//

#ifndef CAR_JOBSTARTCOMMAND_H
#define CAR_JOBSTARTCOMMAND_H


#include "Command.h"
#include "packet_session.h"
#include "common.h"
#include "car.h"


namespace agv {
    class JobStartCommand : public Command {
    public:
        JobStartCommand(boost::shared_ptr<DataProcesser> ptrProcesser) : Command(ptrProcesser)
        {
        }

        virtual int Exec(boost::shared_ptr<session> ptrSession) override;

        virtual int Done(boost::shared_ptr<Command> ptrChild = nullptr, boost::shared_ptr<session> ptrSession = nullptr) override;
    };
}


#endif //CAR_JOBSTARTCOMMAND_H

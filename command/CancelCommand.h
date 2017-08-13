//
// Created by walker on 17-2-19.
//

#ifndef CAR_CANCELCOMMAND_H
#define CAR_CANCELCOMMAND_H


#include "Command.h"
#include "packet_session.h"
#include "common.h"
#include "car.h"


namespace agv {
    class CancelCommand : public Command {
    public:
        CancelCommand(boost::shared_ptr<DataProcesser> ptrProcesser) : Command(ptrProcesser)
        {
        }

        virtual int Exec(boost::shared_ptr<session> ptrSession) override;

    };
}


#endif //CAR_CANCELCOMMAND_H

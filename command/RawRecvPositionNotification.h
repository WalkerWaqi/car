//
// Created by waqi on 2/15/17.
//

#ifndef CAR_RAWRECVPOSITIONNOTIFICATION_H
#define CAR_RAWRECVPOSITIONNOTIFICATION_H


#include "PositionCommand.h"


namespace agv {
    class RawRecvPositionNotification : public PositionCommand {
    public:
        RawRecvPositionNotification(boost::shared_ptr<DataProcesser> ptrProcesser) : PositionCommand(ptrProcesser)
        {
        }

        virtual int Exec(boost::shared_ptr<session> ptrSession) override;

    private:

    };
}


#endif //CAR_RAWRECVPOSITIONNOTIFICATION_H

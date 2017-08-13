//
// Created by walker on 17-2-28.
//

#ifndef CAR_RAWRECVSTATUSNOTIFICATION_H
#define CAR_RAWRECVSTATUSNOTIFICATION_H


#include "StatusCommand.h"


namespace agv {
    class RawRecvStatusNotification : public StatusCommand {
    public:
        RawRecvStatusNotification(boost::shared_ptr<DataProcesser> ptrProcesser) : StatusCommand(ptrProcesser)
        {
        }

        virtual int Exec(boost::shared_ptr<session> ptrSession) override;

    private:

    };
}


#endif //CAR_RAWRECVSTATUSNOTIFICATION_H

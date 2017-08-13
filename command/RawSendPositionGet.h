//
// Created by waqi on 2/17/17.
//

#ifndef CAR_RAWSENDPOSITIONGET_H
#define CAR_RAWSENDPOSITIONGET_H


#include "RawPositionCommand.h"
#include "RawRecvAckCommand.h"
#include "PositionGet.h"


namespace agv {
    class RawSendPositionGet : public RawPositionCommand {
    public:
        RawSendPositionGet(boost::shared_ptr<DataProcesser> ptrProcesser) : RawPositionCommand(ptrProcesser) {
        }

        virtual int Done(boost::shared_ptr<Command> ptrChild = nullptr, boost::shared_ptr<session> ptrSession = nullptr) override;

    private:

    };
}


#endif //CAR_RAWSENDPOSITIONGET_H

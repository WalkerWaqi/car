//
// Created by walker on 17-2-18.
//

#ifndef CAR_RAWSENDPOSITIONSET_H
#define CAR_RAWSENDPOSITIONSET_H


#include "RawPositionCommand.h"
#include "RawRecvAckCommand.h"
#include "PositionGet.h"


namespace agv {
    class RawSendPositionSet : public RawPositionCommand {
    public:
        RawSendPositionSet(boost::shared_ptr<DataProcesser> ptrProcesser) : RawPositionCommand(ptrProcesser) {
        }

        virtual int Done(boost::shared_ptr<Command> ptrChild = nullptr, boost::shared_ptr<session> ptrSession = nullptr) override;

    private:

    };
}


#endif //CAR_RAWSENDPOSITIONSET_H

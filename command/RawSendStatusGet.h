//
// Created by walker on 17-3-1.
//

#ifndef CAR_RAWSENDSTATUSGET_H
#define CAR_RAWSENDSTATUSGET_H


#include "RawSendDataCommand.h"
#include "RawRecvAckCommand.h"
#include "StatusGet.h"


namespace agv {
    class RawSendStatusGet : public RawSendDataCommand {
    public:
        RawSendStatusGet(boost::shared_ptr<DataProcesser> ptrProcesser) : RawSendDataCommand(ptrProcesser) {
        }

        virtual int Done(boost::shared_ptr<Command> ptrChild = nullptr, boost::shared_ptr<session> ptrSession = nullptr) override;

    private:

        STATUS_ACK& GetRawStatus(boost::shared_ptr<std::vector<char>> ptrBuffer) {
            return *PSTATUS_ACK(((unsigned char*)ptrBuffer->data()) + (sizeof(COMM_HEAD) + sizeof(COMM_BODY)));
        }

    };
}


#endif //CAR_RAWSENDSTATUSGET_H

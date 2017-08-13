//
// Created by walker on 17-2-18.
//

#ifndef CAR_RAWPOSITIONCOMMAND_H
#define CAR_RAWPOSITIONCOMMAND_H


#include "RawSendDataCommand.h"


namespace agv {
    class RawPositionCommand : public RawSendDataCommand {
    public:
        RawPositionCommand(boost::shared_ptr<DataProcesser> ptrProcesser) : RawSendDataCommand(ptrProcesser) {
        }

        POSITION_ACK& GetRawPosition() {
            boost::shared_ptr<RawProcesser> ptrRaw = boost::dynamic_pointer_cast<RawProcesser>(ptrProcesser_);
            return *PPOSITION_ACK(((unsigned char*)ptrRaw->getBuffer()->data()) + (sizeof(COMM_HEAD) + sizeof(COMM_BODY)));
        }

        POSITION_ACK& GetRawPosition(boost::shared_ptr<std::vector<char>> ptrBuffer) {
            return *PPOSITION_ACK(((unsigned char*)ptrBuffer->data()) + (sizeof(COMM_HEAD) + sizeof(COMM_BODY)));
        }

    private:
        Positions   positions_;
    };
}


#endif //CAR_RAWPOSITIONCOMMAND_H

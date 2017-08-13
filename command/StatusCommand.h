//
// Created by walker on 17-2-28.
//

#ifndef CAR_STATUSCOMMAND_H
#define CAR_STATUSCOMMAND_H


#include "Command.h"
#include "packet_session.h"
#include "common.h"
#include "car.h"


namespace agv {
    class StatusCommand : public Command {
    public:
        StatusCommand(boost::shared_ptr<DataProcesser> ptrProcesser) : Command(ptrProcesser) {
        }

        const Status &GetStatus() const {
            return status_;
        }

        void SetStatus(const Status &status) {
            StatusCommand::status_.CopyFrom(status);
        }

        void SetRawStatus(const STATUS_ACK& rawStatus)
        {
            Status status;
            status.set_battery(rawStatus.battery);
            status.set_blocking(rawStatus.blocking);
            status.set_empty(rawStatus.empty);

            SetStatus(status);
        }

        STATUS_ACK& GetRawStatus() {
            boost::shared_ptr<RawProcesser> ptrRaw = boost::dynamic_pointer_cast<RawProcesser>(ptrProcesser_);
            return *PSTATUS_ACK(((unsigned char*)ptrRaw->getBuffer()->data()) + (sizeof(COMM_HEAD) + sizeof(COMM_BODY)));
        }

    private:
        Status   status_;

    };
}


#endif //CAR_STATUSCOMMAND_H

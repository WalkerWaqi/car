//
// Created by waqi on 2/17/17.
//

#ifndef CAR_POSITIONCOMMAND_H
#define CAR_POSITIONCOMMAND_H


#include "Command.h"
#include "packet_session.h"
#include "common.h"
#include "car.h"


namespace agv {
    class PositionCommand : public Command {
    public:
        PositionCommand(boost::shared_ptr<DataProcesser> ptrProcesser) : Command(ptrProcesser) {
        }

        void AddPosition(Position& position) {
            positions_.add_positions()->CopyFrom(position);
        }

        void AddRawPosition(const POSITION_ACK& rawPosition) {
            Position position = {};
            position.set_station_id(std::string((char*)rawPosition.rfid, sizeof(rawPosition.rfid)).c_str());
            position.set_angle(ntohl(rawPosition.angle));
            position.set_speed(ntohl(rawPosition.speed));

            AddPosition(position);
        }

        POSITION_ACK& GetRawPosition() {
            boost::shared_ptr<RawProcesser> ptrRaw = boost::dynamic_pointer_cast<RawProcesser>(ptrProcesser_);
            return *PPOSITION_ACK(((unsigned char*)ptrRaw->getBuffer()->data()) + (sizeof(COMM_HEAD) + sizeof(COMM_BODY)));
        }

        Positions& GetPositions() {
            return positions_;
        }

        Position& GetPacketPosition() {
            boost::shared_ptr<PacketProcesser> ptrPacket = boost::dynamic_pointer_cast<PacketProcesser>(ptrProcesser_);
            if(((Positions&)((Packet&) ptrPacket->getPacket()).positions()).positions_size() > 0)
            {
                return (Position&)(((Positions&)((Packet&) ptrPacket->getPacket()).positions()).positions(0));
            }
        }

        POSITION_ACK GetRawPosition(Position& position) {
            POSITION_ACK rawPosition = {};

            strncpy((char*)rawPosition.rfid, position.station_id().c_str(), sizeof(rawPosition.rfid));
            rawPosition.angle = htonl(position.angle());
            rawPosition.speed = htonl(position.speed());

            return rawPosition;
        }

    private:
        Positions   positions_;
    };
}


#endif //CAR_POSITIONCOMMAND_H

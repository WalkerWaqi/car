//
// Created by waqi on 12/14/16.
//

#ifndef CAR_PACKETPROCESSER_H
#define CAR_PACKETPROCESSER_H


#include "TevaProtocols.pb.h"
#include "DataProcesser.h"


namespace agv {
    class PacketProcesser : public DataProcesser {
    public:
        PacketProcesser(unsigned char version = PROTOCOL_VERSION, unsigned char channel = 0, unsigned short id = 0, unsigned int command = 0) :
                DataProcesser(version, channel, id, command) {
        }

        PacketProcesser(Packet &packet, unsigned char version = PROTOCOL_VERSION, unsigned char channel = 0, unsigned short id = 0) :
                packet_(packet), DataProcesser(version, channel, id)
        {
            command_ = packet_.command();
        }

        PacketProcesser(const char* buffer, int size, unsigned char version = PROTOCOL_VERSION, unsigned char channel = 0, unsigned short id = 0) :
                DataProcesser(version, channel, id)
        {
            packet_.ParseFromArray(buffer, size);
            command_ = packet_.command();
        }

        virtual ~PacketProcesser() {
        }

        const Packet &getPacket() const {
            return packet_;
        }

        void setPacket(const Packet &packet_) {
            PacketProcesser::packet_ = packet_;
        }

    protected:
        Packet          packet_;
    };
}


#endif //CAR_PACKETPROCESSER_H

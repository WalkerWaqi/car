//
// Created by waqi on 1/10/17.
//

#ifndef CAR_RAWPROCESSER_H
#define CAR_RAWPROCESSER_H


#include <vector>
#include <boost/asio.hpp>
#include "DataProcesser.h"


namespace agv {
    class RawProcesser : public DataProcesser {
    public:
        RawProcesser(unsigned char version = PROTOCOL_VERSION, unsigned char channel = 0, unsigned short id = 0, unsigned int command = 0) :
                DataProcesser(version, channel, id, command) {
        }

        RawProcesser(boost::shared_ptr<std::vector<char> > &buffer) : buffer_(buffer) {
            PCOMM_HEAD pHead = (PCOMM_HEAD)buffer_->data();
            version_ = pHead->version;
            channel_ = pHead->channel;
            id_ = ntohs(pHead->id);

            PCOMM_BODY pBody = (PCOMM_BODY)pHead->data;
            command_ = ntohl(pBody->command);

            LOG_COMMAND_INFO("RawProcesser: version = " << version_ << " channel = " << channel_ << " id = " << id_ << " command = " << command_);
        }

        virtual ~RawProcesser() {
        }

        const boost::shared_ptr<std::vector<char>> &getBuffer() const {
            return buffer_;
        }

        void setBuffer(const boost::shared_ptr<std::vector<char>> &buffer_) {
            RawProcesser::buffer_ = buffer_;
        }

    protected:
        boost::shared_ptr<std::vector<char> > buffer_;

    };
}


#endif //CAR_RAWPROCESSER_H

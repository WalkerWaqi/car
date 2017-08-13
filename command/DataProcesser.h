//
// Created by waqi on 1/10/17.
//

#ifndef CAR_DATAPROCESSER_H
#define CAR_DATAPROCESSER_H


#include "Command.h"
#include "CommandCreater.h"
#include <boost/enable_shared_from_this.hpp>
#include "commhead.h"


namespace agv {
    class DataProcesser : public boost::enable_shared_from_this<DataProcesser> {
    public:
        DataProcesser(unsigned char version = PROTOCOL_VERSION, unsigned char channel = 0, unsigned short id = 0, unsigned int command = 0) :
                version_(version), channel_(channel), id_(id), command_(command) {
        }

        virtual ~DataProcesser() {
        }

        boost::shared_ptr<Command> CreateCommand() {
            return CommandCreater::CreateCommand(shared_from_this());
        }

        bool operator<(const DataProcesser &d)const
        {
            if (id_ < d.id_ || ((id_ == d.id_) && (channel_ < d.channel_)))
            {
                return true;
            }

            return false;
        }

        unsigned int getVersion() const {
            return version_;
        }

        void setVersion(unsigned int version) {
            version_ = version;
        }

        unsigned int getChannel() const {
            return channel_;
        }

        void setChannel(unsigned int channel) {
            channel_ = channel;
        }

        unsigned int getId() const {
            return id_;
        }

        void setId(unsigned int id) {
            id_ = id;
        }

        unsigned int getCommand() const {
            return command_;
        }

        void setCommand(unsigned int command_) {
            DataProcesser::command_ = command_;
        }

    protected:
        unsigned int    version_;
        unsigned int    channel_;
        unsigned int    id_;

        unsigned int    command_;

    };
}


#endif //CAR_DATAPROCESSER_H

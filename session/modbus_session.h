//
// Created by waqi on 12/8/16.
//

#ifndef CAR_SCREEN_SESSION_H
#define CAR_SCREEN_SESSION_H

#include "session.h"


namespace agv {
    class modbus_session : public session {
    public:
        using session::session;

        virtual ~modbus_session() {
            LOG_SESSION_INFO("begin");
            LOG_SESSION_INFO("end");
        }

        virtual int check_packet(boost::asio::streambuf &buffer) {
            LOG_SESSION_INFO("begin");

            int ret = -1;

            ret = 0;

            LOG_SESSION_INFO("end ret = " << ret);

            return ret;
        }

        virtual void on_receive(boost::shared_ptr<std::vector<char> > buffers, size_t bytes_transferred) override {
            LOG_SESSION_INFO("begin bytes_transferred = " << bytes_transferred);

            LOG_SESSION_INFO("end");
        }
    };
}


#endif //CAR_SCREEN_SESSION_H

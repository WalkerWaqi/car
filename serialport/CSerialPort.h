//
// Created by waqi on 1/18/17.
//

#ifndef CAR_CSERIALPORT_H
#define CAR_CSERIALPORT_H


#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "io_service_pool.hpp"
#include "serialport.h"
#include "logger.h"
#include "modbus.h"
#include "car.h"


namespace agv {
    class CSerialPort : public boost::enable_shared_from_this<CSerialPort> {
    public:
        CSerialPort(std::string &serialPort) : io_service_pool_(1), serial_port_(serialPort) {
            ptrSerialPort_.reset(new SerialPort(io_service_pool_.get_io_service(), 9600, serial_port_));

            ptrSerialPort_->DataRead.connect(boost::bind(&CSerialPort::readHandler, this, _1, _2));
        }

        void run() {
            LOG_SERIALPORT_INFO("begin");

            io_thread_.reset(new boost::thread(boost::bind(&io_service_pool::run, &io_service_pool_)));

            ptrSerialPort_->Initialize();

            LOG_SERIALPORT_INFO("end");
        }

        void stop() {
            LOG_SERIALPORT_INFO("begin");

            ptrSerialPort_->Close();

            io_service_pool_.stop();

            io_thread_->join();

            LOG_SERIALPORT_INFO("end");
        }

        void readHandler(
                boost::array<char, SerialPort::k_readBufferSize> const &buffer,
                size_t bytesTransferred);

        const boost::shared_ptr<SerialPort> &GetPtrSerialPort() const {
            return ptrSerialPort_;
        }

    private:
        boost::shared_ptr<boost::thread> io_thread_;
        io_service_pool io_service_pool_;
        std::string serial_port_;

        boost::shared_ptr<SerialPort> ptrSerialPort_;

        bool running_ = false;
        bool auto_ = false;

        std::string     strRecv_;
        unsigned int    next_ = 0;
    };
}


#endif //CAR_CSERIALPORT_H

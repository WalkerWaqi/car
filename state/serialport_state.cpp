//
// Created by waqi on 1/19/17.
//

#include "serialport_state.h"
#include "CSerialPort.h"
#include "car.h"


CAF_ALLOW_UNSAFE_MESSAGE_TYPE(boost::shared_ptr<agv::CSerialPort>);

behavior agv::Serialport(stateful_actor<serialport_state> *self)
{
    using std::chrono::milliseconds;

    if (!self->state.init("serialport")) {
        A_PRINT("break");
        return {};
    }

    A_PRINT("begin");

    A_PRINT("end");

    return {
            [=](serialport_atom, boost::shared_ptr<CSerialPort> ptrCSerialPort, unsigned int type) {
                //A_PRINT("received {'serialport_atom'}");
                //LOG_SERIALPORT_INFO("type  = " << type);

                if (ptrCSerialPort)
                {
                    if (type == 0)
                    {
                        unsigned char tmp[] = {0x01,0x01,0x00,0x06,0x00,0x08,0xDD,0xCD};
                        std::string msg((char*)tmp, sizeof(tmp));
                        ptrCSerialPort->GetPtrSerialPort()->Write(msg);
                    }
                    else
                    {
                        unsigned char tmp[] = {0x01,0x01,0x00,0x0E,0x00,0x08,0x5C,0x0F};
                        std::string msg((char*)tmp, sizeof(tmp));
                        ptrCSerialPort->GetPtrSerialPort()->Write(msg);
                    }

                }

                //self->delayed_send(self, milliseconds(100), serialport_atom::value, ptrCSerialPort, type);
            }
    };
}

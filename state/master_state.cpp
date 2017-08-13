#include "master_state.h"
#include "worker_state.h"
#include "raw_state.h"
#include "serialport_state.h"
#include "CSerialPort.h"


CAF_ALLOW_UNSAFE_MESSAGE_TYPE(boost::shared_ptr<CSerialPort>);


behavior agv::Master(stateful_actor<master_state> *self) {

    if (!self->state.init("master")) {
        A_PRINT("break");
        return {};
    }

    A_PRINT("begin");

    A_PRINT("end");

    return {
            [=](init_atom, boost::shared_ptr<CSerialPort> ptrCSerialPort) {
                A_PRINT("received {'init_atom'}");

                g_ptr_worker_actor = boost::shared_ptr<actor>(new actor(self->spawn<linked+detached>(agv::Worker)));
                g_ptr_raw_actor = boost::shared_ptr<actor>(new actor(self->spawn<linked+detached>(agv::Raw)));
                if (ptrCSerialPort)
                {
                    g_ptr_serialport_actor = boost::shared_ptr<actor>(new actor(self->spawn<linked+detached>(agv::Serialport)));
                    anon_send(*g_ptr_serialport_actor, serialport_atom::value, ptrCSerialPort, (unsigned int)0);
                }
            },
            [=](start_manage_server_atom, int port, int pool_size) {
                A_PRINT("received {'start_manage_server_atom'} port = " << port << " pool_size = " << pool_size);

                self->state.ptr_manageserver.reset(new server(port, pool_size));
                if (self->state.ptr_manageserver)
                {
                    self->state.ptr_manageserver->run();

                    A_PRINT("server start");
                }
                else
                {
                    A_PRINT("server start error");
                }
            },
            [=](stop_manage_server_atom) {
                A_PRINT("received {'stop_manage_server_atom'}");

                if(self->state.ptr_manageserver)
                {
                    self->state.ptr_manageserver->stop();
                    self->state.ptr_manageserver.reset();

                    A_PRINT("server stop");
                }
                else
                {
                    A_PRINT("server is null");
                }
            },
            [=](start_screen_server_atom, int port, int pool_size) {
                A_PRINT("received {'start_screen_server_atom'} port = " << port << " pool_size = " << pool_size);

                self->state.ptr_screenserver.reset(new server(port, pool_size));
                if (self->state.ptr_screenserver)
                {
                    self->state.ptr_screenserver->run();

                    A_PRINT("server start");
                }
                else
                {
                    A_PRINT("server start error");
                }
            },
            [=](stop_screen_server_atom) {
                A_PRINT("received {'stop_screen_server_atom'}");

                if(self->state.ptr_screenserver)
                {
                    self->state.ptr_screenserver->stop();
                    self->state.ptr_screenserver.reset();

                    A_PRINT("server stop");
                }
                else
                {
                    A_PRINT("server is null");
                }
            }
    };
}

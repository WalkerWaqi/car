#ifndef CAR_H
#define CAR_H

#include <caf/all.hpp>
#include <caf/io/all.hpp>
using namespace caf;
#include <boost/shared_ptr.hpp>
#include "logger.h"
#include "commhead.h"
#include "TevaProtocols.pb.h"


#define CAR_PATH            "/opt/bin"
#define CAR_NAME            "car"
#define CAR_LOG_CONFIG      CAR_PATH "/" CAR_NAME ".lgp"
#define CAR_DATA_PATH       "/home/walker/bin/data"
#define CAR_DATA_CONFIG     "config"

#define CAR_MODE_MANUAL      0x00000000
#define CAR_MODE_AUTO        0x00000001
#define CAR_MODE_RESET       0x00000002

using init_atom = caf::atom_constant<caf::atom("init")>;
using start_manage_server_atom = caf::atom_constant<caf::atom("s_manages")>;
using stop_manage_server_atom = caf::atom_constant<caf::atom("e_manages")>;
using start_screen_server_atom = caf::atom_constant<caf::atom("s_screens")>;
using stop_screen_server_atom = caf::atom_constant<caf::atom("e_screens")>;

using recv_ack_atom = caf::atom_constant<caf::atom("recv_ack")>;
using get_status_atom = caf::atom_constant<caf::atom("get_status")>;
using status_nty_atom = caf::atom_constant<caf::atom("status_nty")>;
using get_position_atom = caf::atom_constant<caf::atom("get_positi")>;
using set_position_atom = caf::atom_constant<caf::atom("set_positi")>;
using position_nty_atom = caf::atom_constant<caf::atom("positi_nty")>;
using set_sensor_atom = caf::atom_constant<caf::atom("set_sensor")>;
using config_atom = caf::atom_constant<caf::atom("config")>;
using job_start_atom = caf::atom_constant<caf::atom("job_start")>;
using start_action_atom = caf::atom_constant<caf::atom("start_acti")>;
using done_action_atom = caf::atom_constant<caf::atom("done_act")>;
using job_cancel_atom = caf::atom_constant<caf::atom("job_cancel")>;
using trigger_atom = caf::atom_constant<caf::atom("trigger")>;

using turn_atom = caf::atom_constant<caf::atom("turn")>;
using stop_atom = caf::atom_constant<caf::atom("stop")>;
using cancel_atom = caf::atom_constant<caf::atom("cancel")>;
using forward_atom = caf::atom_constant<caf::atom("forward")>;
using back_atom = caf::atom_constant<caf::atom("back")>;
using tracing_forward_atom = caf::atom_constant<caf::atom("t_forward")>;
using tracing_back_atom = caf::atom_constant<caf::atom("t_back")>;
using axle_move_atom = caf::atom_constant<caf::atom("axle_move")>;
using control_x_atom = caf::atom_constant<caf::atom("control_x")>;
using control_y_atom = caf::atom_constant<caf::atom("control_y")>;
using control_z_atom = caf::atom_constant<caf::atom("control_z")>;
//using car_align_atom = caf::atom_constant<caf::atom("car_align")>;
using start_charge_atom = caf::atom_constant<caf::atom("start_char")>;
using stop_charge_atom = caf::atom_constant<caf::atom("stop_charg")>;
using stop_nty_atom = caf::atom_constant<caf::atom("stop_nty")>;

using set_session_atom = caf::atom_constant<caf::atom("s_session")>;

using serialport_atom = caf::atom_constant<caf::atom("serialport")>;


extern boost::shared_ptr<actor> g_ptr_master_actor;
extern boost::shared_ptr<actor> g_ptr_worker_actor;
extern boost::shared_ptr<actor> g_ptr_raw_actor;
extern boost::shared_ptr<actor> g_ptr_serialport_actor;
extern int g_Port;
extern int g_ModbusPort;
extern bool g_first;
extern unsigned int g_mode;
extern agv::Position g_position;
extern agv::Status g_status;

#endif // CAR_H
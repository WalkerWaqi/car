#include "car.h"
#include <iostream>
#include <fstream>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include "OptionPrinter.hpp"
#include "common.h"
#include "server.h"
#include "master_state.h"
#include "timer_state.h"
#include "CommandCreater.h"
#include "CSerialPort.h"
#include "easylogging++.h"


CAF_ALLOW_UNSAFE_MESSAGE_TYPE(boost::shared_ptr<CSerialPort>);

void init_daemon(void);

boost::shared_ptr<actor> g_ptr_master_actor;
boost::shared_ptr<actor> g_ptr_worker_actor;
boost::shared_ptr<actor> g_ptr_raw_actor;
boost::shared_ptr<actor> g_ptr_serialport_actor;
int g_Port = 0;
int g_ModbusPort = 0;
bool g_first = true;
unsigned int g_mode = 0;
agv::Position g_position;
agv::Status g_status;

INITIALIZE_EASYLOGGINGPP
bool stackTraceIfAvailable = false;
const el::Level& level = el::Level::Fatal;
const char* logger = "default";

void myCrashHandler(int sig) {
    LOG(ERROR) << "Woops! Crashed!";
    // FOLLOWING LINE IS OPTIONAL
    el::Helpers::logCrashReason(sig, true);
    // FOLLOWING LINE IS ABSOLUTELY NEEDED AT THE END IN ORDER TO ABORT APPLICATION
    el::Helpers::crashAbort(sig);
}

int main(int argc, char* argv[])
{
    el::Helpers::setCrashHandler(myCrashHandler);

    try
    {
        Init_Logger(CAR_LOG_CONFIG);

        std::string appName = boost::filesystem::basename(argv[0]);

        std::string strSerialPort;

        /** Define and parse the program options
         */
        namespace po = boost::program_options;
        po::options_description desc("Options");
        desc.add_options()
                ("help,h", "Print help messages")
                ("port,p", po::value<int>(&g_Port)->required(), "service port")
                ("modbus,m", po::value<int>(&g_ModbusPort)->default_value(0), "modbus tcp")
                ("serial,s", po::value<std::string>(&strSerialPort), "serial port")
                ("daemon,d", "daemon mode")
                ;

        po::positional_options_description positionalOptions;
        positionalOptions.add("out", 1);

        po::variables_map vm;

        try
        {
            po::store(po::command_line_parser(argc, argv).options(desc)
                              .positional(positionalOptions).run(),
                      vm);

            if ( vm.count("help") )
            {
                std::cout << "generate license file" << std::endl << std::endl;
                rad::OptionPrinter::printStandardAppDesc(appName,
                                                         std::cout,
                                                         desc,
                                                         &positionalOptions);
                return SUCCEED;
            }

            po::notify(vm);

            {
                LOG_INFO("begin");

                // todo
                if ( vm.count("daemon") )
                {
                    LOG_INFO("daemon");
                    init_daemon();
                }

                boost::shared_ptr<CSerialPort> ptrCSerialPort;
                if (!strSerialPort.empty())
                {
                    ptrCSerialPort.reset(new CSerialPort(strSerialPort));
                    ptrCSerialPort->run();
                }

                CommandCreater::init();

                actor_system_config cfg;
                actor_system system{cfg};

                Init_Timer(system);

                {
                    actor_system system{cfg};

                    //g_ptr_master_actor = boost::shared_ptr<actor>(new actor(system.spawn<detached>(agv::master)));
                    scoped_actor self{system};
                    g_ptr_master_actor = boost::shared_ptr<actor>(new actor(self->spawn<detached>(agv::Master)));

                    anon_send(*g_ptr_master_actor, init_atom::value, ptrCSerialPort);

                    anon_send(*g_ptr_master_actor, start_manage_server_atom::value, g_Port, SERVER_POOL_SIZE);
                    if (g_ModbusPort > 0 && g_ModbusPort != g_Port)
                    {
                        anon_send(*g_ptr_master_actor, start_screen_server_atom::value, g_ModbusPort, SERVER_POOL_SIZE);
                    }

                    //anon_send_exit(*g_ptr_master_actor, exit_reason::user_shutdown);

                    //system.await_all_actors_done();
                    self->await_all_other_actors_done();

                    g_ptr_master_actor.reset();
                }

                Exit_Timer();

                system.await_all_actors_done();

                Destory_Timer();

                if (ptrCSerialPort)
                {
                    ptrCSerialPort->stop();
                }

                LOG_INFO("end");
            }
        }
        catch(boost::program_options::required_option& e)
        {
            rad::OptionPrinter::formatRequiredOptionError(e);
            std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
            rad::OptionPrinter::printStandardAppDesc(appName,
                                                     std::cout,
                                                     desc,
                                                     &positionalOptions);
            return ERROR_IN_COMMAND_LINE;
        }
        catch(boost::program_options::error& e)
        {
            std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
            rad::OptionPrinter::printStandardAppDesc(appName,
                                                     std::cout,
                                                     desc,
                                                     &positionalOptions);
            return ERROR_IN_COMMAND_LINE;
        }
    }
    catch(std::exception& e)
    {
        std::cerr << "Unhandled Exception reached the top of main: "
                  << e.what() << ", application will now exit" << std::endl;
        return FAILED;
    }

    return SUCCEED;
}
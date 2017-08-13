//
// Created by waqi on 12/6/16.
//

#include "logger.h"


Logger  g_Root_Logger;
Logger  g_Session_Logger;
Logger  g_Timer_Logger;
Logger  g_Command_Logger;
Logger  g_SerialPort_Logger;
Logger  g_Actor_Logger;


int Init_Logger(const char* config_file)
{
    log4cplus::initialize ();
    LogLog::getLogLog()->setInternalDebugging(true);
    PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT(config_file));

    g_Root_Logger       = Logger::getRoot();
    g_Session_Logger    = Logger::getInstance("session");
    g_Timer_Logger      = Logger::getInstance("timer");
    g_Command_Logger    = Logger::getInstance("command");
    g_SerialPort_Logger = Logger::getInstance("serialport");
    g_Actor_Logger      = Logger::getInstance("actor");

    return 0;
}
//
// Created by waqi on 12/6/16.
//

#ifndef CAR_LOGGER_H
#define CAR_LOGGER_H


#ifndef NO_LOG4CPLUS
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/helpers/fileinfo.h>
#include <log4cplus/loggingmacros.h>
using namespace log4cplus;
using namespace log4cplus::helpers;
#else
#define LOG4CPLUS_INFO(logger, logEvent)
#define LOG4CPLUS_ERROR(logger, logEvent)
#endif

extern Logger  g_Root_Logger;
extern Logger  g_Session_Logger;
extern Logger  g_Timer_Logger;
extern Logger  g_Command_Logger;
extern Logger  g_SerialPort_Logger;
extern Logger  g_Actor_Logger;

int Init_Logger(const char* config_file);

#define LOG_INFO(logEvent) \
LOG4CPLUS_INFO(g_Root_Logger, logEvent);

#define LOG_ERROR(logEvent) \
LOG4CPLUS_ERROR(g_Root_Logger, logEvent);

#define LOG_WARN(logEvent) \
LOG4CPLUS_WARN(g_Root_Logger, logEvent);

#define LOG_SESSION_INFO(logEvent) \
LOG4CPLUS_INFO(g_Session_Logger, logEvent);

#define LOG_SESSION_ERROR(logEvent) \
LOG4CPLUS_ERROR(g_Session_Logger, logEvent);

#define LOG_SESSION_WARN(logEvent) \
LOG4CPLUS_WARN(g_Session_Logger, logEvent);

#define LOG_TIMER_INFO(logEvent) \
LOG4CPLUS_INFO(g_Timer_Logger, logEvent);

#define LOG_TIMER_ERROR(logEvent) \
LOG4CPLUS_ERROR(g_Timer_Logger, logEvent);

#define LOG_TIMER_WARN(logEvent) \
LOG4CPLUS_WARN(g_Timer_Logger, logEvent);

#define LOG_COMMAND_INFO(logEvent) \
LOG4CPLUS_INFO(g_Command_Logger, logEvent);

#define LOG_COMMAND_ERROR(logEvent) \
LOG4CPLUS_ERROR(g_Command_Logger, logEvent);

#define LOG_COMMAND_WARN(logEvent) \
LOG4CPLUS_WARN(g_Command_Logger, logEvent);

#define LOG_SERIALPORT_INFO(logEvent) \
LOG4CPLUS_INFO(g_SerialPort_Logger, logEvent);

#define LOG_SERIALPORT_ERROR(logEvent) \
LOG4CPLUS_ERROR(g_SerialPort_Logger, logEvent);

#define LOG_SERIALPORT_WARN(logEvent) \
LOG4CPLUS_WARN(g_SerialPort_Logger, logEvent);


#endif //CAR_LOGGER_H

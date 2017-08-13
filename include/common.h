//
//  common.h
//  common
//
//  Created by Walker on 16-11-22.
//
//

#ifndef COMMON_H
#define COMMON_H

#define SUCCEED                 0
#define FAILED                  -1
#define ERROR_IN_COMMAND_LINE   -2

#define CONNECTION_TIMEOUT      30 * 1000 // ms
#define SEND_TIMEOUT            5 * 1000  // ms
#define COMMAND_LONG_TIMEOUT    1000 * 60 * 1000
#define COMMAND_SHORT_TIMEOUT   60 *1000

#define SERVER_POOL_SIZE        5

#endif // COMMON_H

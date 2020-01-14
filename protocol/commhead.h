//
//  commhead.h
//  commhead
//
//  Created by Walker on 16-11-13.
//
//

#ifndef COMM_HEAD_H
#define COMM_HEAD_H

#pragma pack(1)

#define PROTOCOL_VERSION	1

#define RFID_SIZE		12

#define CHANNEL_ADMIN       0x00
#define CHANNEL_MANAGE      0x01
#define CHANNEL_RAW         0x02
#define CHANNEL_RAW_COMMAND 0xf2
#define CHANNEL_MODBUS      0xff

typedef struct _COMM_HEAD {
	unsigned char         version;    // 0x01
	unsigned char         channel;    // 0x00 admin; 0x01 control; 0x02 raw
	union {
		unsigned short    id;
		struct {
			unsigned char compress;
			unsigned char encrypt;
		};
	};
	unsigned int          size;
	unsigned char         data[0];
} COMM_HEAD, *PCOMM_HEAD;

#define	ERROR_NONE					0
#define	ERROR_GENERAL				-1
#define ERROR_EXCEPTION             -2
#define	ERROR_PENDING				-3
#define	ERROR_CANCEL                -4
#define	ERROR_BLOCKING		        -5
#define	ERROR_TIMEOUT		        -6
#define	ERROR_UNKONWN_COMMAND       -7
#define ERROR_DERAIL				-8

typedef struct _COMM_BODY {	
	unsigned int		command;
	int					error;
	unsigned char		params[0];
} COMM_BODY, *PCOMM_BODY;

#define	COMMAND_ACK					0x00
#define COMMAND_WEAKUP				0x01
#define COMMAND_HEARTBEAT			0x02

#define COMMAND_POSITION			0x03
#define COMMAND_SET_POSITION		0x04
typedef struct _POSITION_COMMAND {
	unsigned char	rfid[RFID_SIZE];
	unsigned int	offset;
	unsigned int	offset_angle;
	unsigned int	angle;
	unsigned int	speed; // cm/s
} POSITION_COMMAND, *PPOSITION_COMMAND, POSITION_ACK, *PPOSITION_ACK;

#define	COMMAND_TURN				0x05
typedef struct _TURN_COMMAND {
	int	angle;
} TURN_COMMAND, *PTURN_COMMAND;
// ack add POSITION_ACK

#define	COMMAND_STOP				0x06
#define	COMMAND_FORWARD				0x07
#define	COMMAND_BACK				0x08
#define	COMMAND_CANCEL				0x09
#define COMMAND_TRACING_FORWARD		0x0a
#define COMMAND_TRACING_BACK		0x0b
typedef struct _TRACING_COMMAND {
	unsigned char	rfid[RFID_SIZE];
	unsigned int	speed; // cm/s
	unsigned int	flags; //
} TRACING_COMMAND, *PTRACING_COMMAND;
// ack add POSITION_ACK

#define	COMMAND_AXLE_MOVE			0x0c
#define	RESET					0x0
#define	AXLE_X					0x1
#define AXLE_Y					0x2
#define	AXLE_Z					0x3
#define ADJUST_X				0x4
#define	ADJUST_Y				0x5
#define ADJUST_Z				0x6
#define MOTOR					0x7
#define	CYLINDER				0x8
#define	CAR_FORWARD_TO_RFID		0x9
#define	CAR_BACK_TO_RFID		0xa
#define	CAR_ALIGN				0xb
#define	CAR_TURN				0xc
#define	WAIT					0xd
typedef struct _AXLE_MOVE_COMMAND {
	unsigned char	axle;
	//int		value; // unit mm; +: up, right, forward; -: down, left, back;
	union {
		struct {
			char	value_char_0;
			char	value_char_1;
			char	value_char_2;
			char	value_char_3;
		};
		struct {
			short	value_short_0;
			short	value_short_1;
		};
		struct {
			int	value;
		};
	};
} AXLE_MOVE_COMMAND, *PAXLE_MOVE_COMMAND;

#define	COMMAND_STATUS				0x0d
#define BLOCKING_MECHANICAL		0x1		// mechanical
#define BLOCKING_FRONT			0x2
#define BLOCKING_BACK			0x4
#define BLOCKING_UP				0x8
typedef struct _STATUS_ACK {
	unsigned char	battery;
	unsigned char	blocking;
	unsigned char	empty;
} STATUS_ACK, *PSTATUS_ACK;

/*#define	COMMAND_CAR_ALIGN		0x0e
typedef struct _CAR_ALIGN_COMMAND {
	int	angle;
} CAR_ALIGN_COMMAND, *PCAR_ALIGN_COMMAND;*/

#define COMMAND_START_CHARGE		0x0f
#define COMMAND_STOP_CHARGE 		0x10
typedef struct _CHARGE_COMMAND {
	unsigned char	station;
} CHARGE_COMMAND, *PCHARGE_COMMAND;
// notify add STATUS_ACK

#define COMMAND_CONTROL_X			0x11
#define COMMAND_CONTROL_Y			0x12
#define COMMAND_CONTROL_Z			0x13
typedef struct _CONTROL_COMMAND {
	int		speed;
} CONTROL_COMMAND, *PCONTROL_COMMAND;

#define COMMAND_SET_SENSOR			0x14
typedef struct _SENSOR_COMMAND {
	unsigned char	sensor_front;
	unsigned char	sensor_front_out;
	unsigned char	sensor_back;
	unsigned char	sensor_back_out;
	unsigned char	sensor_up;
	unsigned char	sensor_up_out;
	unsigned char	rfid[0];
} SENSOR_COMMAND, *PSENSOR_COMMAND;

#define	NOTIFY_STOP					0x100
// notify add POSITION_ACK
// notify add STATUS_ACK

#define	NOTIFY_POSITION				0x101
// notify add POSITION_ACK

#define NOTIFY_STATUS				0x102
// notify add STATUS_ACK

#pragma pack()

#endif // COMM_HEAD_H

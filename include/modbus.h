//
// Created by waqi on 1/18/17.
//

#ifndef CAR_MODBUS_H
#define CAR_MODBUS_H


#pragma pack(1)

typedef struct _MBAP {
    unsigned short  transaction_id;
    unsigned short  protocol_id;
    unsigned short  length;
    unsigned char   unit_id;
    unsigned char   function_code;
    unsigned char   data[0];
} MBAP, *PMBAP;

typedef struct _RTU {
    unsigned char   slav_id;
    unsigned char   function_code;
    unsigned char   data[0];
} RTU, *PRTU;

typedef struct _RTU_ACK {
    unsigned char   size;
    unsigned char   data;
    unsigned short  crc;
} RTU_ACK, *PRTU_ACK;

#define RTU_FORWARD     0x01
#define RTU_LEFT        0x02
#define RTU_BACK        0x04
#define RTU_RIGHT       0x08
#define RTU_AUTO        0x10
#define RTU_RESET       0x20

#define RTU_Z_UP        0x01
#define RTU_Z_DOWN      0x02
#define RTU_Y_FORWARD   0x04
#define RTU_Y_BACK      0x08
#define RTU_X_LEFT      0x10
#define RTU_X_RIGHT     0x20
#define RTU_TRIGGER     0x40

#pragma pack()


#endif //CAR_MODBUS_H

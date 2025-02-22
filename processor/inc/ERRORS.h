#ifndef _ERROR_H_
#define _ERROR_H_

enum ERROR_CODE
{
    NO_ERROR          = 0,
    ERROR             = 1,
    STK_ADDRESS_ERROR = -1,
    ADDRESS_ERROR     = -2,
    CHANGE_ERROR      = -3,
    FILE_ERROR        = -4,
    OPEN_FILE_ERROR   = -5,
    CLOSE_FILE_ERROR  = -6,
    COMMAND_ERROR     = -7,
    ERROR_ARG         = -8,
    POP_ERROR         = -9,
    SPU_ADDRESS_ERROR = -10,
    GET_SIZE_ERROR    = -11,
    ERROR_READ_FROM_FILE = -12
};

#endif // _ERROR_H_

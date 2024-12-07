#ifndef _ERROR_H_
#define _ERROR_H_

enum ERROR_CODE
{
    NO_ERROR          = 0,
    ERROR             = 1,
    CANARY_ERROR      = -2,
    STK_ADDRES_ERROR  = -3,
    DATA_ERROR        = -4,
    DATA_CANARY_ERROR = -5,
    HASH_ERROR        = -6,
    CHANGE_ERROR      = -7,
    ADDRES_ERROR      = -8,
    FILE_ERROR        = -9,
    OPEN_FILE_ERROR   = -10,
    FILE_CLOSE_ERROR  = -11,
    COMMAND_ERROR     = -12,
    ERROR_PUSH_ARG    = -13
};

#endif // _ERROR_H_

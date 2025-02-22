#ifndef _COLORS_H_
#define _COLORS_H_

#define RESET_COLOR  "\033[0m"
#define RED          "\033[1;31m"
#define MAGN         "\033[1;35m"
#define WHITE        "\033[1;37m"
#define GREEN        "\033[1;32m"
#define BLUE         "\033[1;34m"
#define CYAN         "\033[1;36m"
#define YELLOW       "\033[1;33m"

#define RED_TEXT(str)    printf(RED str RESET_COLOR)
#define MAGN_TEXT(str)   printf(MAGN str RESET_COLOR)
#define WHITE_TEXT(str)  printf(WHITE str RESET_COLOR)
#define GREEN_TEXT(str)  printf(GREEN str RESET_COLOR)
#define BLUE_TEXT(str)   printf(BLUE str RESET_COLOR)
#define CYAN_TEXT(str)   printf(CYAN str RESET_COLOR)
#define YELLOW_TEXT(str) printf(YELLOW str RESET_COLOR)

#endif //_COLORS_H_

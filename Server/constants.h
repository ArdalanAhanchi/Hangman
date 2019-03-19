#ifndef CONSTANTS_H
#define CONSTANTS_H

//Operation codes (Only one byte).
#define OP_CREATE_ACT 1
#define OP_LOGIN 2
#define OP_JOIN 3
#define OP_JOIN_RID 4
#define OP_HOST 5
#define OP_CHECK_GAMEOVER 6
#define OP_GET_PLAYERS 7
#define OP_WON 8
#define OP_LOST 9
#define OP_GET_WORD 10
#define OP_GET_BOARD 11
#define OP_UPDATE 12
#define OP_GET_ROOMS 13
#define OP_GET_PLAYER_COUNT 14
#define OP_UNREGISTER 15
#define OP_LOGOUT 16

//Status codes (Only one byte).
#define S_OK 1
#define S_ERROR 2
#define S_NOT_IN_ROOM 3
#define S_ROOM_FULL 4
#define S_INVALID_ROOM_ID 5
#define S_AUTH_ERROR 6
#define S_INVALID_USER_PASS 7
#define S_GAME_OVER 8
#define S_NOT_LOGGED_IN 9

#endif

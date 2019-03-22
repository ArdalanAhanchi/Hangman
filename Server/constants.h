#ifndef CONSTANTS_H
#define CONSTANTS_H

//Operation codes (Only one byte).
#define OP_CREATE_ACT 'A'
#define OP_LOGIN 'B'
#define OP_JOIN 'C'
#define OP_JOIN_RID 'D'
#define OP_HOST 'E'
#define OP_CHECK_GAMEOVER 'F'
#define OP_GET_PLAYERS 'G'
#define OP_WON 'H'
#define OP_LOST 'I'
#define OP_GET_WORD 'J'
#define OP_GET_BOARD 'K'
#define OP_UPDATE 'L'
#define OP_GET_ROOMS 'M'
#define OP_GET_PLAYER_COUNT 'N'
#define OP_UNREGISTER 'O'
#define OP_LOGOUT 'P'
#define OP_LEAVE 'Q'

//Status codes (Only one byte).
#define S_OK 'A'
#define S_ERROR 'B'
#define S_REG_INVALID_USER 'C'
#define S_REG_INVALID_PASS 'D'
#define S_REG_USERNAME_EXISTS 'E'
#define S_AUTH_INVALID_USER 'F'
#define S_AUTH_INVALID_PASS 'G'
#define S_NOT_LOGGED_IN 'H'
#define S_ROOM_FULL 'I'
#define S_INVALID_ROOM_ID 'J'
#define S_GAME_OVER 'K'

#endif

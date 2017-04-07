#ifndef TODO_DEFS_HH
#define TODO_DEFS_HH

#define TD_VERSION          "1.0"

#define CMDK_ENTER       0x000A
#define CMDK_ESCAPE      0x001B
#define CMDK_EXIT        0x0071
#define CMDK_START_CMD   0x003A
#define CMDK_EDIT        0x0065
#define CMDK_UNDO        0x0075
#define CMDK_ARROW_DOWN  0x0102
#define CMDK_ARROW_UP    0x0103
#define CMDK_ARROW_LEFT  0x0104
#define CMDK_ARROW_RIGHT 0x0105
#ifdef __APPLE__
#define CMDK_DELETE      0x0107 //TODO just switched? fix?
#define CMDK_BACKSPACE   0x007F
#elif __linux__
#define CMDK_DELETE      0x007F
#define CMDK_BACKSPACE   0x0107
#else
#error "Unknown compiler"
#endif

//< range of printable characters
#define CMDK_VALID_START 0x0020
#define CMDK_VALID_END   0x007E

//< pseudo keys
#define CMDK_TRIGGERED    0x00FF

typedef struct {
  int scr_x;
  int scr_y;
} td_screen_pos_t;
#define TD_SCREEN_POS_INITIALIZER { 0, 0 }

#ifdef TD_DEBUG
#define __dbg
#else
#define __dbg __attribute__((unused))
#endif

#endif // #ifndef TODO_DEFS_HH

#ifndef TODO_DEFS_HH
#define TODO_DEFS_HH

#define TD_TITLE            "TODO-List"
#define TD_VERSION          "1.0"
#define TD_MARKER           " -=-=-=-=- "
#define TD_HEADER_STR       TD_MARKER TD_TITLE " v. " TD_VERSION TD_MARKER
#define TD_HEADER_SIZE      sizeof(TD_HEADER_STR)

#define CMDK_ENTER 0x0A
#define CMDK_ESCAPE 0x1B
#define CMDK_EXIT 0x71
#define CMDK_START_CMD 0x3A
#define CMDK_EDIT 0x65
#define CMDK_UNDO 0x75
#define CMDK_ARROW_DOWN 0x102
#define CMDK_ARROW_UP 0x103
#define CMDK_ARROW_LEFT 0x104
#define CMDK_ARROW_RIGHT 0x105
#ifdef __APPLE__
#define CMDK_DELETE 0x107 //TODO just switched? fix?
#define CMDK_BACKSPACE 0x7F
#elif __linux__
#define CMDK_DELETE 0x7F
#define CMDK_BACKSPACE 0x107
#else
#error "Unknown compiler"
#endif

typedef struct {
  int scr_x;
  int scr_y;
} td_screen_pos_t;
#define TD_SCREEN_POS_INITIALIZER { 0, 0 }

typedef struct {
  void ** params;
  unsigned int param_count;
  void (*callback_wrapper)(void ** params);
} td_callback_wrapper_t;

#endif // #ifndef TODO_DEFS_HH

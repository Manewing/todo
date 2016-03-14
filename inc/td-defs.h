#ifndef TODO_DEFS_HH
#define TODO_DEFS_HH

#define TD_TITLE            "TODO-List"
#define TD_VERSION          "1.0"
#define TD_MARKER           " -=-=-=-=- "
#define TD_HEADER_STR       TD_MARKER TD_TITLE " v. " TD_VERSION TD_MARKER
#define TD_HEADER_SIZE      sizeof(TD_HEADER_STR)

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

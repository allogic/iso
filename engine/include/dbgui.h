#ifndef DBGUI_H
#define DBGUI_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void dbgui_create(void);
void dbgui_draw(void);
void dbgui_message(HWND window_handle, UINT window_message, WPARAM w_param, LPARAM l_param);
void dbgui_destroy(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DBGUI_H

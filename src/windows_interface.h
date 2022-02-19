#ifndef WINDOWS_INTERFACE_H
#define WINDOWS_INTERFACE_H

#include <Windows.h>

class WindowsInterface
{

public:
  unsigned int getWindowWidth(HWND hwnd);
  unsigned int getWindowHeight(HWND hwnd);
  HWND getWID(unsigned long pid);

private:
  struct handle_data
  {
    unsigned long process_id;
    HWND window_handle;
  };
  RECT rect;
  static BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam);
  static BOOL is_main_window(HWND handle);
};

#endif // WINDOWS_INTERFACE_H

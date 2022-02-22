#ifndef WINDOWS_INTERFACE_H
#define WINDOWS_INTERFACE_H

#include <Windows.h>

class WindowsInterface
{

public:
  HWND getWID(PROPID pid);

private:
  struct handle_data
  {
    PROPID process_id;
    HWND window_handle;
  };
  static BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam);
  static BOOL is_main_window(HWND handle);
};

#endif // WINDOWS_INTERFACE_H

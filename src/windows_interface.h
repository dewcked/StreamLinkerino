#ifndef WINDOWS_INTERFACE_H
#define WINDOWS_INTERFACE_H

#include <Windows.h>
#include <stdint.h>
class WindowsInterface
{

public:
  // static void ControlVolume(HWND handle, bool isPositive, int quantity);
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

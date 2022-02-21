#include "windows_interface.h"

///* https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getwindowrect
// *
// * Return given window's width
// */
// unsigned int WindowsInterface::getWindowWidth(HWND hwnd)
//{
//  GetWindowRect(hwnd, &rect);
//  return rect.right - rect.left;
//}

///* https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getwindowrect
// *
// * Return given window's height
// */
// unsigned int WindowsInterface::getWindowHeight(HWND hwnd)
//{
//  GetWindowRect(hwnd, &rect);
//  return rect.bottom - rect.top;
//}

/* Code of 'getWID', 'enum_windows_callback', 'is_main_window' from
 * https://stackoverflow.com/questions/1888863/how-to-get-main-window-handle-from-process-id
 *
 * Return window ID of given process ID
 */

// void ControlVolume(HWND handle, bool isPositive, int quantity)
//{
//   return; // SendMessage(handle, WM_APPCOMMAND, 0, (long long)APPCOMMAND_VOLUME_MUTE);
// }

HWND WindowsInterface::getWID(unsigned long pid)
{
  handle_data data;
  data.process_id = pid;
  data.window_handle = 0;
  EnumWindows(WindowsInterface::enum_windows_callback, (LPARAM)&data);
  return data.window_handle;
}

BOOL CALLBACK WindowsInterface::enum_windows_callback(HWND handle, LPARAM lParam)
{
  handle_data &data = *(handle_data *)lParam;
  unsigned long process_id = 0;
  GetWindowThreadProcessId(handle, &process_id);
  if (data.process_id != process_id || !is_main_window(handle))
    return TRUE;
  data.window_handle = handle;
  return FALSE;
}

BOOL WindowsInterface::is_main_window(HWND handle)
{
  return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

#include "windows_interface.h"

/* Code of 'getWID', 'enum_windows_callback', 'is_main_window' from
 * https://stackoverflow.com/questions/1888863/how-to-get-main-window-handle-from-process-id
 *
 * Return window ID of given process ID
 */

HWND WindowsInterface::getWID(PROPID pid)
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
  PROPID process_id = 0;
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

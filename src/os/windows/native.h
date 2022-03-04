#pragma once
#include <Windows.h>

namespace streamlinkerino {
  class WindowInterface {
   public:
    HWND getWID(PROPID pid);

   private:
    struct handle_data {
      PROPID process_id;
      HWND window_handle;
    };
    static BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam);
    static BOOL is_main_window(HWND handle);
  };
} // namespace streamlinkerino

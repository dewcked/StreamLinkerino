#include "darkmode.h"

#ifdef Q_OS_WIN
namespace streamlinkerino{
  bool isDarkTheme()
  {
    return WinDark::isDarkTheme();
  }
  void setDarkTitlebar(WId winId)
  {
    WinDark::setDark_Titlebar(reinterpret_cast<HWND>(winId));
  }
}
#else
namespace streamlinkerino{
  bool isDarkTheme()
  {
    return false;
  }
  void setDarkTitlebar(WId winId)
  {
    return;
  }
}
#endif
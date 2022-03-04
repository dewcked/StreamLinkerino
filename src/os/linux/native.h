// Explictly including QActionGroup fixes compile bug on linux
#pragma once
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <qactiongroup.h>

#include <QDebug>

// Qt Compatibility
#undef Bool
#ifdef FrameFeature
#undef FrameFeature
#endif

namespace streamlinkerino {
  class WindowInterface {
   public:
    WindowInterface();
    Window getWID(unsigned long pid);

   private:
    void search(Window w);

    unsigned long _pid;
    Atom _atomPID;
    Display* _display;
    Window _result;
  };
} // namespace streamlinkerino

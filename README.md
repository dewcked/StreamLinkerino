## Streamlinkerino for Windows

This project is an unofficial window porting of Streamlinkerino. For more information, take a look at [Official Streamlinkerino README page](https://github.com/JohnCiubuc/StreamLinkerino#official-chatterino-implementation).

## Experimental Features

Windows port of Streamlinkerino provides experimental features. As porting isn't completely done, some features from original project may be possible and some maybe not.

### Known Issues and bugs

1. Streamlinkerino implementation use `--wid` argument to execute streamlink with mpv player, but in Windows original features of mpv player is not possible.
2. Focusing problem occurs between embeded chatterino window and main window. So if shortcuts doesn't work, just click title bar or screen (not Chatterino area) once. Also, **double-click Chatterino channel to load stream** function makes main window lose focus.
3. Currently, `load stream` function needs specific logic that prompts and close a window briefly.
4. Don't ever try to touch minimize button in embedded Chatterino. If you did so, only way to recover Chatterino is to restart or click `load stream` from context menu.
5. Settings and screen messages doesn't work properly. Gonna fix this soon..

### Features

- press `f` or `double-click screen` to enable/disable fullscreen mode
- press `w` to enable/disable frameless window mode
- press `esc` to disable fullscreen
- `double-click Chatterino channel` to load stream
- right-click screen to open context menu
  - You can open `Settings`, `Load Stream` or `Close` application

### Usabilities

- Windows dark theme supported. To turn on this, press `Window logo` button and search setting `dark mode`. This is supported only for upper than specific version of Windows 10
- Auto screen resize
  - Responsive for both resizing Chatterino and main window
- Fit screen edge and adjusted screen position to center
- Draggable screen (Like default window drag behavior)

## Building

[My gist](https://gist.github.com/dewcked) shows how to Install Qt framework with Aqtinstall in Windows. Also, for code formatting. I recommend you to use Qt Creator for editting source codes.

To compile your project, just clck build button at bottom-left edge. You can find build directory in Project tab (in left area).

To deploy this to normal users, you need to use `windeployqt.exe` in Qt directory. In my case, Qt directory was `C:\Qt\6.3.0\msvc2019_64\bin`.

move executable file (generated from build) into empty path and run command like below:

```
C:\Qt\6.3.0\msvc2019_64\bin\windeployqt.exe <Path-to-executable>
```

then everything is done automatically. Just ZIP it and deploy.

## Screenshots

Not yet :)

## Streamlinkerino for Windows :beer:

This project is Windows version of Streamlinkerino. For more information, take a look at [Official Streamlinkerino README page](https://github.com/JohnCiubuc/StreamLinkerino#official-chatterino-implementation).

## Download

Click [Releases ](https://github.com/dewcked/StreamLinkerino/releases)to download latest release.

## Experimental Features :clap:

Windows port of Streamlinkerino provides experimental features. Some features here might be updated to the master branch, or kept here only for this own repo.

### Known Issues and bugs :anger:

1. Currently, `load stream` function needs specific logic that prompts and close a window briefly.
2. Don't ever try to touch minimize button in embedded Chatterino. If you did so, only way to recover Chatterino is to restart or click `load stream` from context menu. (Or you can intentionally quit the Chatterino window and watch stream only. To prevent accidental exit of Chatterino window, the first time Chatterino window is reopened.)
3. Resizing Chatterino window stutters...
4. If any other issue occurs, feel free to contact via email - dewcked@protonmail.ch

### Features (v1.0.0) :angel:

- press `f` or `double-click screen` to enable/disable **fullscreen mode**
- press `w` to enable/disable **frameless window mode**
- press `t` to enable/disable **window always on top mode**
- press `esc` to disable fullscreen mode
- right-click screen to open context menu
  - You can open `Settings`, `Load Stream` or `Close` application
- load stream of currently active channel
  - **Double-click Chatterino channel**
  - Or right click in left area > click `Load Stream`

- volume control
  - press `+` `=` `0` `)` `volume up` or mouse `wheelright` `wheelup` to increase volume
  - press `_` `-` `9` `(` `volume down` or mouse `wheelleff` `wheeldown` to decrease volume
  - press `m` `mute` to mute volume
  - press `ctrl` + `+` to change audio/video sync by delaying the audio
  - press `ctrl` + `-` to change audio/video sync by shifting the audio earlier

- playback control
  - press `space` `pause` `play` `playpause` `p` to toggle pause / replay
  - press `[` to decrease current playback speed by 10%
  - press `]` to increase current playback speed by 10%
  - press `{` to halve current playback speed
  - press `}` to double current playback speed
  - press `backspace` to reset playback speed to normal
  - press `.` to step forward one frame and stop (backward `,` doesn't work)

- press `s` to take a screenshot
- press `d` to activate/deactivate deinterlacer
- modify stream output
  - press `1` to decrease, `2` to increase contrast value
  - press `3` to decrease, `4` to increase brightness value
  - press `5` to decrease, `6` to increase gamma value
  - press `7` to decrease, `8` to decrease saturation value

- zoom and move around
  - press `alt` + `+` to zoom in, `alt` + `-` to zoom out
    - `alt` + `=` and `alt` + `_` also works

  - press `alt` + `backspace` to set default zoom
  - press `alt` + `→` `←` `↑` `↓`  to move stream


### Usabilities :hand:

- Windows dark theme supported. To turn on this, press `Window logo` button and search setting `dark mode`. This is supported only for upper than specific version of Windows 10
- Auto screen resize
  - Responsive for both resizing Chatterino and main window. Still stuttering and searching for fix
- Draggable screen (Like default window drag behavior)

## Building :hammer:

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

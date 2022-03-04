#include "../path.h"

#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

namespace streamlinkerino {
  namespace path {
    QString WINDOW_LAYOUT_JSON = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                                 + "/Chatterino2/Settings/window-layout.json";
    QStringList ROOTPATH = []() -> QStringList {
      QStringList rootpath;
      foreach (const QFileInfo& drive, QDir::drives()) {
        rootpath.push_back(drive.path() + "Program Files/");
        rootpath.push_back(drive.path() + "Program Files (x86)/");
      }
      return rootpath;
    }();
    int EXE_NAME_LEN = 14;
    QString CHATTERINO_EXE = "chatterino.exe";
    QString CHATTERINO_EXE_PATH = "Chatterino/chatterino.exe";
    QString STREAMLINK_EXE = "streamlink.exe";
    QString STREAMLINK_EXE_PATH = "streamlink/bin/streamlink.exe";
    bool MPVPLAYER_AVAILABLE = []() -> bool {
      QStringList envVars = qEnvironmentVariable("PATH").split(";");
      foreach (const QString& envVar, envVars)
        if (envVar.contains("mpv")) {
          QFileInfo executable;
          if (envVar.back() == '/')
            executable = QFileInfo(envVar + "mpv.exe");
          else
            executable = QFileInfo(envVar + "/mpv.exe");
          if (executable.exists() && executable.isFile())
            return true;
        }
      return false;
    }();
    QString NAMED_PIPE = "\\\\.\\pipe\\mpvsocket";
  } // namespace path
} // namespace streamlinkerino

#include "../path.h"

#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

namespace streamlinkerino {
  namespace path {
    QString WINDOW_LAYOUT_JSON = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                                 + "/chatterino/Settings/window-layout.json";
    QStringList ROOTPATH = QStringList() << "/usr/bin/"
                                         << "/bin/"
                                         << "/usr/local/bin/"
                                         << "~/.local/bin/";
    QString EXECUTABLE_EXTENSION = "";
    int EXE_NAME_LEN = 10;
    QString CHATTERINO_EXE = "chatterino";
    QString CHATTERINO_EXE_PATH = "chatterino";
    QString STREAMLINK_EXE = "streamlink";
    QString STREAMLINK_EXE_PATH = "streamlink";
    bool MPVPLAYER_AVAILABLE = true;
    QString NAMED_PIPE = "/tmp/mpvsocket";
  } // namespace path
} // namespace streamlinkerino

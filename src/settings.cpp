#include "settings.h"

#include <QFileInfo>
#include <QMessageBox>

#include "debug.h"
#include "os/darkmode.h"
#include "path.h"
#include "type.h"

namespace streamlinkerino {

  QString Settings::streamlinkPath() { return _streamlinkPath; }
  QString Settings::chatterinoPath() { return _chatterinoPath; }
  QStringList Settings::streamlinkArguments(QString channel, WId mpvContainer, int pipeNumber) {
    QStringList args;
    args << "--twitch-low-latency";
    args << "--twitch-disable-ads";
    args << "--player";
    args << "mpv --wid=" + QString::number(mpvContainer) + " --input-ipc-server="
                + streamlinkerino::path::NAMED_PIPE + QString::number(pipeNumber);
    args << "https://www.twitch.tv/" + channel;
    args << _streamQuality;
    return args;
  }

  void Settings::initialize() {
    qDebug2() << "Settings::initialize()";
    QFileInfo executable;

    // Load settings
    streamlinkerino::g::localSettings.beginGroup("preferences");
    _streamQuality = streamlinkerino::g::localSettings.value("streamlink_quality", "").toString();
    _streamlinkPath = streamlinkerino::g::localSettings.value("streamlink", "").toString();
    _streamlinkArgs = streamlinkerino::g::localSettings.value("streamlink_args", "").toString();
    _chatterinoPath = streamlinkerino::g::localSettings.value("chatterino", "").toString();
    qDebug2_() << "(settings)_streamlinkPath: " << _streamlinkPath;
    qDebug2_() << "(settings)_streamlinkArgs: " << _streamlinkArgs;
    qDebug2_() << "(settings)_chatterinoPath: " << _chatterinoPath;
    qDebug2_() << "(settings)_streamlinkQuality: " << _streamQuality;

    // Load stream quality (default: "Best")
    const QStringList streamQuality(
        {"Best", "1080p60", "720p60", "720p", "480p", "360p", "160p", "Worst", "Audio only"});
    if (streamQuality.contains(_streamQuality))
      streamlinkerino::g::localSettings.setValue("streamlink_quality", _streamQuality);
    else
      _streamQuality = "Best";

    // Search Streamlink paths, load if possible
    executable = QFileInfo(_streamlinkPath);
    if (!executable.exists() || !executable.isFile()) {
      _streamlinkPath = "";
      foreach (const QString& rootpath, streamlinkerino::path::ROOTPATH) {
        executable = QFileInfo(rootpath + streamlinkerino::path::STREAMLINK_EXE_PATH);
        if (executable.exists() && executable.isFile()) {
          _streamlinkPath = rootpath + streamlinkerino::path::STREAMLINK_EXE_PATH;
          streamlinkerino::g::localSettings.setValue("streamlink", _streamlinkPath);
          break;
        }
      }
    }
    // Search Chatterino paths, load if possible
    executable = QFileInfo(_chatterinoPath);
    if (!executable.exists() || !executable.isFile()) {
      _chatterinoPath = "";
      foreach (const QString& rootpath, streamlinkerino::path::ROOTPATH) {
        executable = QFileInfo(rootpath + streamlinkerino::path::CHATTERINO_EXE_PATH);
        if (executable.exists() && executable.isFile()) {
          _chatterinoPath = rootpath + streamlinkerino::path::CHATTERINO_EXE_PATH;
          streamlinkerino::g::localSettings.setValue("chatterino", _chatterinoPath);
          break;
        }
      }
    }
    streamlinkerino::g::localSettings.endGroup();
    qDebug2_() << "(searched)_streamlinkPath:" << _streamlinkPath;
    qDebug2_() << "(searched)_streamlinkArgs:" << _streamlinkArgs;
    qDebug2_() << "(searched)_chatterinoPath:" << _chatterinoPath;
    qDebug2_() << "(searched)_streamlinkQuality:" << _streamQuality;
    qDebug2_() << "MPVPlayreAvailable:" << streamlinkerino::path::MPVPLAYER_AVAILABLE;

    int missing = (streamlinkerino::path::MPVPLAYER_AVAILABLE ? 0 : 1)
                  + (_streamlinkPath.isEmpty() ? 2 : 0) + (_chatterinoPath.isEmpty() ? 4 : 0);
    qDebug2_() << "Missing: " << missing;
    if (missing != 0)
      Settings::showMessageBox(missing);
    else {
      isInit = false;
      emit initialized();
    }
  }
  void Settings::showMessageBox(int missing) {
    QMessageBox alert;
    QString text;
    alert.setWindowTitle("Streamlinkerino");
    alert.setWindowIcon(QIcon(":resource/Streamlinkerinoicon.ico"));
    if ((1 & missing) != 0) { // If MPVPlayer not found
      text = "Can't find MPVplayer. <a href='https://mpv.io/installation/'>Click here</a> to "
             "install from official website. If You already installed MPVplayer, please include "
             "the path in environment variable.";
      if (WinDark::isDarkTheme())
        WinDark::setDark_Titlebar(reinterpret_cast<HWND>(alert.winId()));
      alert.setTextFormat(Qt::RichText); // Make links clickable
      alert.setText(text);
      alert.exec();
      exit(0);
      // RETURN
    }
    if ((2 & missing) != 0) // If Streamlink not found
      text = "Streamlink";
    if ((4 & missing) != 0) // If Chatterino not found
      text += (text.isEmpty() ? QString("") : QString(" and ")) + "Chatterino";
    text = "Could not find " + text
           + ". Please select the executable program or folder where programs can be found.";
    alert.setText(text);
    alert.exec();
    openSettings();
  }

  void Settings::openSettings() {
    emit openSettingsSignal(_chatterinoPath, _streamlinkPath, _streamlinkArgs, _streamQuality);
  }

  void Settings::setSettings(QString chatterinoPath,
                             QString streamlinkPath,
                             QString streamlinkArgs,
                             QString streamQuality) {
    _chatterinoPath = chatterinoPath;
    _streamlinkPath = streamlinkPath;
    _streamlinkArgs = streamlinkArgs;
    _streamQuality = streamQuality;
    streamlinkerino::g::localSettings.beginGroup("preferences");
    streamlinkerino::g::localSettings.setValue("chatterino", _chatterinoPath);
    streamlinkerino::g::localSettings.setValue("streamlink", _streamlinkPath);
    streamlinkerino::g::localSettings.setValue("streamlink_args", _streamlinkArgs);
    streamlinkerino::g::localSettings.setValue("streamlink_quality", _streamQuality);
    streamlinkerino::g::localSettings.endGroup();
    if (isInit) {
      isInit = false;
      emit initialized();
    }
  }
} // namespace streamlinkerino

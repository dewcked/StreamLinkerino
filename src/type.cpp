#include "type.h"

#include <QLocalSocket>

#include "debug.h"
#include "path.h"
namespace streamlinkerino {
  namespace g {
    QSettings localSettings(QSettings::NativeFormat, QSettings::UserScope, "streamlinkerino", "streamlinkerino");
  }
  QMap<int, QString> keyBinding = {
      // Increase volume
      {Qt::Key_Plus, "0"},
      {Qt::Key_0, "0"},
      {Qt::Key_Equal, "0"},
      {Qt::Key_ParenRight, "0"},
      {Qt::Key_Slash, "/"},
      {-10, "WHEEL_RIGHT"},
      {Qt::Key_VolumeUp, "VOLUME_UP"},
      // Decrease volume
      {Qt::Key_Minus, "9"},
      {Qt::Key_9, "9"},
      {Qt::Key_Underscore, "9"},
      {Qt::Key_ParenLeft, "9"},
      {Qt::Key_Asterisk, ","},
      {-11, "WHEEL_LEFT"},
      {Qt::Key_VolumeDown, "VOLUME_DOWN"},
      // Mute volume
      {Qt::Key_M, "m"},
      {Qt::Key_VolumeMute, "MUTE"},
      // Pause & replay
      {Qt::Key_Pause, "PAUSE"},
      {Qt::Key_Space, "SPACE"},
      {Qt::Key_Play, "PLAY"},
      {Qt::Key_P, "p"},
      {Qt::Key_MediaTogglePlayPause, "PLAYPAUSE"},
      {Qt::Key_MediaPlay, "PLAYPAUSE"},
      {Qt::Key_MediaPause, "PLAYPAUSE"},
      {Qt::Key_BracketLeft, "["},  // Decrease current playback speed by 10%
      {Qt::Key_BracketRight, "]"}, // Increase current playback speed by 10%
      {Qt::Key_BraceLeft, "{"},    // Halve current playback speed
      {Qt::Key_BraceRight, "}"},   // double current playback speed
      {Qt::Key_Backspace, "BS"},   // Reset playback speed to normal
      {Qt::Key_Period, "."},       // Stop stream temporary and Step forward
      {Qt::Key_S, "s"},            // Take a screenshot
      {Qt::Key_D, "d"},            // Testing - Activate/deactivate deinterlacer.
      {Qt::Key_1, "1"},            // Adjust contrast
      {Qt::Key_2, "2"},            // Adjust contrast
      {Qt::Key_3, "3"},            // Adjust brightness
      {Qt::Key_4, "4"},            // Adjust brightness
      {Qt::Key_5, "5"},            // Adjust gamma
      {Qt::Key_6, "6"},            // Adjust gamma
      {Qt::Key_7, "7"},            // Adjust saturation
      {Qt::Key_8, "8"},            // Adjust saturation
      {-20, "Alt++"},              // Zoom in
      {-21, "Alt+-"},              // Zoom out
      {-22, "Alt+BS"},             // Zoom default
      {-23, "Alt+LEFT"},           // Move left
      {-24, "Alt+RIGHT"},          // Move right
      {-25, "Alt+UP"},             // Move up
      {-26, "Alt+DOWN"},           // Move down
      {-30, "ctrl++"},             // change audio/video sync by delaying the audio
      {-31, "ctrl+-"},             // change audio/video sync by shifting the audio earlier

  };

  QLocalSocketContainer::QLocalSocketContainer() {
    socket = new QLocalSocket(this);
    connect(socket, &QLocalSocket::connected, this, [=]() {
      qDebugSocket() << "[QLocalSocketContainer::connected] socketDescriptor: "
                     << socket->socketDescriptor();
    });
    //    connect(socket, &QLocalSocket::readyRead, this, &QLocalSocketContainer::readyRead);
    connect(socket, &QLocalSocket::disconnected, this, [=]() {
      qDebugSocket() << "[QLocalSocketContainer::Disconnected]";
    });
  };
  void QLocalSocketContainer::inputCommand(int key, const QString& command) {
    // process key input;
    if (streamlinkerino::keyBinding.contains(key))
      socket->write(QString(command + keyBinding[key] + "\n").toUtf8());
    ;
  }

  void QLocalSocketContainer::connectToHost(int pipeNumber) {
    socket->abort();
    qDebugSocket() << "Connecting to" + streamlinkerino::path::NAMED_PIPE
                          + QString::number(pipeNumber);
    QTimer* connectTimer = new QTimer;
    connect(connectTimer, &QTimer::timeout, this, [=]() {
      if (socket->state() == QLocalSocket::UnconnectedState)
        socket->connectToServer(streamlinkerino::path::NAMED_PIPE + QString::number(pipeNumber));
      else
        connectTimer->deleteLater();
    });
    connectTimer->start(100);
  }
} // namespace streamlinkerino

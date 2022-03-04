#pragma once
#include <QLocalSocket>
#include <QProcess>
#include <QSettings>
#include <QTimer>
#include <QWindow>

namespace streamlinkerino {
  namespace g {
    extern QSettings localSettings;
  }
  class QLocalSocketContainer : public QObject {
    Q_OBJECT

   public:
    explicit QLocalSocketContainer();
    void connectToHost(int pipeNumber);
    void inputCommand(int key, const QString& command);

   private:
    QLocalSocket* socket;

    void readyRead();
    void reset();
  };
  typedef struct ChatContainer {
    QProcess* process;
    QWidget* widget;
    QWindow* window;
    // TODO: replace monitor to connect() event trigger
    QTimer* sizer;
    bool lock = false;
    bool embedded = false;
    // int _chatterinoWidth;
    // int _chatterinoHeight;
    void quit() {
      sizer->stop();

      streamlinkerino::g::localSettings.beginGroup("chatterino");
      streamlinkerino::g::localSettings.setValue("embeddedsize_x", window->width());
      streamlinkerino::g::localSettings.endGroup();
      window->setParent(nullptr);
      process->terminate();
      embedded = false;
    }
    void exit() {
      sizer->stop();
      window->setParent(nullptr);
      streamlinkerino::g::localSettings.beginGroup("chatterino");
      quint32 defaultPos_x = streamlinkerino::g::localSettings.value("pos_x", 0).toUInt();
      quint32 defaultPos_y = streamlinkerino::g::localSettings.value("pos_y", 0).toUInt();
      quint32 defaultSize_x = streamlinkerino::g::localSettings.value("size_x", 0).toUInt();
      quint32 defaultSize_y = streamlinkerino::g::localSettings.value("size_y", 0).toUInt();
      streamlinkerino::g::localSettings.setValue("embeddedsize_x", window->width());
      streamlinkerino::g::localSettings.endGroup();
      window->setPosition(QPoint(defaultPos_x, defaultPos_y));
      window->setWidth(defaultSize_x);
      window->setHeight(defaultSize_y);
      process->terminate();
      embedded = false;
    }
    bool isRunning() { return process->state() != QProcess::NotRunning; }
    bool isEmbedded() { return embedded; }
    void setEmbedded(bool param) { embedded = param; }
  } C_Chat;
  // MPV & Streamlink
  // TODO: Store stream?.... ffmpeg?
  // TODO2: concurrent stream?...
  typedef struct MPVContainer : public QObject {
    QWidget* widget;
    QWindow* window;
    QLocalSocketContainer* socket;
    bool embedded = false;
    bool isEmbedded() { return embedded; }
    void setEmbedded(bool param) { embedded = param; }
  } C_MPV;
  typedef struct StreamContainer : public QObject {
    MPVContainer* MPV;
    QProcess* streamlinkProcess;
    QString channel;
    void quit() {
      // MPV->process->kill();
      streamlinkProcess->kill();
    }
    bool isRunning() { return streamlinkProcess->state() != QProcess::NotRunning; }
  } C_Stream;

} // namespace streamlinkerino

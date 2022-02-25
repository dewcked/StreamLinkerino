#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFile>
#include <QMainWindow>
#include <QOperatingSystemVersion>
#include <QSettings>
#include <QWidget>

#include "debugging.h"
#include "submodules.h"
#include "windark.h"
#include "windows_interface.h"
#include <QDebug>
#include <QFileSystemWatcher>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>
#include <QProcess>
#include <QResizeEvent>
#include <QStyleFactory>
#include <QTimer>
#include <QWindow>
#include <thread>

QT_BEGIN_NAMESPACE
namespace Ui
{
  class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

signals:
  void loadChannel();
  void refreshChannel();

private slots:
  void setupUI();
  void setupCustomContextMenu();
  // Context menu
  void showContextMenu(const QPoint &pos);
  // Main logic
  void setupChatterino();
  void setupStream();
  void setupMPV();
  void initialize();
  void refreshStream();
  void loadStream();
  void experimental();
  void resizeEmbeds();
  QString loadCurrentChannel();

  //  static void controlVolume(WId handle, bool isPositive, int quantity);

protected:
  void resizeEvent(QResizeEvent *event) override;
  bool eventFilter(QObject *obj, QEvent *event) override;
  void closeEvent(QCloseEvent *event) override;

private:
  Ui::MainWindow *ui;
  // Main logic

  QString generateStatusHTML(bool bPrerollAds = false);
  void quitStream(qint32 index);

  WindowsInterface _WMP;
  Submodules::SubmodulesDialog *_Submodules;
  QMenu *_contextMenu;
  QList<QAction *> _contextMenuActions;
  bool _bIsMenuOff = true;
  // Chatterino
  typedef struct ChatContainer : public QObject
  {
    QProcess *process;
    QWidget *widget;
    QWindow *window;
    // TODO: replace monitor to connect() event trigger
    QTimer *sizer;
    bool lock = false;
    bool embedded = false;
    // int _chatterinoWidth;
    // int _chatterinoHeight;
    void quit()
    {
      sizer->stop();
      QSettings settings = QSettings(QSettings::NativeFormat, QSettings::UserScope, "streamlinkerino", "streamlinkerino"); // Registry setting
      settings.beginGroup("chatterino");
      settings.setValue("embeddedsize_x", window->width());
      settings.endGroup();
      window->setParent(nullptr);
      process->terminate();
      embedded = false;
    }
    void exit()
    {
      sizer->stop();
      window->setParent(nullptr);
      QSettings settings = QSettings(QSettings::NativeFormat, QSettings::UserScope, "streamlinkerino", "streamlinkerino"); // Registry setting
      settings.beginGroup("chatterino");
      quint32 defaultPos_x = settings.value("pos_x", 0).toUInt();
      quint32 defaultPos_y = settings.value("pos_y", 0).toUInt();
      quint32 defaultSize_x = settings.value("size_x", 0).toUInt();
      quint32 defaultSize_y = settings.value("size_y", 0).toUInt();
      settings.setValue("embeddedsize_x", window->width());
      settings.endGroup();
      window->setPosition(QPoint(defaultPos_x, defaultPos_y));
      window->setWidth(defaultSize_x);
      window->setHeight(defaultSize_y);
      process->terminate();
      embedded = false;
    }
    bool isRunning()
    {
      return process->state() != QProcess::NotRunning;
    }
    bool isEmbedded()
    {
      return embedded;
    }
    void setEmbedded(bool param)
    {
      embedded = param;
    }
  } ChatContainer;

  ChatContainer *_ChatContainer;

  // MPV & Streamlink
  // TODO: Store stream?.... ffmpeg?
  // TODO2: concurrent stream?...
  typedef struct MPVContainer : public QObject
  {
    QWidget *widget;
    QWindow *window;
    bool embedded = false;
    bool isEmbedded()
    {
      return embedded;
    }
    void setEmbedded(bool param)
    {
      embedded = param;
    }
  } MPVContainer;
  typedef struct StreamContainer : public QObject
  {
    MPVContainer *MPV;
    QProcess *streamlinkProcess;
    QString channel;
    void quit()
    {
      // MPV->process->kill();
      streamlinkProcess->kill();
    }
    bool isRunning()
    {
      return streamlinkProcess->state() != QProcess::NotRunning;
    }
  } StreamContainer;
  QList<StreamContainer *> _StreamContainer;
  MPVContainer *_MPVContainer;
  qsizetype _bStreamSelector = 1;
  qsizetype _bStreamLimit = 2;
  bool _bStreamConcurrent = false; // TODO: switch to already opened channel with volume replacement
  bool _bStreamLock = true;
  bool _bDoubleClickedChannel = false;
  // Debug
  bool _bDebug = true;
  int _chatterinoWidth;
  int _chatterinoHeight;
};
#endif // MAINWINDOW_H

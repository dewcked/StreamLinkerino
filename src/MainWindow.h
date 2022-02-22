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
// test
#include <cstdio>
#include <tlhelp32.h>
#include <windows.h>

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

private slots:
  void setupUI();
  void setupCustomContextMenu();
  // Context menu
  void showContextMenu(const QPoint &pos);
  // Main logic
  void setupChatterino();
  void setupStreamlink();
  void setupMVP();
  QString loadCurrentChannel();
  void initialize();
  void refreshStream();
  void loadStream();
  void experimental();
  //  static void controlVolume(WId handle, bool isPositive, int quantity);

protected:
  void resizeEvent(QResizeEvent *event) override;
  bool eventFilter(QObject *obj, QEvent *event);
  void closeEvent(QCloseEvent *event);

private:
  Ui::MainWindow *ui;
  // Main logic
  void resizeEmbeds();
  QString generateStatusHTML(bool bPrerollAds = false);
  void quitChatterino();
  void quitChatterino_();
  void initChatterinoFocus();
  void simulateMouse(unsigned int pos, INPUT *inputs);

  WindowsInterface _WMP;
  Submodules::SubmodulesDialog *_Submodules;
  QWidget *_chatContainer;
  QWidget *_mpvContainer;
  QMenu *_contextMenu;
  QList<QAction *> _contextMenuActions;
  QList<QProcess *> _pStreamlinkProcess;
  QProcess *_pChatterinoProcess;
  QWindow *_wChatterinoWindow;
  QTimer *monitorChatterinoWidth = new QTimer;
  QTimer *monitorChatterinoHeight = new QTimer;
  int _chatterinoWidth;
  int _chatterinoHeight;
  bool _lChatterinoLock = false;
  bool _bChatterinoEmbedded = false;
  QString _cChatChannel;
  unsigned long _mpvContainerWID;
  bool _bStreamlinkProcessSelector = false;
  bool _bStreamlinkAllowSwitching = false;
  bool _bDebug = true;
  bool _bLeftMouseButtonPressed = false;
  QPoint _dragMousePos;
  QPoint _dragMainWindowPos;
  bool _bIsMenuOff = true;
};
#endif // MAINWINDOW_H

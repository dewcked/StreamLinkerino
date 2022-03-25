#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#pragma once
#include <QMainWindow>

#include "chatterino/chatterinojson.h"
#include "native.h"
#include "settings.h"
#include "settingsdialog.h"
#include "type.h"

QT_BEGIN_NAMESPACE
namespace Ui {
  class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

 signals:
  void loadChannel();
  void refreshChannel();

 private slots:
  void setupUI();
  void setupCustomContextMenu();
  // Context menu
  void showContextMenu(const QPoint& pos);
  // Main logic
  void setupChatterino();
  void setupStream();
  void setupMPV();
  void initialize();
  void refreshStream();
  void loadStream();
  void experimental();
  void resizeEmbeds();

  //  static void controlVolume(WId handle, bool isPositive, int quantity);

 protected:
  void resizeEvent(QResizeEvent* event) override;
  bool eventFilter(QObject* obj, QEvent* event) override;
  void closeEvent(QCloseEvent* event) override;

 private:
  Ui::MainWindow* ui;
  // Main logic

  QString generateStatusHTML(bool bPrerollAds = false);
  void quitStream(qint32 index);

  streamlinkerino::WindowInterface _WMP;
  streamlinkerino::Settings* _settings;
  SettingsDialog* _settingsDialog;
  QMenu* _contextMenu;
  QList<QAction*> _contextMenuActions;
  bool _bIsMenuOff = true;
  // Chatterino

  streamlinkerino::C_Chat* _ChatContainer;

  // MPV & Streamlink
  // TODO: Store stream?.... ffmpeg?
  // TODO2: concurrent stream?...

  streamlinkerino::ChatterinoJSON* _ChatterinoJSON;
  QList<streamlinkerino::C_Stream*> _StreamContainer;
  streamlinkerino::C_MPV* _MPVContainer;
  qsizetype _bStreamSelector = 1;
  qsizetype _bStreamLimit = 2;
  bool _bStreamConcurrent = false; // TODO: switch to already opened channel with volume replacement
  bool _bStreamLock = true;
  bool _bDoubleClickedChannel = false;
  QSet<int> _pressedKeys;
  int _chatterinoWidth;
  int _chatterinoHeight;
};
#endif // MAINWINDOW_H

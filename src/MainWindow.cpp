#include "mainwindow.h"

#include "darkmode.h"
#include "debug.h"
#include "ui_mainwindow.h"

// Constructor
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
  // Setup UI
  ui->setupUi(this);
  MainWindow::setupUI();

  // Setup Modules
  _settings = new streamlinkerino::Settings;
  _settingsDialog = new SettingsDialog;
  connect(_settings,
          &streamlinkerino::Settings::openSettingsSignal,
          _settingsDialog,
          &SettingsDialog::showDialog);
  connect(_settingsDialog,
          &SettingsDialog::settingFinished,
          _settings,
          &streamlinkerino::Settings::setSettings);
  connect(_settings, &streamlinkerino::Settings::initialized, this, &MainWindow::initialize);
  connect(_settingsDialog, &SettingsDialog::refreshStream, this, &MainWindow::refreshStream);

  // Setup right-click context menu
  MainWindow::setupCustomContextMenu();

  // Init settings
  _settings->initialize();

  // Setup Event handler (needed for MainWindow::eventFilter)
  qApp->installEventFilter(this);
}

// Deconstructor
MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::setupUI() {
  // Setup dark title bar
  if (WinDark::isDarkTheme())
    WinDark::setDark_Titlebar(reinterpret_cast<HWND>(winId()));

  // Setup Icon
  this->setWindowIcon(QIcon(":resource/Streamlinkerinoicon.ico"));

  // Hide menubar & statusbar
  this->setWindowTitle(QString("Streamlinkerino"));
  ui->menubar->hide();
  ui->statusbar->hide();

  // Load screen settings
  streamlinkerino::g::localSettings.beginGroup("screen");
  QSizeF screenSize = QGuiApplication::primaryScreen()->virtualSize();
  quint32 screenPos_x = streamlinkerino::g::localSettings.value("screenpos_x", 0).toUInt();
  quint32 screenPos_y = streamlinkerino::g::localSettings.value("screenpos_y", 0).toUInt();
  quint32 screenSize_x = streamlinkerino::g::localSettings.value("screensize_x", 0).toUInt();
  quint32 screenSize_y = streamlinkerino::g::localSettings.value("screensize_y", 0).toUInt();
  streamlinkerino::g::localSettings.endGroup();

  // Sanitize screen settings
  if (screenPos_x > 0 && screenPos_y > 0)
    this->move(screenPos_x, screenPos_y);
  if (screenSize_x >= 500 && screenSize_x <= screenSize.width()
      && screenSize_y <= screenSize.height())
    this->resize(screenSize_x, screenSize_y);
  else
    this->resize(screenSize.width() * 0.7, screenSize.height() * 0.7);
}

// Setup custom context menu
void MainWindow::setupCustomContextMenu() {
  qDebug() << "MainWindow::setupCustomContextMenu()";
  // Generate Menu
  _contextMenuActions << new QAction("Settings", this) << new QAction("Load Stream", this)
                      << new QAction("Quit", this) << new QAction("Experimental", this);
  connect(_contextMenuActions[0], SIGNAL(triggered()), _settings, SLOT(openSettings()));
  connect(_contextMenuActions[1], SIGNAL(triggered()), this, SLOT(loadStream()));
  connect(_contextMenuActions[2], SIGNAL(triggered()), this, SLOT(close()));
  connect(_contextMenuActions[3], SIGNAL(triggered()), this, SLOT(experimental()));
  _contextMenu = new QMenu();
  _contextMenu->addActions(_contextMenuActions);
}

// Close event occurred when terminating Streamlinkerino
void MainWindow::closeEvent(QCloseEvent* event) {
  QSettings settings = QSettings(
      QSettings::NativeFormat, QSettings::UserScope, "streamlinkerino", "streamlinkerino");
  settings.beginGroup("screen");
  settings.setValue("screenpos_x", this->pos().x());
  settings.setValue("screenpos_y", this->pos().y());
  settings.setValue("screensize_x", this->width());
  settings.setValue("screensize_y", this->height());
  settings.endGroup();
  foreach (streamlinkerino::C_Stream* streamContainer, _StreamContainer)
    streamContainer->quit();
  _ChatContainer->exit();
  _ChatContainer->process->waitForFinished();
  event->accept();
}

// Show context menu
void MainWindow::showContextMenu(const QPoint& pos) {
  qDebugSlot() << "MainWindow::showContextMenu(const QPoint &pos)";
  // Temporary fix for bug: Context menu popups twice for first attempt
  static int cnt = 0;
  if (cnt++ == 1)
    return;
  else {
    _bIsMenuOff = false;
    _contextMenu->exec(pos);
    _bIsMenuOff = true;
  }
}

// Handle every input events
// Toggle Frameless Window & Fullscreen
bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
  if (event != nullptr && obj->isWindowType()) {
    switch (event->type()) {
      case QEvent::KeyPress:
        if (!this->isActiveWindow())
          break;
        _pressedKeys.insert(static_cast<QKeyEvent*>(event)->key());
        switch (static_cast<QKeyEvent*>(event)->key()) { // TODO: find possible bugs
          case Qt::Key_W:
            if (_pressedKeys.contains(Qt::Key_Control))
              MainWindow::close();
            else {
              if (this->windowFlags().testFlag(Qt::FramelessWindowHint))
                this->setWindowFlag(Qt::FramelessWindowHint, false);
              else if (!this->windowFlags().testFlag(Qt::FramelessWindowHint))
                this->setWindowFlag(Qt::FramelessWindowHint, true);
              this->activateWindow();
              show();
            }
            break;
          case Qt::Key_F:
            if (this->windowState() == Qt::WindowFullScreen)
              this->setWindowState(Qt::WindowNoState);
            else
              this->setWindowState(Qt::WindowFullScreen);
            break;
          case Qt::Key_Escape:
            if (this->windowState() == Qt::WindowFullScreen)
              this->setWindowState(Qt::WindowNoState);
            break;
          case Qt::Key_T:
            if (this->windowFlags().testFlag(Qt::WindowStaysOnTopHint)) {
#ifndef Q_OS_WIN
              this->setWindowFlag(Qt::X11BypassWindowManagerHint, false);
#endif
              this->setWindowFlag(Qt::WindowStaysOnTopHint, false);
            } else {
#ifndef Q_OS_WIN
              this->setWindowFlag(Qt::X11BypassWindowManagerHint, true);
#endif
              this->setWindowFlag(Qt::WindowStaysOnTopHint, true);
            }
            show();
            break;
          case Qt::Key_Plus:
            [[fallthrough]];
          case Qt::Key_Equal:
            if (_pressedKeys.contains(Qt::Key_Alt))
              _MPVContainer->socket->inputCommand(-20, "keypress ");
            else if (_pressedKeys.contains(Qt::Key_Control))
              _MPVContainer->socket->inputCommand(-30, "keydown ");
            else
              _MPVContainer->socket->inputCommand(Qt::Key_Plus, "keydown ");
            break;
          case Qt::Key_Minus:
            [[fallthrough]];
          case Qt::Key_Underscore:
            if (_pressedKeys.contains(Qt::Key_Alt))
              _MPVContainer->socket->inputCommand(-21, "keypress ");
            else if (_pressedKeys.contains(Qt::Key_Control))
              _MPVContainer->socket->inputCommand(-31, "keydown ");
            else
              _MPVContainer->socket->inputCommand(Qt::Key_Minus, "keydown ");
            break;
          case Qt::Key_Backspace:
            if (_pressedKeys.contains(Qt::Key_Alt))
              _MPVContainer->socket->inputCommand(-22, "keypress ");
            else
              _MPVContainer->socket->inputCommand(Qt::Key_Backspace, "keypress ");
            break;
          case Qt::Key_Left:
            if (_pressedKeys.contains(Qt::Key_Alt))
              _MPVContainer->socket->inputCommand(-23, "keydown ");
            break;
          case Qt::Key_Right:
            if (_pressedKeys.contains(Qt::Key_Alt))
              _MPVContainer->socket->inputCommand(-24, "keydown ");
            break;
          case Qt::Key_Up:
            if (_pressedKeys.contains(Qt::Key_Alt))
              _MPVContainer->socket->inputCommand(-25, "keydown ");
            break;
          case Qt::Key_Down:
            if (_pressedKeys.contains(Qt::Key_Alt))
              _MPVContainer->socket->inputCommand(-26, "keydown ");
            break;
          default:
            _MPVContainer->socket->inputCommand(static_cast<QKeyEvent*>(event)->key(), "keydown ");
            break;
        }
        break;
      case QEvent::KeyRelease:
        _pressedKeys.remove(static_cast<QKeyEvent*>(event)->key());
        switch (static_cast<QKeyEvent*>(event)->key()) {
          case Qt::Key_Alt:
            [[fallthrough]];
          case Qt::Key_Control:
            _pressedKeys.clear();
          case Qt::Key_W:
            [[fallthrough]];
          case Qt::Key_F:
            [[fallthrough]];
          case Qt::Key_Escape:
            break;
          case Qt::Key_Left:
            if (_pressedKeys.contains(Qt::Key_Alt))
              _MPVContainer->socket->inputCommand(-23, "keyup ");
            break;
          case Qt::Key_Right:
            if (_pressedKeys.contains(Qt::Key_Alt))
              _MPVContainer->socket->inputCommand(-24, "keyup ");
            break;
          case Qt::Key_Up:
            if (_pressedKeys.contains(Qt::Key_Alt))
              _MPVContainer->socket->inputCommand(-25, "keyup ");
            break;
          case Qt::Key_Down:
            if (_pressedKeys.contains(Qt::Key_Alt))
              _MPVContainer->socket->inputCommand(-26, "keyup ");
            break;
          default:
            _MPVContainer->socket->inputCommand(static_cast<QKeyEvent*>(event)->key(), "keyup ");
            break;
        }
      case QEvent::MouseButtonPress:
        if (!_bIsMenuOff || !this->isActiveWindow())
          break;
        if (static_cast<QMouseEvent*>(event)->button() == Qt::LeftButton)
          this->windowHandle()->startSystemMove();
        break;
      case QEvent::MouseButtonRelease:
        if (static_cast<QMouseEvent*>(event)->button() == Qt::LeftButton) {
          QPoint calc = mapToGlobal(ui->widget->pos())
                        - static_cast<QMouseEvent*>(event)->globalPosition().toPoint();
          if (calc.x() < 0 && calc.y() < 0 && _ChatContainer->isEmbedded()) {
            _ChatContainer->setEmbedded(false);
            _bDoubleClickedChannel = true;
            loadStream();
          }
        } else if (static_cast<QMouseEvent*>(event)->button() == Qt::RightButton)
          showContextMenu(static_cast<QMouseEvent*>(event)->globalPosition().toPoint());
        break;
      case QEvent::MouseButtonDblClick:
        if (!this->isActiveWindow())
          break;
        if (this->windowState() == Qt::WindowFullScreen)
          this->setWindowState(Qt::WindowNoState);
        else
          this->setWindowState(Qt::WindowFullScreen);
        break;
      default:
        break;
      case QEvent::Wheel:
        if (static_cast<QWheelEvent*>(event)->angleDelta().y() > 0
            || static_cast<QWheelEvent*>(event)->angleDelta().x() > 0)
          _MPVContainer->socket->inputCommand(-10, "keypress ");
        else
          _MPVContainer->socket->inputCommand(-11, "keypress ");

        break;
    }
  }
  return QObject::eventFilter(obj, event);
}

// Setup Chatterino process & widget & window
void MainWindow::setupChatterino() {
  qDebugSlot() << "MainWindow::setupChatterino()";
  // If already embedded
  if (_bDoubleClickedChannel) {
    _ChatContainer->quit();
    _ChatContainer->process->terminate();
  } else if (_ChatContainer->embedded) {
    _ChatContainer->quit();
  }
  _ChatContainer->process->setProgram(_settings->chatterinoPath());
  // If process not running, start normal process of setup
  QTimer* chatTerminated = new QTimer;
  connect(chatTerminated, &QTimer::timeout, this, [=]() {
    if (_ChatContainer->process->state() == QProcess::NotRunning) {
      chatTerminated->deleteLater();
      emit loadChannel();
      QPointer<QTimer> findChatterino(new QTimer);
      _ChatContainer->process->start();
      _ChatContainer->process->waitForStarted();
      connect(findChatterino, &QTimer::timeout, this, [=]() {
        WId wid = WId(_WMP.getWID(_ChatContainer->process->processId()));
        if (wid != 0) {
          QSettings settings = QSettings(
              QSettings::NativeFormat, QSettings::UserScope, "streamlinkerino", "streamlinkerino");
          settings.beginGroup("chatterino");

          findChatterino->deleteLater();
          _ChatContainer->window = QWindow::fromWinId(wid);
          settings.setValue("pos_x", _ChatContainer->window->position().x());
          settings.setValue("pos_y", _ChatContainer->window->position().y());
          settings.setValue("size_x", _ChatContainer->window->width());
          settings.setValue("size_y", _ChatContainer->window->height());
          _ChatContainer->widget = createWindowContainer(_ChatContainer->window);
          _ChatContainer->widget->setParent(ui->widget);
          _ChatContainer->widget->setSizePolicy(QSizePolicy::Policy::Expanding,
                                                QSizePolicy::Policy::Expanding);

          // ui->widget->resize(settings.value("embeddedsize_x",
          // 600).toUInt(), _ChatContainer->widget->height());
          int width = settings.value("embeddedsize_x", 350).toUInt();
          width = width <= 100 ? 350 : width;
          ui->widget->setMaximumWidth(width);
          ui->widget->setMinimumWidth(width);
          // _ChatContainer->widget->resize(settings.value("embeddedsize_x",
          // 200).toUInt(), _ChatContainer->widget->height());
          settings.endGroup();
          _ChatContainer->widget->show();
          this->activateWindow();
          _ChatContainer->sizer->start(100);
          _ChatContainer->embedded = true;
          _bDoubleClickedChannel = false;
          resizeEmbeds();
        }
      });
      findChatterino->start(10);
    }
  });
  chatTerminated->start(10);
}

// Init, run and change Streamlink process
// Read streamlink output
// show on QPlainTextEdit debug
// Update Statusbar
// Load MPV when ready
void MainWindow::setupStream() {
  qDebugSlot() << "MainWindow::setupStream()";

  ui->plainTextEdit->show();

  QString s = _StreamContainer.at(_bStreamSelector)->streamlinkProcess->readAll();
  ui->plainTextEdit->setPlainText(ui->plainTextEdit->toPlainText().append(s).append("\n"));
  ui->statusbar->showMessage(s);
  if (s.contains("pre-roll ads")) {
    ui->textEdit_SwitchAlert->setHtml(generateStatusHTML(true));
    resizeEmbeds();
  }
  if (s.contains("player: mpv")) {
    _MPVContainer->widget->setParent(ui->widget_2);
    _MPVContainer->widget->show();
    ui->textEdit_SwitchAlert->hide();
    ui->plainTextEdit->hide();

    // TODO: switch to already opened channel with volume replacement
    qint32 prevStreamIndex = _bStreamSelector == 0 ? _bStreamLimit - 1 : _bStreamSelector - 1;
    if (_StreamContainer.at(prevStreamIndex)->streamlinkProcess->state() != QProcess::NotRunning) {
      QTimer::singleShot(
          600, this, [=]() { _StreamContainer.at(prevStreamIndex)->streamlinkProcess->kill(); });
    }
    resizeEmbeds();
  }
}

// test Init MVP container
void MainWindow::setupMPV() {
  qDebugSlot() << "MainWindow::setupMPV() test";
  _MPVContainer = new streamlinkerino::C_MPV;
  _MPVContainer->socket = new streamlinkerino::QLocalSocketContainer;

  //    connect(localSocket, &QLocalSocket::error, this, &MainWindow::onError);
  //    connect(localSocket, SIGNAL(error(QLocalSocket::LocalSocketError)), this,
  //    SLOT(onError(QLocalSocket::LocalSocketError)));

  _MPVContainer->window = new QWindow;
  _MPVContainer->widget = createWindowContainer(_MPVContainer->window);
  _MPVContainer->widget->setSizePolicy(QSizePolicy::Policy::Expanding,
                                       QSizePolicy::Policy::Expanding);
  _MPVContainer->widget->hide();
  _MPVContainer->setEmbedded(true);
  _bStreamLock = false;
}

// Initialize Chatterino, Streamlink, MVP
// TODO: Implement case for rapid setting changes
void MainWindow::initialize() {
  qDebugSlot() << "MainWindow::initialize()";
  // Init and load channel list
  _ChatterinoJSON = new streamlinkerino::ChatterinoJSON;
  // Init Chatterino
  _ChatContainer = new streamlinkerino::C_Chat;
  _ChatContainer->process = new QProcess(ui->widget);
  _ChatContainer->sizer = new QTimer;
  // Init shared MPV cotainer
  setupMPV();
  // Init Stream (streamlink & mpv)
  for (qint32 i = 0; i < _bStreamLimit; i++)
    _StreamContainer << new streamlinkerino::C_Stream;
  foreach (streamlinkerino::C_Stream* streamContainer, _StreamContainer) {
    // TODO: support concurrent stream changes
    // streamContainer->MPV = new MPVContainer;
    streamContainer->streamlinkProcess = new QProcess(ui->widget_2);
    streamContainer->streamlinkProcess->setProgram(_settings->streamlinkPath());
    connect(
        streamContainer->streamlinkProcess, &QProcess::readyRead, this, &MainWindow::setupStream);
    // streamContainer->MPV->socket = new QLocalSocketContainer(this);
  };
  connect(this, &MainWindow::loadChannel, this, [=]() {
    _ChatterinoJSON->loadChannel();
    QString channel = _ChatterinoJSON->getActiveChannel();
    if (channel == "" || _StreamContainer.at(_bStreamSelector)->channel == channel)
      return;
    else {
      _bStreamSelector = (_bStreamSelector + 1) % _bStreamLimit;
      _StreamContainer.at(_bStreamSelector)->channel = channel;
      // Prompt channel loading message
      ui->textEdit_SwitchAlert->setHtml(generateStatusHTML(false));
      ui->textEdit_SwitchAlert->show();
      _StreamContainer.at(_bStreamSelector)
          ->streamlinkProcess->setArguments(_settings->streamlinkArguments(
              channel, _MPVContainer->window->winId(), int(_bStreamSelector)));
      _StreamContainer.at(_bStreamSelector)->streamlinkProcess->start();
      _MPVContainer->socket->connectToHost(int(_bStreamSelector));
    }
  });
  connect(this, &MainWindow::refreshChannel, this, [=]() {
    QString channel = _ChatterinoJSON->getActiveChannel();
    {
      _bStreamSelector = (_bStreamSelector + 1) % _bStreamLimit;
      _StreamContainer.at(_bStreamSelector)->channel = channel;
      // Prompt channel loading message
      ui->textEdit_SwitchAlert->setHtml(generateStatusHTML(false));
      ui->textEdit_SwitchAlert->show();
      _StreamContainer.at(_bStreamSelector)
          ->streamlinkProcess->setArguments(_settings->streamlinkArguments(
              channel, _MPVContainer->window->winId(), int(_bStreamSelector)));
      _StreamContainer.at(_bStreamSelector)->streamlinkProcess->start();
      _MPVContainer->socket->connectToHost(int(_bStreamSelector));
    }
  });
  connect(_ChatContainer->sizer, &QTimer::timeout, this, [=]() {
    ui->widget->setMaximumWidth(_ChatContainer->window->width());
    ui->widget->setMinimumWidth(_ChatContainer->window->width());
    MainWindow::resizeEmbeds();
  });
  ui->textEdit_SwitchAlert->setHtml(generateStatusHTML(false));
  loadStream();
}

void MainWindow::loadStream() {
  qDebugSlot() << "MainWindow::loadStream()";
  _ChatContainer->lock = true;
  setupChatterino();
  _ChatContainer->lock = false;
}

void MainWindow::experimental() {
  return;
}

void MainWindow::refreshStream() {
  qDebugSlot() << "MainWindow::refreshStream()";
  if (_bStreamLock == true)
    return;
  else
    emit refreshChannel();
}

void MainWindow::resizeEvent(QResizeEvent* event) {
  // qDebugSlot() << "MainWindow::resizeEvent(QResizeEvent *event)";
  if (_ChatContainer->isEmbedded() && !_ChatContainer->lock && !_bStreamLock
      && _MPVContainer->isEmbedded() && _StreamContainer.at(_bStreamSelector)->isRunning())
    resizeEmbeds();
  event->accept();
}

void MainWindow::resizeEmbeds() {
  // qDebug2() << "MainWindow::resizeEmbeds()";
  _ChatContainer->widget->setGeometry(
      0, 0, ui->widget->geometry().width(), ui->widget->geometry().height());
  _MPVContainer->widget->setGeometry(
      0, 0, ui->widget_2->geometry().width(), ui->widget_2->geometry().height());
  // _ChatContainer->window->setPosition(QPoint(0, 0));
}

QString MainWindow::generateStatusHTML(bool bPrerollAds) {
  qDebug2() << "MainWindow::generateStatusHTML(bool bPrerollAds)";
  QString out = "<body style=\" font-family:'Callibri'; font-size:11pt; "
                "font-weight:400; "
                "font-style:normal;\"><p style=\" margin-top:0px; margin-bottom:0px; "
                "margin-left:0px; margin-right:0px; -qt-block-indent:0; "
                "text-indent:0px;\"><span style=\" font-family:'Callibri'; "
                "font-size:11pt; color:#f9f9f9;\">Switching to Channel: </span><span "
                "style=\" font-weight:600; color:#0ef0ce;\">";
  out += _StreamContainer.at(_bStreamSelector)->channel;
  out += "</span>. ";
  if (bPrerollAds)
    out += "<span style=\" font-family:'Callibri'; font-size:11pt; "
           "color:#f9f9f9;\">Waiting for </span><span style=\" "
           "font-weight:600; "
           "color:#00ff00;\">pre-roll ads</span><span style=\" "
           "color:#00ff00;\"> "
           "to finish</span><span style=\" font-family:'Callibri'; "
           "font-size:11pt; color:#f9f9f9;\">!</span></p></body>";
  return out;
}

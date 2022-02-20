#include "mainwindow.h"
#include "ui_mainwindow.h"

// Constructor
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
  // Setup UI
  ui->setupUi(this);
  MainWindow::setupUI();
  // Setup Modules
  _Submodules = new Submodules::SubmodulesDialog;
  connect(_Submodules, &Submodules::SubmodulesDialog::settingFinished, this, &MainWindow::initialize);
  connect(_Submodules, &Submodules::SubmodulesDialog::refreshStream, this, &MainWindow::refreshStream);

  // Setup right-click context menu
  MainWindow::setupCustomContextMenu();

  _Submodules->initialize();

  // Setup Event handler (needed for MainWindow::eventFilter)
  qApp->installEventFilter(this);
}

// Deconstructor
MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::setupUI()
{
  // Setup dark theme
  MainWindow::setupDarktheme();

  // Hide menubar & statusbar
  this->setWindowTitle(QString("Streamlinkerino"));
  ui->menubar->hide();
  ui->statusbar->hide();

  // Load screen setting from registry.
  QSettings settings = QSettings(QSettings::NativeFormat, QSettings::UserScope, "streamlinkerino", "streamlinkerino"); // Registry setting
  settings.beginGroup("screen");
  QSizeF screenSize = QGuiApplication::primaryScreen()->virtualSize();
  quint32 screenPos_x = settings.value("screenpos_x", 0).toUInt();
  quint32 screenPos_y = settings.value("screenpos_y", 0).toUInt();
  quint32 screenSize_x = settings.value("screensize_x", 0).toUInt();
  quint32 screenSize_y = settings.value("screensize_y", 0).toUInt();
  settings.endGroup();
  if (screenPos_x > 0 && screenPos_y > 0)
    this->move(screenPos_x, screenPos_y);
  if (screenSize_x >= 500 && screenSize_x <= screenSize.width() && screenSize_y <= screenSize.height())
    this->resize(screenSize_x, screenSize_y);
  else
  {
    this->resize(screenSize.width() * 0.7, screenSize.height() * 0.7);
  }
}

// Enable dark theme if Windows native dark theme is enabled
// TODO: Change theme color
// TODO: Add setting for choosing dark theme
void MainWindow::setupDarktheme()
{
  qDebug() << "MainWindow::setupDarktheme()";
  if (isWindowsDarkthemeAvailable() && isWindowsDarktheme())
  {
    // QWindowsWindow::setDarkBorderToWindow();
    QFile f(":./qdarkstyle/dark/style.qss");
    if (f.open(QFile::ReadOnly | QFile::Text))
    {
      QTextStream ts(&f);
      setStyleSheet(ts.readAll());
    }
  }
}
// Check if current Windows version supports dark theme
bool MainWindow::isWindowsDarkthemeAvailable()
{
  qDebug2() << "MainWindow::setupDarktheme()";
  // dark mode supported Windows 10 1809 10.0.17763 onward
  // https://stackoverflow.com/questions/53501268/win10-dark-theme-how-to-use-in-winapi
  if (QOperatingSystemVersion::current().majorVersion() == 10)
    return QOperatingSystemVersion::current().microVersion() >= 17763;
  else if (QOperatingSystemVersion::current().majorVersion() > 10)
    return true;
  else
    return false;
}

// Check if dark theme is enabled
bool MainWindow::isWindowsDarktheme()
{
  qDebug2() << "MainWindow::isWindowsDarktheme()";
  QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);
  return settings.value("AppsUseLightTheme", 1).toInt() == 0;
}

// Setup custom context menu
void MainWindow::setupCustomContextMenu()
{
  qDebug() << "MainWindow::setupCustomContextMenu()";
  // Generate Menu
  _contextMenuActions << new QAction("Settings", this) << new QAction("Load Stream", this) << new QAction("Quit", this) << new QAction("Experimental", this);
  connect(_contextMenuActions[0], SIGNAL(triggered()), _Submodules, SLOT(showDialog()));
  connect(_contextMenuActions[1], SIGNAL(triggered()), this, SLOT(loadStream()));
  connect(_contextMenuActions[2], SIGNAL(triggered()), this, SLOT(close()));
  _contextMenu.addActions(_contextMenuActions);
}

void MainWindow::quitChatterino()
{
  // Setting part (need to revert)
  qDebug2() << "MainWindow::quitChatterino()";
  _lChatterinoLock = true;
  _wChatterinowindow->hide();
  _wChatterinowindow->setParent(nullptr);
  _pChatterinoProcess->terminate();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  MainWindow::quitChatterino();
  _pStreamlinkProcess.at(0)->kill(); // terminate doesn't work properly on Windows
  _pStreamlinkProcess.at(1)->kill(); // terminate doesn't work properly on Windows
  QTimer *exit = new QTimer();
  connect(exit, &QTimer::timeout, this,
          [=]()
          {
            if (_pChatterinoProcess->state() == QProcess::NotRunning && _pStreamlinkProcess.at(0)->state() == QProcess::NotRunning && _pStreamlinkProcess.at(0)->state() == QProcess::NotRunning)
              event->accept();
          });
  exit->start(10);
}

// Show context menu
void MainWindow::showContextMenu(const QPoint &pos) // this is a slot
{
  qDebugSlot() << "MainWindow::showContextMenu(const QPoint &pos)";
  _contextMenu.exec(pos);
}
// Handle every input events
// Toggle Frameless Window & Fullscreen
// TODO: When toggling FramelessWindowHint, Focus is not working properly - https://doc.qt.io/archives/qt-4.8/qwidget.html#clearFocus
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
  if (event->type() == QEvent::KeyPress && obj->isWindowType())
  {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    switch (keyEvent->key())
    {
    case Qt::Key_W:
      if (this->windowFlags().testFlag(Qt::FramelessWindowHint))
        this->setWindowFlag(Qt::FramelessWindowHint, false);
      else
        this->setWindowFlag(Qt::FramelessWindowHint, true);
      show();
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
    }
    //    qDebug() << "key " << keyEvent->key() << "from" << obj;
  }
  else if (event->type() == QEvent::MouseButtonPress && obj->isWindowType())
  {
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
    if (mouseEvent->button() == Qt::RightButton)
    {
      qDebug() << mouseEvent->globalPosition().toPoint();
      showContextMenu(mouseEvent->globalPosition().toPoint());
    }

    // qDebug() << this->x() + this->width() - 100 << this->y() + 50 << mouseEvent->globalPosition().toPoint();
  }
  else if (event->type() == QEvent::MouseButtonDblClick && obj->isWindowType())
  {
    if (this->windowState() == Qt::WindowFullScreen)
      this->setWindowState(Qt::WindowNoState);
    else
      this->setWindowState(Qt::WindowFullScreen);
  }
  return QObject::eventFilter(obj, event);
}

// Init, run and embed Chatterino process & window
void MainWindow::setupChatterino()
{
  qDebugSlot() << "MainWindow::setupChatterino()";

  if (_lChatterinoLock)
    return;
  _lChatterinoLock = true;
  // If Chatterino isn't running, run
  if (_pChatterinoProcess->state() == QProcess::NotRunning)
  {
    QTimer *findChatterino = new QTimer;
    _pChatterinoProcess->start();
    _pChatterinoProcess->waitForStarted();
    connect(findChatterino, &QTimer::timeout, this,
            [=]()
            {
              WId wid = WId(_WMP.getWID(_pChatterinoProcess->processId()));
              if (wid != 0)
              {
                // Make QTimer monitoring for chat size changes
                connect(monitorChatterinoWidth, &QTimer::timeout, this,
                        [=]()
                        {
                          int tmpChatterinoWidth = _WMP.getWindowWidth(HWND(wid));
                          if (_chatterinoWidth != tmpChatterinoWidth && _lChatterinoLock != true)
                          {
                            _chatterinoWidth = tmpChatterinoWidth;
                            ui->widget->setMinimumWidth(_chatterinoWidth);
                            MainWindow::resizeEmbeds();
                          }
                        });
                connect(monitorChatterinoHeight, &QTimer::timeout, this,
                        [=]()
                        {
                          int tmpChatterinoHeight = _WMP.getWindowHeight(HWND(wid));
                          if (_chatterinoHeight != tmpChatterinoHeight && _lChatterinoLock != true)
                            MainWindow::resizeEmbeds();
                        });
                // Setting part
                ui->widget->setMinimumWidth(_WMP.getWindowWidth(HWND(wid)));
                qDebug_() << ui->widget->minimumWidth();
                _wChatterinowindow = QWindow::fromWinId(wid);
                findChatterino->deleteLater();
                _chatContainer = createWindowContainer(_wChatterinowindow);

                _chatContainer->setParent(ui->widget); // TODO: Is this needed? possible duplicate of line 98 `_pChatterinoProcess = new QProcess(ui->widget);`
                _chatContainer->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
                _chatContainer->show();
                // connect(window, &QWindow::widthChanged, this, &MainWindow::resizeEmbeds);
                _bChatterinoEmbedded = true;
                _lChatterinoLock = false;
                monitorChatterinoWidth->start(100);
                monitorChatterinoHeight->start(100);
                resizeEmbeds();
              }
            });
    findChatterino->start(10);
  }
}

// Init, run and change Streamlink process
// Read streamlink output
// show on QPlainTextEdit debug
// Update Statusbar
// Load MPV when ready
void MainWindow::setupStreamlink()
{
  qDebugSlot() << "MainWindow::setupStreamlink()";

  if (_bDebug)
  {
    ui->plainTextEdit->show();
  }
  QString s = _pStreamlinkProcess.at(_bStreamlinkProcessSelector)->readAll();
  ui->plainTextEdit->setPlainText(ui->plainTextEdit->toPlainText().append(s).append("\n"));
  // ui->statusbar->showMessage(s);
  if (s.contains("pre-roll ads"))
  {
    ui->textEdit_SwitchAlert->setHtml(generateStatusHTML(true));
    resizeEmbeds();
  }
  if (s.contains("player: mpv"))
  {
    _mpvContainer->setParent(ui->widget_2);
    _mpvContainer->show();
    ui->textEdit_SwitchAlert->hide();
    ui->plainTextEdit->hide();
    _bStreamlinkAllowSwitching = true;

    // Turnoff other streamlink if it's running
    // half second delay for better transition
    if (_pStreamlinkProcess.at(!_bStreamlinkProcessSelector)->state() != QProcess::NotRunning)
    {
      QTimer::singleShot(600, this, [=]()
                         { _pStreamlinkProcess.at(!_bStreamlinkProcessSelector)->terminate(); });
    }
    resizeEmbeds();
  }
}

// Init MVP container
void MainWindow::setupMVP()
{
  qDebugSlot() << "MainWindow::setupMVP()";
  // Setup mpv container and wid //changed some ******************
  QWindow *mpv_window = new QWindow;
  _mpvContainerWID = mpv_window->winId();
  _mpvContainer = createWindowContainer(mpv_window);
  _mpvContainer->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
  _mpvContainer->hide();
}

// Load current channel from json file
QString MainWindow::loadCurrentChannel()
{
  qDebug2() << "MainWindow::loadCurrentChannel()";
  // load json
  QString val;
  QFile file(QDir::homePath() + "/Appdata/Roaming/Chatterino2/Settings/window-layout.json");
  if (file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    val = file.readAll();
    file.close();
    QJsonDocument jdoc = QJsonDocument::fromJson(val.toUtf8());
    QJsonArray channels = jdoc.object()
                              .value(QString("windows"))
                              .toArray()
                              .at(0)
                              .toObject()
                              .value(QString("tabs"))
                              .toArray();
    foreach (QJsonValue channel, channels)
    {
      QString channel_name;
      QJsonObject channel_info = channel["splits2"]["data"].toObject();
      channel_name = channel_info.value(QString("name")).toString();
      // qDebug_() << channel_info.value(QString("type")).toString() << channel_name << channel["selected"];

      // If it is twitch channel
      if (channel_info.value(QString("type")).toString() == QString("twitch") && channel["selected"].toBool() == true)
      {
        qDebug_() << channel_name;
        return channel_name;
      }
    }
  }
  return "";
}

// Initialize Chatterino, Streamlink, MVP
// TODO: Implement case for rapid setting changes
void MainWindow::initialize()
{
  qDebugSlot() << "MainWindow::initialize()";
  // If we are re-initializing
  if (_Submodules->getChanges() > 0)
  {
    qDebug_() << "Reinitialize";
    if (_Submodules->getChanges() & Submodules::ChangeFlags::Chatterino)
    {
      if (_pChatterinoProcess->state() == QProcess::NotRunning)
        setupChatterino();
      else
      {
        _lChatterinoLock = true;
        _bChatterinoEmbedded = false;
        _pChatterinoProcess->terminate();
        QTimer *restart = new QTimer(this);
        connect(restart, &QTimer::timeout, this,
                [=]()
                {
                  if (_pChatterinoProcess->state() == QProcess::NotRunning)
                  {
                    restart->deleteLater();
                    _pChatterinoProcess->setProgram(_Submodules->chatterinoPath());
                    _lChatterinoLock = false;
                    setupChatterino();
                  }
                });
        restart->start(10);
      }
    }
    if (_Submodules->getChanges() & Submodules::ChangeFlags::StreamLink)
    { // Only change program setting and don't touch current running stream
      // To reload streamlink, click `load Stream` in context Menu
      _pStreamlinkProcess[0]->setProgram(_Submodules->streamlinkPath());
      _pStreamlinkProcess[1]->setProgram(_Submodules->streamlinkPath());
    }
  }
  else
  {
    qDebug_() << "Initialize";
    // Init MVP
    setupMVP();

    // Init Chatterino
    _pChatterinoProcess = new QProcess(ui->widget);
    _pChatterinoProcess->setProgram(_Submodules->chatterinoPath());
    setupChatterino();

    // Init Streamlink
    _pStreamlinkProcess << new QProcess(ui->widget_2) << new QProcess(ui->widget_2);
    _pStreamlinkProcess[0]->setProgram(_Submodules->streamlinkPath());
    _pStreamlinkProcess[1]->setProgram(_Submodules->streamlinkPath());
    connect(_pStreamlinkProcess[0], &QProcess::readyRead, this, &MainWindow::setupStreamlink);
    connect(_pStreamlinkProcess[1], &QProcess::readyRead, this, &MainWindow::setupStreamlink);

    QString channel = MainWindow::loadCurrentChannel();
    if (channel != "" && channel != _cChatChannel)
    { // TODO: Check if works correctly
      _cChatChannel = channel;
      _pStreamlinkProcess.at(_bStreamlinkProcessSelector)->setArguments(_Submodules->getStreamLinkArguments(_cChatChannel, _mpvContainerWID));
      _pStreamlinkProcess[_bStreamlinkProcessSelector]->start();
    }
    //    QTimer *initChatterinoFocus = new QTimer;
    //    QTimer *initChatterinoFocus2 = new QTimer;
    //    connect(initChatterinoFocus, &QTimer::timeout, this,
    //            [=]()
    //            {
    //              if (_pChatterinoProcess->state() == QProcess::Running)
    //              {
    //                initChatterinoFocus2->start(10000);
    //                initChatterinoFocus->deleteLater();
    //              }
    //            });
    //    connect(initChatterinoFocus2, &QTimer::timeout, this,
    //            [=]()
    //            {
    //              MainWindow::initChatterinoFocus();
    //              initChatterinoFocus2->deleteLater();
    //            });
    //    initChatterinoFocus->start(10);
  }
}

// NEED FIX
void MainWindow::loadStream()
{
  MainWindow::quitChatterino();
  // Load channel if new channel exists
  QTimer *loadStream = new QTimer(this);
  _lChatterinoLock = true;
  connect(loadStream, &QTimer::timeout, this,
          [=]()
          {
            qDebug() << _pChatterinoProcess->state();
            if (_pChatterinoProcess->state() == QProcess::NotRunning)
            {
              QString channel = loadCurrentChannel();
              loadStream->deleteLater();
              _lChatterinoLock = false;
              setupChatterino();
              if (channel == "" || _cChatChannel == channel)
                return;
              else
              {
                _cChatChannel = channel;
                _bStreamlinkProcessSelector = !_bStreamlinkProcessSelector;
                _pStreamlinkProcess.at(_bStreamlinkProcessSelector)->setArguments(_Submodules->getStreamLinkArguments(_cChatChannel, _mpvContainerWID));
                _pStreamlinkProcess[_bStreamlinkProcessSelector]->start();
              }
            }
          });
  loadStream->start(10);
}

void MainWindow::refreshStream()
{
  qDebugSlot() << "MainWindow::refreshStream()";
  if (_bStreamlinkAllowSwitching)
  {
    _bStreamlinkProcessSelector = !_bStreamlinkProcessSelector;
    _bStreamlinkAllowSwitching = !_bStreamlinkAllowSwitching;
  }
  auto tChannel = _cChatChannel;
  _cChatChannel = "";
  _pStreamlinkProcess.at(_bStreamlinkProcessSelector)->setArguments(_Submodules->getStreamLinkArguments(tChannel, _mpvContainerWID));
  _pStreamlinkProcess.at(_bStreamlinkProcessSelector)->start();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
  // qDebugSlot() << "MainWindow::resizeEvent(QResizeEvent *event)";
  if (_bChatterinoEmbedded == true && _lChatterinoLock == false)
    resizeEmbeds();
  event->accept();
}

void MainWindow::resizeEmbeds()
{
  // qDebug2() << "MainWindow::resizeEmbeds()";
  QTimer::singleShot(10, this,
                     [=]()
                     {
                       _chatContainer->setGeometry(0, 1, ui->widget->geometry().width(), ui->widget->geometry().height() - 1);
                       _mpvContainer->setGeometry(9, 24, ui->widget_2->geometry().width() - 18, ui->widget_2->geometry().height() - 24);
                     });
  // Load screen setting from registry.
  QSettings settings = QSettings(QSettings::NativeFormat, QSettings::UserScope, "streamlinkerino", "streamlinkerino"); // Registry setting
  settings.beginGroup("screen");
  settings.setValue("screenpos_x", this->pos().x());
  settings.setValue("screenpos_y", this->pos().y());
  settings.setValue("screensize_x", this->width());
  settings.setValue("screensize_y", this->height());
  settings.endGroup();
}

QString MainWindow::generateStatusHTML(bool bPrerollAds)
{
  qDebug2() << "MainWindow::generateStatusHTML(bool bPrerollAds)";
  QString out = "<body style=\" font-family:'Callibri'; font-size:11pt; font-weight:400; font-style:normal;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Callibri'; font-size:11pt; color:#f9f9f9;\">Switching to Channel: </span><span style=\" font-weight:600; color:#0ef0ce;\">";
  out += _cChatChannel;
  out += "</span>. ";
  if (bPrerollAds)
    out += "<span style=\" font-family:'Callibri'; font-size:11pt; color:#f9f9f9;\">Waiting for </span><span style=\" font-weight:600; color:#00ff00;\">pre-roll ads</span><span style=\" color:#00ff00;\"> to finish</span><span style=\" font-family:'Callibri'; font-size:11pt; color:#f9f9f9;\">!</span></p></body>";
  return out;
}

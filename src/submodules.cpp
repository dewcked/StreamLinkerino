#include "submodules.h"

#include "ui_submodulesDialog.h"
using namespace Submodules;

SubmodulesDialog::SubmodulesDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::SubmodulesDialog), isInit(true)
{
  ui->setupUi(this);
  this->setWindowTitle(QString("Settings"));
  this->hide();
  hideAlerts();
  setupConnections();
}

SubmodulesDialog::~SubmodulesDialog() { delete ui; }

void SubmodulesDialog::showMessageBox()
{
  QMessageBox alert;
  QString text = "Warning! The following external tools have not been found:\n\n";
  if (_streamlinkPath.isEmpty())
    text += "[streamlink]";
  if (_chatterinoPath.isEmpty())
    text += "[chatterino]";
  text += "\n\nPlease select the folder where the missing tools can be found.";
  alert.setText(text);
  alert.exec();

  showDialog();
}

Streamlink::Streamlink(QString quality) : _quality(quality){};

QString SubmodulesDialog::streamlinkPath() { return _streamlinkPath; }

QString SubmodulesDialog::chatterinoPath() { return _chatterinoPath; }

QString SubmodulesDialog::streamlinkArguments() { return _streamlinkArgs; }

QStringList SubmodulesDialog::getStreamLinkArguments(QString channel, unsigned long mpvContainer)
{
  return _SL->getArgs(channel, mpvContainer);
}

void SubmodulesDialog::loadSettings()
{
  qDebug2() << "SubmodulesDialog::loadSettings()";
  // drive, path, executable location
  QFileInfoList drives = QDir::drives();
  QStringList paths = QStringList() << "Program Files (x86)/"
                                    << "Program Files/";
  QFileInfo executable;

  // Load registries
  settings.beginGroup("preferences");
  ui->comboBox_StreamlinkQuality->setCurrentIndex(settings.value("streamlink_quality").toInt());
  _streamlinkPath = settings.value("streamlink", "").toString();
  _streamlinkArgs = settings.value("streamlink_args", "").toString();
  _chatterinoPath = settings.value("chatterino", "").toString();
  _streamlinkQuality = settings.value("streamlink_quality", "best").toString();
  qDebug2_() << "_streamlinkPath: " << _streamlinkPath;
  qDebug2_() << "_streamlinkArgs: " << _streamlinkArgs;
  qDebug2_() << "chatterinoPath: " << _chatterinoPath;
  qDebug2_() << "streamlinkQuality: " << _streamlinkQuality;

  // Set default quality
  if (StreamQuality.find(_streamlinkQuality) == StreamQuality.end())
    _streamlinkQuality = "Best";
  settings.setValue("streamlink_quality", _streamlinkQuality);

  // Check if executables that predefined in registries exist. If not defined or exist, search executables' path
  executable = QFileInfo(_streamlinkPath);
  if (!executable.exists() || !executable.isFile())
  {
    _streamlinkPath = "";
    foreach (QFileInfo drive, drives)
      foreach (QString path, paths)
      {
        executable = QFileInfo(drive.path() + path + "streamlink/bin/streamlink.exe");
        if (executable.exists() && executable.isFile())
        {
          _streamlinkPath = drive.path() + path + "streamlink/bin/streamlink.exe";
          settings.setValue("streamlink", _streamlinkPath);
          break;
        }
      }
  }
  executable = QFileInfo(_chatterinoPath);
  if (!executable.exists() || !executable.isFile())
  {
    _chatterinoPath = "";
    foreach (QFileInfo drive, drives)
      foreach (QString path, paths)
      {
        executable = QFileInfo(drive.path() + path + "Chatterino/chatterino.exe");
        if (executable.exists() && executable.isFile())
        {
          _chatterinoPath = drive.path() + path + "Chatterino/chatterino.exe";
          settings.setValue("chatterino", _chatterinoPath);
          break;
        }
      }
  }
  settings.endGroup();

  qDebug2_() << "(loaded)_streamlinkPath: " << _streamlinkPath;
  qDebug2_() << "(loaded)_streamlinkArgs: " << _streamlinkArgs;
  qDebug2_() << "(loaded)chatterinoPath: " << _chatterinoPath;
  qDebug2_() << "(loaded)streamlinkQuality: " << _streamlinkQuality;
  if (_streamlinkPath.isEmpty() || _chatterinoPath.isEmpty())
    showMessageBox();
}

void SubmodulesDialog::showDialog()
{
  ui->lineEdit_chatterinoPath->setText(_chatterinoPath);
  ui->lineEdit_streamLinkPath->setText(_streamlinkPath);
  ui->lineEdit_streamLinkOptions->setText(_streamlinkArgs);
  ui->comboBox_StreamlinkQuality->setCurrentIndex(StreamQuality[_streamlinkQuality]);
  _changes = 0;
  this->show();
}

void SubmodulesDialog::initialize()
{
  loadSettings();
  _SL = new Streamlink(_streamlinkQuality);
  emit SubmodulesDialog::settingFinished();
}

/* TODO - Make setting possible while streaming */
void SubmodulesDialog::setupConnections()
{
  qDebug() << "SubmodulesDialog::setupConnections()";
  connect(ui->lineEdit_chatterinoPath, &QLineEdit::editingFinished, this,
          [=]()
          {
            QString path = ui->lineEdit_chatterinoPath->text().simplified();
            QString chatterino = "chatterino.exe";
            qDebug() << "(input)path: " << path;

            if (!(path.length() > chatterino.length() && chatterino == path[path.length() - chatterino.length()]))
            {
              if (path[path.length() - 1] != '/')
                path += '/';
              path += chatterino;
            }

            QFileInfo executable(path);
            if (executable.exists() && executable.isFile())
            {
              qDebug() << "Chatterino Found: " << path;
              ui->label_chatterinoAlert->setText("Chatterino Found");
              ui->label_chatterinoAlert->setStyleSheet(_styleGreen);
              ui->label_chatterinoAlert->show();

              // If there's a difference
              if (_chatterinoPath != path)
                _changes |= ChangeFlags::Chatterino;
              _chatterinoPath = path;
              return;
            }

            //        db_f(not found);
            ui->label_chatterinoAlert->setText("Chatterino Not Found");
            ui->label_chatterinoAlert->setStyleSheet(_styleRed);
            ui->label_chatterinoAlert->show();
          });
  connect(ui->lineEdit_streamLinkPath, &QLineEdit::editingFinished, this,
          [=]()
          {
            QString path = ui->lineEdit_streamLinkPath->text().simplified();
            QString streamlink = "streamlink.exe";
            qDebug() << "(input)path: " << path;

            if (!(path.length() > streamlink.length() && streamlink == path[path.length() - streamlink.length()]))
            {
              if (path[path.length() - 1] != '/')
                path += '/';
              path += streamlink;
            }

            QFileInfo executable(path);
            if (executable.exists() && executable.isFile())
            {
              qDebug() << "Streamlink Found: " << path;
              ui->label_streamlinkAlert->setText("Streamlink Found");
              ui->label_streamlinkAlert->setStyleSheet(_styleGreen);
              ui->label_streamlinkAlert->show();

              // If there's a difference
              if (_streamlinkPath != path)
                _changes |= ChangeFlags::StreamLink;
              _streamlinkPath = path;
              return;
            }

            //        db_f(not found);
            ui->label_streamlinkAlert->setText("Streamlink Not Found");
            ui->label_streamlinkAlert->setStyleSheet(_styleRed);
            ui->label_streamlinkAlert->show();
          });
  // Man, this connect is ugly af
  connect(ui->comboBox_StreamlinkQuality,
          static_cast<void (QComboBox::*)(const int)>(
              &QComboBox::currentIndexChanged),
          this,
          [=]()
          {
            // Queue up streamlink reset
            _changes |= ChangeFlags::StreamLink;

            // Save default stream quality
            _streamlinkQuality = ui->comboBox_StreamlinkQuality->currentText();
          });
  // If click 'OK' button
  connect(ui->pushButton, &QPushButton::clicked, this,
          [=]()
          {
            if (ui->label_chatterinoAlert->text() == "Chatterino Found" && ui->label_streamlinkAlert->text() == "Streamlink Found" && StreamQuality.find(ui->comboBox_StreamlinkQuality->currentText()) != StreamQuality.end())
            { // TODO: check if valid streamlink_args
              settings.beginGroup("preferences");
              settings.setValue("chatterino", _chatterinoPath);
              settings.setValue("streamlink", _streamlinkPath);
              settings.setValue("streamlink_args", _streamlinkArgs);
              settings.setValue("streamlink_quality", _streamlinkQuality);
              settings.endGroup();
              this->close(); // Quit dialog
            }
          });
  // If click 'Cancel' button
  connect(ui->pushButton_2, &QPushButton::clicked, this,
          [=]()
          {
            if (isInit)      // If current process is init process
              this->close(); // Quit dialog
            else
              QApplication::quit(); // Terminate program
          });
  // If click 'Refresh current stream' button
  connect(ui->pushButton_3, &QPushButton::clicked, this, &SubmodulesDialog::refreshStream);
}

void SubmodulesDialog::hideAlerts()
{
  ui->label_chatterinoAlert->hide();
  ui->label_streamlinkAlert->hide();
}

int Submodules::SubmodulesDialog::getChanges() { return _changes; } //?

QStringList Streamlink::getArgs(QString channel, unsigned long mpvContainer) //
{
  QStringList args;
  args << "--twitch-low-latency";
  args << "--twitch-disable-ads";
  args << "--player";
  args << "mpv --wid=" + QString::number(mpvContainer);
  args << "https://www.twitch.tv/" + channel;
  args << _quality;
  return args;
}

void Streamlink::setQuality(QString quality)
{
  if (StreamQuality.find(quality) == StreamQuality.end())
  {
    db "setQuality OOR";
    return;
  }
  _quality = quality;
}

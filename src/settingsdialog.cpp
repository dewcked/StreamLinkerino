#include "settingsdialog.h"

#include <QCompleter>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include "debug.h"

#include "darkmode.h"
#include "path.h"

namespace streamlinkerino {
  namespace style {
    QString _styleGreen = "color: rgb(44, 145, 7);";
    QString _styleRed = "color: rgb(156, 12, 21);";
  } // namespace style
  QMap<QString, qint32> StreamQuality = {{"Best", 0},
                                         {"1080p60", 1},
                                         {"720p60", 2},
                                         {"720p", 3},
                                         {"480p", 4},
                                         {"360p", 5},
                                         {"160p", 6},
                                         {"Worst", 7},
                                         {"Audio only", 8}};
} // namespace streamlinkerino

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
    , isInit(true) {
  ui->setupUi(this);
  this->hide();
  this->setWindowTitle(QString("Settings"));
  this->setWindowIcon(QIcon(":resource/Streamlinkerinoicon.ico"));
  ui->pushButton->setDisabled(true); // Disable OK Button as default
  ui->label_chatterinoAlert->hide(); // Disable alert highlits
  ui->label_streamlinkAlert->hide(); // Disable alert highlits
  setupConnections();
}
SettingsDialog::~SettingsDialog() {
  delete ui;
}

/* TODO - Make setting possible while streaming */
void SettingsDialog::setupConnections() {
  qDebug() << "SettingsDialog::setupConnections()";
  connect(ui->toolButton_streamlink, &QToolButton::clicked, this, [=]() {
    QString path = QFileDialog::getOpenFileName(this, tr("Open Streamlink"), QDir::homePath());
    if (!path.isEmpty())
      ui->lineEdit_streamLinkPath->setText(path);
  });
  connect(ui->toolButton_chatterino, &QToolButton::clicked, this, [=]() {
    QString path = QFileDialog::getOpenFileName(this, tr("Open Chatterino"), QDir::homePath());
    if (!path.isEmpty())
      ui->lineEdit_chatterinoPath->setText(path);
  });
  // connect(ui->toolButton_chatterino, &QToolButton::triggered, this, [=]() { ; });
  connect(this, &SettingsDialog::exeFound, this, [=]() {
    if (ui->label_chatterinoAlert->text() == streamlinkerino::path::CHATTERINO_EXE + " Found"
        && ui->label_streamlinkAlert->text() == streamlinkerino::path::STREAMLINK_EXE + " Found")
      ui->pushButton->setEnabled(true);
  });
  connect(ui->lineEdit_chatterinoPath, &QLineEdit::textChanged, this, [=]() {
    SettingsDialog::validatePath(ui->lineEdit_chatterinoPath,
                                 ui->label_chatterinoAlert,
                                 streamlinkerino::path::CHATTERINO_EXE,
                                 streamlinkerino::path::CHATTERINO_EXE_PATH);
  });
  connect(ui->lineEdit_streamLinkPath, &QLineEdit::textChanged, this, [=]() {
    SettingsDialog::validatePath(ui->lineEdit_streamLinkPath,
                                 ui->label_streamlinkAlert,
                                 streamlinkerino::path::STREAMLINK_EXE,
                                 streamlinkerino::path::STREAMLINK_EXE_PATH);
  });
  // If clicked 'OK' button
  connect(ui->pushButton, &QPushButton::clicked, this, [=]() {
    if (ui->label_chatterinoAlert->text() == streamlinkerino::path::CHATTERINO_EXE + " Found"
        && ui->label_streamlinkAlert->text() == streamlinkerino::path::STREAMLINK_EXE + " Found"
        && streamlinkerino::StreamQuality.find(ui->comboBox_StreamlinkQuality->currentText())
               != streamlinkerino::StreamQuality.end()) { // TODO: check if valid streamlink_args
      emit settingFinished(ui->lineEdit_chatterinoPath->text(),
                           ui->lineEdit_streamLinkPath->text(),
                           ui->lineEdit_streamLinkOptions->text(),
                           ui->comboBox_StreamlinkQuality->currentText());
      isInit = false;
      this->close(); // Quit dialog
    }
  });
  // If clicked 'Cancel' button
  connect(ui->pushButton_2, &QPushButton::clicked, this, [=]() {
    if (isInit)      // If current process is init process
      this->close(); // Quit dialog
    else
      exit(0); // Terminate program
  });
  // If click 'Refresh current stream' button
  connect(ui->pushButton_3, &QPushButton::clicked, this, &SettingsDialog::refreshStream);
}

void SettingsDialog::showDialog(QString chatterinoPath,
                                QString streamlinkPath,
                                QString streamlinkArgs,
                                QString streamQuality) {
  ui->lineEdit_chatterinoPath->setText(chatterinoPath);
  ui->lineEdit_streamLinkPath->setText(streamlinkPath);
  ui->lineEdit_streamLinkOptions->setText(streamlinkArgs);
  ui->comboBox_StreamlinkQuality->setCurrentIndex(streamlinkerino::StreamQuality[streamQuality]);
  if (streamlinkerino::isDarkTheme())
    streamlinkerino::setDarkTitlebar(winId());
  this->show();
}

bool SettingsDialog::exeExists(const QString& path) {
  QFileInfo executable(path);
  // If executable found
  if (executable.exists() && executable.isFile())
    return true;
  return false;
}

void SettingsDialog::decorateLabel(QLabel* label, const QString& msg, const QString& style) {
  label->setText(msg);
  label->setStyleSheet(style);
  label->show();
}

void SettingsDialog::validatePath(QLineEdit* textline,
                                  QLabel* label,
                                  const QString& exe,
                                  const QString& exepath) {
  textline->setText(QDir::fromNativeSeparators(textline->text()));
  if (textline->text().length() < 2) {
    ui->pushButton->setDisabled(true);
    decorateLabel(label, exe + " Not Found", streamlinkerino::style::_styleRed);
    return;
  }
  for (int i = 2; i <= streamlinkerino::path::EXE_NAME_LEN; i++)
    if (textline->text().right(i).toLower() == exe.left(i)) {
      QString path = textline->text();

      // If executable found
      if (exeExists(path)) {
        qDebug() << exe + "Found:" << path;
        decorateLabel(label, exe + " Found", streamlinkerino::style::_styleGreen);
        emit exeFound();
        return;
      }
      // IF not
      decorateLabel(label, exe + " Not Found", streamlinkerino::style::_styleRed);
      // Auto-detect executable and set Completer;
      QStringList genPath(
          {textline->text().simplified() + exe.right(streamlinkerino::path::EXE_NAME_LEN - i),
           textline->text().simplified().chopped(i) + exepath});
      if (!exeExists(genPath.back()))
        genPath.pop_back();
      if (!exeExists(genPath.front()))
        genPath.pop_front();
      if (genPath.isEmpty()) {
        return;
      } else {
        QCompleter* completer = new QCompleter(genPath, this);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        textline->setCompleter(completer);
      }
      break;
    }
  ui->pushButton->setDisabled(true);
  decorateLabel(label, exe + " Not Found", streamlinkerino::style::_styleRed);
  return;
}

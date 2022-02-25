#ifndef SUBMODULESDIALOG_H
#define SUBMODULESDIALOG_H
#include "debugging.h"
#include "windark.h"
#include <QCloseEvent>
#include <QDebug>
#include <QDialog>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QSettings>

//#include <QComboBox>

#define db qDebug() << this <<
// need to fix this
const QMap<QString, qint32> StreamQuality = {{"Best", 0}, {"1080p60", 1}, {"720p60", 2}, {"720p", 3}, {"480p", 4}, {"360p", 5}, {"160p", 6}, {"Worst", 7}, {"Audio only", 8}};
namespace Ui
{
  class SubmodulesDialog;
}

namespace Submodules
{
  enum ChangeFlags
  {
    StreamLink = 0x01,
    Chatterino = 0x02
  };

  class Streamlink
  {
  public:
    Streamlink(QString quality);
    ~Streamlink();
    QStringList getArgs(QString channel, WId mpvContainer);

    void setQuality(QString quality);

  private:
    QString _quality;
  };

  class SubmodulesDialog : public QDialog
  {
    Q_OBJECT
  signals:
    void settingFinished();
    void refreshStream();

  public:
    explicit SubmodulesDialog(QWidget *parent = nullptr);
    ~SubmodulesDialog();

    void showMessageBox();

    QString streamlinkPath();
    QString chatterinoPath();
    QString streamlinkArguments();
    QString streamlinkQuality();

    QStringList getStreamLinkArguments(QString channel, WId mpvContainer);

    int getChanges();
    ;
  public slots:
    void showDialog();
    void initialize();

  private:
    void loadSettings();
    void setupConnections();
    void hideAlerts();

    Ui::SubmodulesDialog *ui;
    Streamlink *_SL;
    QString _streamlinkPath;
    QString _streamlinkArgs;
    QString _chatterinoPath;
    QString _streamlinkQuality; // default quality

    QString _styleGreen = "color: rgb(44, 145, 7);";
    QString _styleRed = "color: rgb(156, 12, 21);";
    QSettings settings = QSettings(QSettings::NativeFormat, QSettings::UserScope, "streamlinkerino", "streamlinkerino"); // Registry setting
    bool isInit;

    int _changes = 0;
  };
} // namespace Submodules
#endif // SUBMODULESDIALOG_H

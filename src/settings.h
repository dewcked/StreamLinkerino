#pragma once

#include <QDialog>
#include <QList>

#define db qDebug() << this <<

namespace streamlinkerino {
  class Settings : public QObject {
    Q_OBJECT
   signals:
    void initialized();
    void openSettingsSignal(QString chatterinoPath,
                            QString streamlinkPath,
                            QString streamlinkArgs,
                            QString streamQuality);

   public:
    QString streamlinkPath();
    QString chatterinoPath();
    QStringList streamlinkArguments(QString channel, WId mpvContainer, int socketNumber);

   public slots:
    void initialize();
    void setSettings(QString chatterinoPath,
                     QString streamlinkPath,
                     QString streamlinkArgs,
                     QString streamQuality);
    void openSettings();

   private:
    void showMessageBox(int missing);

    QString _streamlinkPath;
    QString _streamlinkArgs;
    QString _chatterinoPath;
    QString _streamQuality;
    bool isInit = true;
  };
} // namespace streamlinkerino

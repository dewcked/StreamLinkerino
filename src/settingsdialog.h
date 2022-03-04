#pragma once

#include <QDialog>

#include "ui_settingsDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
  class SettingsDialog;
}
QT_END_NAMESPACE

class SettingsDialog : public QDialog {
  Q_OBJECT
 signals:
  void settingFinished(QString chatterinoPath,
                       QString streamlinkPath,
                       QString streamlinkArgs,
                       QString streamQuality);
  void refreshStream();
  void exeFound();

 public:
  explicit SettingsDialog(QWidget* parent = nullptr);
  ~SettingsDialog();

 public slots:
  void showDialog(QString chatterinoPath,
                  QString streamlinkPath,
                  QString streamlinkArgs,
                  QString streamQuality);

 private:
  Ui::SettingsDialog* ui;
  void setupConnections();
  void hideAlerts();
  bool exeExists(const QString& path);
  void decorateLabel(QLabel* label, const QString& msg, const QString& style);
  void validatePath(QLineEdit* textbox, QLabel* label, const QString& exe, const QString& exepath);

  bool isInit;
};

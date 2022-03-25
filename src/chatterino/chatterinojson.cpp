#include "chatterinojson.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace streamlinkerino {
  ChatterinoJSON::ChatterinoJSON() { ChatterinoJSON::loadChannel(); }

  // Load current channels from json file
  void ChatterinoJSON::loadChannel() {
    // qDebug() << WINDOW_LAYOUT_JSON;
    QFile file(streamlinkerino::path::WINDOW_LAYOUT_JSON);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      QString text = file.readAll();
      file.close();
      QJsonDocument jdoc = QJsonDocument::fromJson(text.toUtf8());
      QJsonArray channels = jdoc.object()
                                .value(QString("windows"))
                                .toArray()
                                .at(0)
                                .toObject()
                                .value(QString("tabs"))
                                .toArray();
      ChatterinoJSON::clearChannel();
      foreach (const QJsonValue& channel, channels) {
        QJsonObject channel_info = channel["splits2"]["data"].toObject();
        QString channel_name = channel_info.value(QString("name")).toString();
        if (channel_info.value(QString("type")).toString()
            == QString("twitch")) {
          if (channel["selected"].toBool())
            ChatterinoJSON::setActiveChannel(channel_name);
          ChatterinoJSON::addChannel(channel_name);
        }
      }
    }
  }
  qsizetype ChatterinoJSON::channalCount() const { return channels.count(); }
  QStringList ChatterinoJSON::getChannels() const { return channels; }
  void ChatterinoJSON::addChannel(const QString& channel) {
    channels.push_back(channel);
  }
  void ChatterinoJSON::clearChannel() { channels.clear(); }
  QString ChatterinoJSON::getActiveChannel() const { return activeChannel; }
  void ChatterinoJSON::setActiveChannel(const QString& channel) {
    activeChannel = channel;
  }
} // namespace streamlinkerino

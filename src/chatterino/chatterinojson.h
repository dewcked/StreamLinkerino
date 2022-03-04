#pragma once
#include <QList>

#include "path.h"

namespace streamlinkerino {
  class ChatterinoJSON {
   public:
    ChatterinoJSON();
    void loadChannel();
    qsizetype channalCount() const;
    QStringList getChannels() const;
    QString getActiveChannel() const;

   private:
    void addChannel(const QString& channel);
    void clearChannel();
    void setActiveChannel(const QString& channel);

    QStringList channels;
    QString activeChannel;
  };
} // namespace streamlinkerino

#pragma once
#include <QDebug>

#define qDebug2() qDebug() << "  └"
#define qDebug3() qDebug() << "    └"
#define qDebug_() qDebug() << " "
#define qDebug2_() qDebug() << "   "
#define qDebug3_() qDebug() << "     "
#define qDebugSlot() qDebug() << "[Slot]"
#define qDebugSocket() qDebug() << "    >> "

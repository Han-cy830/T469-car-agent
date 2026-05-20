QT += core gui widgets

TARGET = t469-car-agent
TEMPLATE = app
CONFIG += c++11

# 禁用已弃用的警告
DEFINES += QT_DEPRECATED_WARNINGS

include(core/core.pri)
include(models/models.pri)
include(widgets/widgets.pri)
include(utils/utils.pri)

SOURCES += main.cpp

# Windows 下隐藏控制台窗口
win32 {
    CONFIG += console
}

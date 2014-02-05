QT += core gui

TARGET = Dialogs
TEMPLATE = app

SOURCES += main.cpp \
dialogs.cpp \
../translate.cpp

HEADERS += dialogs.h \
../translate.h

TRANSLATIONS += locale/dialogs_zh_TW.ts \
locale/dialogs_zh_CN.ts \
locale/dialogs_ja_JP.ts

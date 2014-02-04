QT += core gui

TARGET = Dialogs
TEMPLATE = app

SOURCES += main.cpp \
dialogs.cpp \
../translate.cpp

HEADERS += dialogs.h \
../translate.h

TRANSLATIONS += dialogs_zh_TW.ts \
dialogs_zh_CN.ts \
dialogs_ja_JP.ts

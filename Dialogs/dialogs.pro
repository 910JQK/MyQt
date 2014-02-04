QT += core gui

TARGET = Dialogs
TEMPLATE = app

SOURCES += main.cpp \
dialogs.cpp \
../translate.cpp

HEADERS += dialogs.h \
../translate.h

TRANSLATIONS += ts/dialogs_zh_TW.ts \
ts/dialogs_zh_CN.ts \
ts/dialogs_ja_JP.ts

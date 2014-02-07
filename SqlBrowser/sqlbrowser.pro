QT += core gui sql

TARGET = Sqlbrowser
TEMPLATE = app

SOURCES += main.cpp \
sqlbrowser.cpp \
../translate.cpp

HEADERS += sqlbrowser.h \
../translate.h

TRANSLATIONS += locale/sqlbrowser_ja_JP.ts \
locale/sqlbrowser_zh_CN.ts \
locale/sqlbrowser_zh_TW.ts

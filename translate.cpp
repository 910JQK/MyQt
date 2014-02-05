#include <QtGui/QApplication>
#include <QtCore>
#include <QLocale>
#include <QLibraryInfo>
#include "translate.h"


TR::TR(QApplication *APP, QString PFNAME, QString TFDIR){
  app = APP;
  PreFileName = PFNAME;
  TFileDir = TFDIR;

  //取得系統語言環境
  localeName = QLocale::system().name();

  //程式翻譯
  appT = new QTranslator;
  appT -> load(PreFileName+localeName, TFileDir );
  app -> installTranslator(appT);

  //Qt翻譯
  sysT = new QTranslator;
  sysT -> load("qt_"+localeName,QLibraryInfo::location(QLibraryInfo::TranslationsPath));
  app -> installTranslator(sysT);
}


TR::~TR(){

}


void TR::ChangeLocale(QString CurrentLocale){
  //卸下原來的Translator
  app -> removeTranslator(appT);
  app -> removeTranslator(sysT);

  //程式重新翻譯
  appT -> load(PreFileName+CurrentLocale, TFileDir );
  app -> installTranslator(appT);

  //Qt重新翻譯
  sysT -> load("qt_"+CurrentLocale,QLibraryInfo::location(QLibraryInfo::TranslationsPath));
  app -> installTranslator(sysT);

  //發送重翻譯信號
  UpdateUI();
}

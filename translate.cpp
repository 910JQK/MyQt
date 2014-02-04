#include <QtGui/QApplication>
#include <QtCore>
#include <QLocale>
#include <QLibraryInfo>


TR::TR(QApplication *APP, QString PFNAME){
  app = APP;
  PreFileName = PFNAME;

  //取得系統語言環境
  QString localeName = QLocale::system().name();

  //程式翻譯
  appT = new QTranslator;
  appT -> load(PreFileName+localeName, "locale" );
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

  //程式翻譯
  appT -> load(PreFileName+CurrentLocale, "locale" );
  app -> installTranslator(appT);

  //Qt翻譯
  sysT -> load("qt_"+CurrentLocale,QLibraryInfo::location(QLibraryInfo::TranslationsPath));
  app -> installTranslator(sysT);
}

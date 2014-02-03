#include <QtGui/QApplication>
#include <QtCore>
#include <QLocale>
#include <QLibraryInfo>
#include "hello.h"

int main(int argc, char *argv[]){
  //創建QApplication
  QApplication a(argc,argv);

  //取得系統語言環境
  QString localeName = QLocale::system().name();

  //程式翻譯
  QTranslator appT;
  appT.load("hello_"+localeName, "locale" );
  a.installTranslator(&appT);

  //Qt翻譯
  QTranslator sysT;
  sysT.load("qt_"+localeName,QLibraryInfo::location(QLibraryInfo::TranslationsPath));
  a.installTranslator(&sysT);

  //創建MainWindow
  MainWindow W;

  //綁定退出信號
  QObject::connect(&W, SIGNAL(ExitSignal()), &a, SLOT(quit()) );

  //顯示
  W.show();

  //進入迴圈
  return a.exec();
}

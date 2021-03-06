#include <QtGui/QApplication>
#include <QTextCodec>
#include "../translate.h"
#include "dialogs.h"

int main(int argc, char *argv[]){
  QApplication a(argc,argv);

  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
  TR appTR(&a,"dialogs_","locale");

  MainWindow W;
  W.LC_init(appTR.localeName);
  QObject::connect(&W, SIGNAL(ExitSignal()), &a, SLOT(quit()) );
  QObject::connect(&W, SIGNAL(LC_Change_Signal(const QString &)), &appTR, SLOT(ChangeLocale(const QString &)) );
  QObject::connect(&appTR, SIGNAL(UpdateUI()), &W, SLOT(reTranslate()) );

  W.show();

  return a.exec();
}

#include <QApplication>
#include "sqlbrowser.h"
#include "../translate.h"

int main(int argc, char *argv[]){
  QApplication a(argc,argv);
  TR appTR(&a,"sqlbrowser_","locale");

  MainWindow W;
  W.show();

  return a.exec();
}

#include <QApplication>
#include <QList>
#include "sqlbrowser.h"
#include "../translate.h"

int main(int argc, char *argv[]){
  if(argc > 1){
    if(strcmp(argv[1],"--help") == 0){
      MainWindow::OutputHelp();
      return 0;
    }
  }

  QApplication a(argc,argv);
  TR appTR(&a,"sqlbrowser_","locale");
  
  QList<MainWindow*> W;
  MainWindow::handleArgs(&W);

  return a.exec();
}


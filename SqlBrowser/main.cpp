#include <QApplication>
#include "sqlbrowser.h"
#include "../translate.h"

int main(int argc, char *argv[]){
  QApplication a(argc,argv);

  if(a.arguments().length() > 1){
    if(a.arguments()[1] == "--help" || a.arguments()[1] == "-h"){
      MainWindow::OutputHelp();
      a.quit();
      return 0;
    }
  }
  TR appTR(&a,"sqlbrowser_","locale");

  MainWindow *W;
  W = new MainWindow;
  W -> show();

  return a.exec();
}


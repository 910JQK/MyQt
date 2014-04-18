#include <QApplication>
#include <QLabel>
#include <QDBusConnection>
#include "adaptor.h"

int main(int argc, char *argv[]){
  /* Application */
  QApplication a(argc, argv);
  /* Window */
  QLabel *l = new QLabel();
  l -> setWindowTitle("DBus Test");
  l ->setText("<h3>Test</h3>");
  l -> show();
  /* DBus */
  new Adaptor(l);
  QDBusConnection con = QDBusConnection::sessionBus();
  con.registerObject("/Hello",l);
  con.registerService("org.poker.hello");
  QDBusConnection::sessionBus().connect(QString(), QString(), "org.poker.hello", "ToggleVisible", l, SLOT(setVisible(bool)) );
  QDBusConnection::sessionBus().connect(QString(), QString(), "org.poker.hello", "ChangeText", l, SLOT(setText(QString)) );
  /* return */
  return a.exec();
}

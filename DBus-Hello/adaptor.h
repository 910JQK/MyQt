#ifndef _DBUSHELLO_ADAPTOR_H
#define _DBUSHELLO_ADAPTOR_H
#include <QDBusAbstractAdaptor>

class Adaptor : public QDBusAbstractAdaptor {
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "org.poker.hello")
  Q_CLASSINFO("D-Bus Introspection", ""
 "  <interface name=\"org.poker.hello\">\n"
 "    <signal name=\"ToggleVisible\">\n"
 "      <arg direction=\"out\" type=\"s\" name=\"stat\"/>\n"
 "    </signal>\n"
 "    <signal name=\"ChangeText\">\n"
 "      <arg direction=\"out\" type=\"s\" name=\"txt\"/>\n"
 "    </signal>\n"
 "  </interface>\n"
	      "")
public:
  Adaptor(QObject *parent);
  ~Adaptor();
Q_SIGNALS:
  void ToggleVisible(const bool &stat);
  void ChangeText(const QString &txt);
};

#endif //_DBUSHELLO_ADAPTOR_H

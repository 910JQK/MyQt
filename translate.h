#include <QObject>
class QApplication;
class QTranslator;
class QString;

class TR : public QObject
{
  Q_OBJECT
public:
  TR(QApplication *APP, QString PFNAME);
  ~TR();
  QApplication *app;
  QTranslator *appT;
  QTranslator *sysT;
  QString PreFileName;
public slots:
  void ChangeLocale(QString CurrentLocale);
};


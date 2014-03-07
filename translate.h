#ifndef MYQT_TRANSLATE_H_
#define MYQT_TRANSLATE_H_
#include <QObject>
class QApplication;
class QTranslator;
class QString;

class TR : public QObject
{
  Q_OBJECT
public:
  TR(QApplication *APP, QString PFNAME, QString TFDIR);
  ~TR();
  QApplication *app;
  QTranslator *appT;
  QTranslator *sysT;
  QString PreFileName;
  QString TFileDir;
  QString localeName;
signals:
  void UpdateUI();
public slots:
  void ChangeLocale(QString CurrentLocale);
};

#endif //MYQT_TRANSLATE_H_

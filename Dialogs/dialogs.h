#include <QMainWindow>

class QString;
class QMenu;
class QTextEdit;
class QLabel;
class QAction;
class QActionGroup;

class MainWindow : public QMainWindow
{
  Q_OBJECT
public:
  MainWindow(QWidget *parent = 0);
  ~MainWindow();
  void LC_init(QString init_lc);
signals:
  void LC_Change_Signal(QString whatlocale);
  void ExitSignal();
private:
  QMenu *menu1;
  QMenu *menu2;
  QLabel *msglabel;
  QTextEdit *textedit;
  QAction *FontDialogAction;
  QAction *ColorDialogAction;
  QAction *ExitAction;
  QAction *LC_en_US_Action;
  QAction *LC_ja_JP_Action;
  QAction *LC_zh_CN_Action;
  QAction *LC_zh_TW_Action;
  QActionGroup *LC_Group;
private slots:
  void setFont();
  void setColor();
  void Ticker();
  void LC_Changed();
  void reTranslate();
};

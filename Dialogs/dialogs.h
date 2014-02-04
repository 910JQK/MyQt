#include <QMainWindow>

class QMenu;
class QTextEdit;
class QAction;
class QActionGroup;

class MainWindow : public QMainWindow
{
  Q_OBJECT
public:
  MainWindow(QWidget *parent = 0);
  ~MainWindow();
signals:
  void LC_Change_Signal(QString whatlocale);
  void ExitSignal();
private:
  QMenu *menu1;
  QMenu *menu2;
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
  void LC_Changed();
  void setFont();
  void setColor();
  void reTranslate();
};

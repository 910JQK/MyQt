#include <QMainWindow>
#include <QStringList>
#include <QSqlDatabase>

class QStringListModel;
class QString;
class QModelIndex;
class QListView;
class QTabWidget;
class QAction;
class QSqlDatabase;


class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  MainWindow(QWidget *parent = 0);
  ~MainWindow();
public slots:
  void DB_Open();
  void SqliteOpen(QString whichDB);
  int OpenTable(const QModelIndex &idx);
  void tabClosing(int whichTab);
private:
  QStringListModel *tableListModel;
  QSqlDatabase DB;
  QStringList OpenedTableList;
  QListView *tableList;
  QTabWidget *tabs;
  QAction *openAction;
};

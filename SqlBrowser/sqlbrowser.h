#include <QMainWindow>
#include <QStringList>
#include <QSqlDatabase>
#include <QList>

class QStringListModel;
class QSqlTableModel;
class QTableView;
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
  void ListDebugging();
  void SaveCurrent();
private:
  QSqlDatabase DB;
  QStringList OpenedTableList;
  QStringListModel *tableListModel;
  QListView *tableList;
  QTabWidget *tabs;
  QAction *openAction;
  QAction *submitAction;
  QList<QSqlTableModel*> tableModel;
  QList<QTableView*> tableView;
};

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
class QLineEdit;
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
  void tabChanged(int whichTab);
  void tabClosing(int whichTab);
  void AddCurrent();
  void DelCurrent();
  void SaveCurrent();
  void RevertCurrent();
  void SetDirty();
  void NotDirty();
  void CheckDeletable();
  void do_query();
private:
  QSqlDatabase DB;
  QStringList OpenedTableList;
  QStringListModel *tableListModel;
  QListView *tableList;
  QTabWidget *tabs;
  QLineEdit *queryEdit;
  QAction *openAction;
  QAction *submitAction;
  QAction *revertAction;
  QAction *addAction;
  QAction *delAction;
  QList<QSqlTableModel*> tableModel;
  QList<QTableView*> tableView;
  QList<bool> tableDirty;
  QString DBNAME;
  int tableIndex();
  void ListDebugging();
};

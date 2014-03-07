#ifndef SQLBROWSER_SQLBROWSER_H_
#define SQLBROWSER_SQLBROWSER_H_
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
class QDockWidget;
class QLineEdit;
class QAction;
class QSqlQuery;
class QSqlQueryModel;
class QApplication;

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  MainWindow(QWidget *parent = 0);
  ~MainWindow();
  static void handleArgs(QList<MainWindow*> *W_List);
  static void OutputHelp();
  static std::string stdTr(std::string Str);
  QString DB_Type;
  QString sqliteFile;
  QString sqlHost;
  int sqlPort;
  QString sqlUser;
  QString sqlPasswd;
  QString sqlDB;
public slots:
  void openMysqlByArgs(QStringList dataField);
  void DB_Open();
  void SqliteOpen(QString whichDB);
  void MySqlOpen(QString host, int port, QString user, QString passwd, QString dbName);
  void OpenClear();
  int OpenTable(const QModelIndex &idx);
  void tabChanged(int whichTab);
  void tabClosing(int whichTab);
  void AddCurrent();
  void DelCurrent();
  void SaveCurrent();
  void RevertCurrent();
  void RefreshCurrent();
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
  QDockWidget *dock;
  QLineEdit *queryEdit;
  QAction *openAction;
  QAction *submitAction;
  QAction *revertAction;
  QAction *refreshAction;
  QAction *addAction;
  QAction *delAction;
  QList<QSqlTableModel*> tableModel;
  QList<QTableView*> tableView;
  QList<bool> tableDirty;
  QList<int> tabType;
  QList<QSqlQuery*> queryList;
  QList<QSqlQueryModel*> queryModel;
  QString DBNAME;
  int tableIndex();
  void ListDebugging();
};

#endif //SQLBROWSER_SQLBROWSER_H_

#include <QSqlDatabase>
#include <QSqlError>
#include <QTabWidget>
#include <QStringListModel>
#include <QListView>
#include <QSqlTableModel>
#include <QTableView>
#include <QModelIndex>
#include <QToolBar>
#include <QAction>
#include <QFileDialog>
#include <QDebug>
#include "sqlbrowser.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
  //資料表列表的Model&View
  tableListModel = new QStringListModel(this);
  tableList = new QListView(this);
  tableList -> setEditTriggers(QAbstractItemView::EditKeyPressed);
  tableList -> setModel(tableListModel);
  connect(tableList, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(OpenTable(const QModelIndex &)) );

  //標籤
  tabs = new QTabWidget(this);
  tabs -> setTabsClosable(true);
  tabs -> addTab(tableList,tr("Tables"));
  //Fucking protected element
  //tabs -> tabBar() -> tabButton(0,QTabBar::RightSide) -> hide();
  connect(tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(tabClosing(int)) );
  setCentralWidget(tabs);

  //工具條
  openAction = new QAction(tr("Open file..."),this);
  connect(openAction, SIGNAL(triggered()), this, SLOT(DB_Open()) );
  QToolBar *toolbar = addToolBar(tr("Toolbar"));
  toolbar -> addAction(openAction);
}


MainWindow::~MainWindow(){

}


void MainWindow::DB_Open(){
  QFileDialog fd(this);
  fd.setFileMode(QFileDialog::ExistingFile);
  fd.setAcceptMode(QFileDialog::AcceptOpen);
  fd.setViewMode(QFileDialog::Detail);
  if(fd.exec()){
    SqliteOpen(fd.selectedFiles()[0]);
  }
}


void MainWindow::SqliteOpen(QString whichDB){
  if(DB.isValid()){
    int tabsNum = tabs->count();
    qDebug()<<tabsNum;
    for(int i=tabsNum; i >= 1; i--){
      tabs -> removeTab(i);
    }
    OpenedTableList.clear();
  }
  DB = QSqlDatabase::addDatabase("QSQLITE");
  DB.setDatabaseName(whichDB);
  if(DB.open()){
    tableListModel -> setStringList(DB.tables());
  }else{
    qDebug() << "Error:Cannot open database " + whichDB + " - " + DB.lastError().text();
  }
}


int MainWindow::OpenTable(const QModelIndex &idx){
  //取得表名
  QString whichTable = tableListModel -> stringList()[idx.row()];

  //判斷是否已開
  if(OpenedTableList.indexOf(whichTable) != -1)
    return 1;
  else
    OpenedTableList << whichTable;

  //創建SqlTableModel
  QSqlTableModel *tableModel;
  tableModel = new QSqlTableModel(this,DB);
  tableModel -> setTable(whichTable);
  tableModel -> select();

  //創建對應的TableView
  QTableView *tableView;
  tableView = new QTableView(this);
  tableView -> setModel(tableModel);
  //新加標籤
  tabs -> addTab(tableView,whichTable);
  //轉到標籤
  tabs -> setCurrentIndex(tabs->count() - 1);
  return 0;
}

void MainWindow::tabClosing(int whichTab){
  if(whichTab != 0){
  OpenedTableList.removeOne(tabs->tabText(whichTab));
  tabs -> removeTab(whichTab);
  }
}

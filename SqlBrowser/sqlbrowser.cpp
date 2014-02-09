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
#include <QIcon>
#include <QKeySequence>
#include <QFileDialog>
#include <QDebug>
#include "sqlbrowser.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
  //臨時性代碼
  QIcon::setThemeName("oxygen");

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
  openAction -> setShortcut(QKeySequence("Ctrl+O"));
  openAction -> setIcon(QIcon::fromTheme("document-open"));
  connect(openAction, SIGNAL(triggered()), this, SLOT(DB_Open()) );
  submitAction = new QAction(tr("Save changes of current table"),this);
  submitAction -> setEnabled(false);
  submitAction -> setShortcut(QKeySequence("Ctrl+S"));
  submitAction -> setIcon(QIcon::fromTheme("document-save"));
  connect(submitAction, SIGNAL(triggered()), this, SLOT(SaveCurrent()) );
  QToolBar *toolbar = addToolBar(tr("Toolbar"));
  toolbar -> addAction(openAction);
  toolbar -> addAction(submitAction);
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
  //若已經打開一個資料庫則進行清理
  if(DB.isValid()){
    int tabsNum = tabs->count();
    for(int i=tabsNum; i >= 1; i--){
      tabs -> removeTab(i);
    }
    tableView.clear();
    tableModel.clear();
    OpenedTableList.clear();
    qDebug()<<"Cleaning";
    ListDebugging();
  }

  //打開資料庫
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
  QSqlTableModel *tableModela;
  tableModela = new QSqlTableModel(this,DB);
  tableModela -> setEditStrategy(QSqlTableModel::OnManualSubmit);
  tableModela -> setTable(whichTable);
  tableModela -> select();
  tableModel.append(tableModela);

  //創建對應的TableView
  QTableView *tableViewa;
  tableViewa = new QTableView(this);
  tableViewa -> setModel(tableModela);
  tableViewa -> resizeColumnsToContents();
  tableView.append(tableViewa);

  //新加標籤
  tabs -> addTab(tableViewa,whichTable);
  //轉到標籤
  tabs -> setCurrentIndex(tabs->count() - 1);

  qDebug()<<"Opening:"<<whichTable;
  ListDebugging();

  return 0;
}

void MainWindow::tabClosing(int whichTab){
  if(whichTab != 0){
  OpenedTableList.removeOne(tabs->tabText(whichTab));
  tableView.removeAt(whichTab-1);
  tableModel.removeAt(whichTab-1);
  tabs -> removeTab(whichTab);
  qDebug()<<"Closing:"<<whichTab;
  ListDebugging();
  }
}

void MainWindow::ListDebugging(){
  qDebug()<<"tableView:"<<tableView.count();
  qDebug()<<"tableModel:"<<tableModel.count();
}

void MainWindow::SaveCurrent(){
  int save_index = tabs->currentIndex()-1;
  if(save_index >= 0){
    DB.transaction();
    if(tableModel[save_index] -> submitAll()){
      DB.commit();
    }else{
      DB.rollback();
      qDebug()<<"Error while submiting data.";
    }
  }
}

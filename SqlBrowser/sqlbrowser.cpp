#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTabWidget>
#include <QStringListModel>
#include <QListView>
#include <QSqlTableModel>
#include <QTableView>
#include <QLineEdit>
#include <QModelIndex>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QAction>
#include <QIcon>
#include <QKeySequence>
#include <QFileDialog>
#include <QApplication>
#include <QDesktopWidget>
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

  //DirtyList
  tableDirty<<false;

  //標籤
  tabs = new QTabWidget(this);
  tabs -> setTabsClosable(true);
  tabs -> addTab(tableList,tr("Tables"));
  //Fucking protected element
  //tabs -> tabBar() -> tabButton(0,QTabBar::RightSide) -> hide();
  connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)) );
  connect(tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(tabClosing(int)) );
  setCentralWidget(tabs);

  //工具條
  openAction = new QAction(tr("Open file..."),this);
  openAction -> setShortcut(QKeySequence("Ctrl+O"));
  openAction -> setIcon(QIcon::fromTheme("document-open"));
  submitAction = new QAction(tr("Save changes of current table"),this);
  submitAction -> setEnabled(false);
  submitAction -> setShortcut(QKeySequence("Ctrl+S"));
  submitAction -> setIcon(QIcon::fromTheme("document-save"));
  revertAction = new QAction(tr("Revert all changes of current table"),this);
  revertAction -> setEnabled(false);
  revertAction -> setShortcut(QKeySequence("Ctrl+R"));
  revertAction -> setIcon(QIcon::fromTheme("document-revert"));
  addAction = new QAction(tr("Add a row record to current table"),this);
  addAction -> setEnabled(false);
  addAction -> setIcon(QIcon::fromTheme("list-add"));
  delAction = new QAction(tr("Delete a row record from current table"),this);
  delAction -> setEnabled(false);
  delAction -> setIcon(QIcon::fromTheme("list-remove"));
  connect(openAction, SIGNAL(triggered()), this, SLOT(DB_Open()) );
  connect(submitAction, SIGNAL(triggered()), this, SLOT(SaveCurrent()) );
  connect(revertAction, SIGNAL(triggered()), this, SLOT(RevertCurrent()) );
  connect(addAction, SIGNAL(triggered()), this, SLOT(AddCurrent()) );
  connect(delAction, SIGNAL(triggered()), this, SLOT(DelCurrent()) );
  //--
  QToolBar *toolbar = addToolBar(tr("Toolbar"));
  toolbar -> addAction(openAction);
  toolbar -> addAction(submitAction);
  toolbar -> addAction(revertAction);
  toolbar -> addAction(addAction);
  toolbar -> addAction(delAction);

  //狀態列
  queryEdit = new QLineEdit;
  queryEdit -> setPlaceholderText(tr("Input SQL command here"));
  queryEdit -> setEnabled(false);
  connect(queryEdit, SIGNAL(returnPressed()), this, SLOT(do_query()) );
  QDockWidget *dock = new QDockWidget("SqlQuery",this);
  dock -> setWidget(queryEdit);
  dock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
  addDockWidget(Qt::BottomDockWidgetArea, dock);

  //Size and Position
  resize(680,360);
  QDesktopWidget *desktop = QApplication::desktop();
  move((desktop->width() - this->width())/2, (desktop->height() - this->height())/2);
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
    tableDirty.clear();
    tableDirty<<false;
    OpenedTableList.clear();
    DB.close();
    DB.removeDatabase(DBNAME);
    qDebug()<<"Cleaning";
    ListDebugging();
  }

  //打開資料庫
  DB = QSqlDatabase::addDatabase("QSQLITE");
  DB.setDatabaseName(whichDB);
  if(DB.open()){
    tableListModel -> setStringList(DB.tables());
    DBNAME=whichDB;
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
  connect(tableModela, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex &)), this, SLOT(SetDirty()) );

  //創建對應的TableView
  QTableView *tableViewa;
  tableViewa = new QTableView(this);
  tableViewa -> setModel(tableModela);
  tableViewa -> resizeColumnsToContents();
  tableView.append(tableViewa);
  connect(tableViewa, SIGNAL(viewportEntered()), this, SLOT(CheckDeletable()) );

  //DirtyList
  tableDirty<<false;

  //新加標籤
  tabs -> addTab(tableViewa,whichTable);
  //設定圖標
  tabs -> setTabIcon(tabs->count()-1, QIcon::fromTheme("folder"));
  //轉到標籤
  tabs -> setCurrentIndex(tabs->count()-1);

  //Debug
  qDebug()<<"Opening:"<<whichTable;
  ListDebugging();
  
  //返回值
  return 0;
}


void MainWindow::tabChanged(int whichTab){
  if(whichTab != 0){
    if(tableDirty[whichTab]){
      submitAction -> setEnabled(true);
      revertAction -> setEnabled(true);
      qDebug()<<"Table "<<whichTab<<":Dirty";
    }else{
      submitAction -> setEnabled(false);
      revertAction -> setEnabled(false);
      qDebug()<<"Table "<<whichTab<<":Non-dirty";
    }
    queryEdit -> setEnabled(true);
    addAction -> setEnabled(true);
    CheckDeletable();
  }else{
    submitAction -> setEnabled(false);
    revertAction -> setEnabled(false);
    queryEdit -> setEnabled(false);
    addAction -> setEnabled(false);
    delAction -> setEnabled(false);
  }
}


void MainWindow::tabClosing(int whichTab){
  if(whichTab != 0){
  OpenedTableList.removeOne(tabs->tabText(whichTab));
  tableView.removeAt(whichTab-1);
  tableModel.removeAt(whichTab-1);
  tableDirty.removeAt(whichTab);
  tabs -> removeTab(whichTab);
  qDebug()<<"Closing:"<<whichTab;
  ListDebugging();
  }
}


void MainWindow::ListDebugging(){
  qDebug()<<"tableView:"<<tableView.count();
  qDebug()<<"tableModel:"<<tableModel.count();
}


int MainWindow::tableIndex(){
  return tabs->currentIndex()-1;
}


void MainWindow::AddCurrent(){
  int add_index = tableIndex();
  tableModel[add_index] -> insertRow(tableModel[add_index]->rowCount());
  SetDirty();
}


void MainWindow::DelCurrent(){
  //Index
  int del_index = tableIndex();

  //獲取被選擇列
  QList<int> del_rows;
  foreach(const QModelIndex &index, tableView[del_index]->selectionModel()->selection().indexes() ){
    del_rows.append(index.row());
  }

  //排序
  qSort(del_rows);

  //保護機制：序號一樣的新加行
  int prev = -1;
  //從後往前刪
  for(int i = del_rows.count()+1; i>=0; i--){
    int delCurrent = del_rows[i];
    if(delCurrent != prev){
    tableModel[del_index] -> removeRow(delCurrent);
    prev = delCurrent;
    }
  }
  SetDirty();
}


void MainWindow::SaveCurrent(){
  int save_index = tableIndex();
  DB.transaction();
  if(tableModel[save_index] -> submitAll()){
    DB.commit();
    NotDirty();
  }else{
    DB.rollback();
    qDebug()<<"Error while submiting data.";
  }
}


void MainWindow::RevertCurrent(){
  int revert_index = tableIndex();
  tableModel[revert_index] -> revertAll();
  NotDirty();
}


void MainWindow::SetDirty(){
  tableDirty[tabs->currentIndex()]=true;
  submitAction -> setEnabled(true);
  revertAction -> setEnabled(true);
  tabs -> setTabIcon(tabs->currentIndex(), QIcon::fromTheme("folder-open"));
}


void MainWindow::NotDirty(){
  tableDirty[tabs->currentIndex()]=false;
  submitAction -> setEnabled(false);
  revertAction -> setEnabled(false);
  tabs -> setTabIcon(tabs->currentIndex(), QIcon::fromTheme("folder"));
}


void MainWindow::CheckDeletable(){
  if(tabs->currentIndex() != 0){
    if(tableView[tabs->currentIndex()-1]->selectionModel()->hasSelection())
      delAction -> setEnabled(true);
    else
      delAction -> setEnabled(false);
  }
}


void MainWindow::do_query(){
  QSqlQuery query = tableModel[tabs->currentIndex()-1]->query();
  if(!query.exec(queryEdit->text()) ){
    qDebug()<<"Error while executing SQL command";
  }else{
    queryEdit -> setText("");
    tableListModel -> setStringList(DB.tables());
  }
}

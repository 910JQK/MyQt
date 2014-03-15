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
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include "sqlbrowser.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
  //臨時性代碼
  QIcon::setThemeName("oxygen");

  //資料表列表的Model&View
  tableListModel = new QStringListModel(this);
  tableList = new QListView(this);
  tableList -> setEditTriggers(QAbstractItemView::EditKeyPressed);
  //tableList -> setViewMode(QListView::IconMode);
  tableList -> setModel(tableListModel);
  connect(tableList, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(OpenTable(const QModelIndex &)) );

  //ListInit
  tableDirty<<false;
  tabType<<1;

  //標籤
  tabs = new QTabWidget(this);
  tabs -> setTabsClosable(true);
  tabs -> addTab(tableList,QIcon::fromTheme("x-office-document"),tr("Tables"));
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
  revertAction = new QAction(tr("Revert all changes of the current table"),this);
  revertAction -> setEnabled(false);
  revertAction -> setIcon(QIcon::fromTheme("document-revert"));
  refreshAction = new QAction(tr("Revert all changes of the current table and refresh data from database"),this);
  refreshAction -> setEnabled(false);
  refreshAction -> setIcon(QIcon::fromTheme("view-refresh"));
  addAction = new QAction(tr("Add a row record to the current table"),this);
  addAction -> setEnabled(false);
  addAction -> setIcon(QIcon::fromTheme("list-add"));
  delAction = new QAction(tr("Delete selected row-records from the current table"),this);
  delAction -> setEnabled(false);
  delAction -> setIcon(QIcon::fromTheme("list-remove"));
  connect(openAction, SIGNAL(triggered()), this, SLOT(DB_Open()) );
  connect(submitAction, SIGNAL(triggered()), this, SLOT(SaveCurrent()) );
  connect(revertAction, SIGNAL(triggered()), this, SLOT(RevertCurrent()) );
  connect(refreshAction, SIGNAL(triggered()), this, SLOT(RefreshCurrent()) );
  connect(addAction, SIGNAL(triggered()), this, SLOT(AddCurrent()) );
  connect(delAction, SIGNAL(triggered()), this, SLOT(DelCurrent()) );
  //--
  QToolBar *toolbar = addToolBar(tr("Toolbar"));
  toolbar -> addAction(openAction);
  toolbar -> addAction(submitAction);
  toolbar -> addAction(revertAction);
  toolbar -> addAction(refreshAction);
  toolbar -> addAction(addAction);
  toolbar -> addAction(delAction);

  //DockWidget
  queryEdit = new QLineEdit;
  queryEdit -> setPlaceholderText(tr("Input SQL command here"));
  queryEdit -> setEnabled(false);
  connect(queryEdit, SIGNAL(returnPressed()), this, SLOT(do_query()) );
  dock = new QDockWidget("SqlQuery",this);
  dock -> setWidget(queryEdit);
  dock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
  addDockWidget(Qt::BottomDockWidgetArea, dock);

  //Title
  setWindowTitle(tr("Sqlbrowser"));

  //Size and Position
  resize(720,400);
  QDesktopWidget *desktop = QApplication::desktop();
  move((desktop->width() - this->width())/2, (desktop->height() - this->height())/2);
}


MainWindow::~MainWindow(){

}

//BEGIN:static { 


void MainWindow::handleArgs(QList<MainWindow*> *W_List){
  //Handle the arguments
  QString _DB_Type = "QSQLITE";
  QStringList sFileList; 
  QString _sqlHost; //dataField[0]
  int _sqlPort = 3306; //[1]
  QString _sqlUser; //[2]
  QString _sqlDB; //[3]
  QStringListIterator args(QApplication::arguments());
  int ac = QApplication::arguments().count();
  if(ac > 1){
    args.next(); //GoToPos 0
    while(args.hasNext()){ //Read 1 by 1
      const QString &a = args.next();
      bool optsEnd = false;
      if(a == "--"){ //End of options
	optsEnd = true;
      }
      if(optsEnd == false){
	if(a == "--help"){
	  OutputHelp();
	}
	else if(a == "--mysql" || a == "-m"){
	  _DB_Type = "QMYSQL";
	}
	else if(a == "--host" || a == "-h"){
	  if(args.hasNext()){
	    _sqlHost = args.next();
	  }
	}
	else if(a == "--port" || a == "-p"){
	  if(args.hasNext()){
	    _sqlPort = args.next().toInt();
	  }
	}
	else if(a == "--user" || a == "-u"){
	  if(args.hasNext()){
	    _sqlUser = args.next();
	  }
	}
	else if(a == "--database" || a == "-d"){
	  if(args.hasNext()){
	    _sqlDB = args.next();
	  }
	}
	else if(_DB_Type=="QSQLITE"){ 	//SQLite3DBFile
	  sFileList << a;
	}
      }
      else if(_DB_Type=="QSQLITE"){ 	//SQLite3DBFile
	sFileList << a;
      }
    }
  }
  if(_DB_Type == "QMYSQL"){
    //CheckEnough
    if(_sqlHost.isEmpty() || _sqlUser.isEmpty() ){
      qDebug()<<tr("Error:Not enough arguments to open the Mysql database");
      MainWindow *window;
      window = new MainWindow();
      qDebug() << "Memory allocated";
      window -> show();
      W_List -> append(window);      
    }else{
      //Mysql
      MainWindow *window;
      window = new MainWindow();
      window -> DB_Type = _DB_Type;
      window -> openMysqlByArgs( QStringList() << _sqlHost << QString::number(_sqlPort) << _sqlUser << _sqlDB );
      window -> show();
      W_List -> append(window);
    }
  }else{
    if(sFileList.count() != 0){
      // MultiDB or 1 from args
      foreach(QString currentFile, sFileList){
	MainWindow *window;
	window = new MainWindow();
	qDebug() << "Memory allocated";
	window -> DB_Type = _DB_Type;
	window -> SqliteOpen(currentFile);
	window -> show();
	W_List -> append(window);
      }
    }else{
      //Non-preopen
      MainWindow *window;
      window = new MainWindow();
      qDebug() << "Memory allocated";
      window -> DB_Type = _DB_Type;
      window -> show();
      W_List -> append(window);
    }
  }
  //---------------
}


void MainWindow::OutputHelp(){
  using namespace std;
  cout << "usage: Sqlbrowser [--help|-h] [--mysql|-m] [--port|-p " << QT_TR_NOOP("PORT") << "] [--host|-h " << QT_TR_NOOP("HOST") << "] [--user|-u " << QT_TR_NOOP("USERNAME") << "] [--database|-d " << QT_TR_NOOP("DBNAME") << "] [" << QT_TR_NOOP("DBFILE") << "]" << endl << endl;
  cout << QT_TR_NOOP("optional arguments:") << endl;
  cout << "  " << QT_TR_NOOP("DBFILE") << endl << "     " << QT_TR_NOOP("Sqlite3 database file to be opened") << endl << endl;
  cout << "  -h, --help" << endl << "       " << QT_TR_NOOP("Show this help message and exit") << endl << endl;
  cout << "  -m, --mysql" << endl << "       " << QT_TR_NOOP("Specify database type : Mysql") << endl << endl;
  cout << "  -p " << QT_TR_NOOP("PORT") << ", --port " << QT_TR_NOOP("PORT") << endl << "       " << QT_TR_NOOP("Port of Mysql connection, 3306 for default") << endl << endl;
  cout << "  -h " << QT_TR_NOOP("HOST") << ", --host " << QT_TR_NOOP("HOST") << endl << "       " << QT_TR_NOOP("Host of Mysql connection") << endl << endl;
  cout << "  -u " << QT_TR_NOOP("USERNAME") << ", --user " << QT_TR_NOOP("USERNAME") << endl << "       " << QT_TR_NOOP("Username of Mysql connection") << endl << endl;
  cout << "  -d " << QT_TR_NOOP("DBNAME") << ", --database " << QT_TR_NOOP("DBNAME") << endl << "       " << QT_TR_NOOP("Database of Mysql connection");

  cout << endl;
}


//}END:static 


void MainWindow::openMysqlByArgs(QStringList dataField){
  // Open the mysql database in arguments
  sqlHost = dataField[0];
  sqlPort = dataField[1].toInt();
  sqlUser = dataField[2];
  sqlDB = dataField[3];
  
    //GetPassword
    std::cout<<tr("Password:").toStdString();
    //HideInput
    termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    //Get
    std::string s;
    getline(std::cin, s);
    sqlPasswd = QString::fromStdString(s); 
    s="";
    //Reset
    newt.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    std::cout << std::endl;
    
    if(!sqlDB.isEmpty()){
      //Open
      MySqlOpen(sqlHost,sqlPort,sqlUser,sqlPasswd,sqlDB);
    }else{ 
      /* Take a temp database and get databases */
      QSqlDatabase TempDB = QSqlDatabase::addDatabase("QMYSQL");
      TempDB.setHostName(sqlHost);
      TempDB.setPort(sqlPort);
      TempDB.setUserName(sqlUser);
      TempDB.setPassword(sqlPasswd);
      if(TempDB.open()){
	QSqlQuery get_list(TempDB);
	get_list.exec("show databases");
	std::cout << "List of databases:" << std::endl ;
	while(get_list.next())
	  std::cout << get_list.value(0).toString().toStdString() << std::endl ;
	get_list.clear();
	TempDB.close();
	TempDB = QSqlDatabase();
	TempDB.removeDatabase("qt_sql_default_connection");
	/* Select database by input */
	std::string userSelectedDB;
	std::cout << "Select:";
	std::cin >> userSelectedDB;
	std::cout << std::endl;
	MySqlOpen(sqlHost,sqlPort,sqlUser,sqlPasswd,QString::fromStdString(userSelectedDB));
      }else{
	qDebug() << "Error:Cannot open database  - " + DB.lastError().text();
      }
    }
    sqlPasswd="";
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
  OpenClear();
  //打開資料庫
  DB = QSqlDatabase::addDatabase("QSQLITE");
  DB.setDatabaseName(whichDB);
  if(DB.open()){
    tableListModel -> setStringList(DB.tables());
    queryEdit -> setEnabled(true);
    DBNAME=whichDB;
    setWindowTitle(tr("Sqlbrowser")+" - "+DBNAME);
  }else{
    qDebug() << "Error:Cannot open database " + whichDB + " - " + DB.lastError().text();
    queryEdit -> setEnabled(false);
  }
}


void MainWindow::MySqlOpen(QString host, int port, QString user, QString passwd, QString dbName){
  OpenClear();
  //打開資料庫
  DB = QSqlDatabase::addDatabase("QMYSQL");
  DB.setHostName(host);
  DB.setPort(port);
  DB.setUserName(user);
  DB.setPassword(passwd);
  DB.setDatabaseName(dbName);
  if(DB.open()){
    //Tables
    tableListModel -> setStringList(DB.tables());
    queryEdit -> setEnabled(true);
    DBNAME=dbName;
    setWindowTitle(tr("Sqlbrowser")+" - "+host+"_"+DBNAME);
  }else{
    qDebug() << "Error:Cannot open database " + dbName + " - " + DB.lastError().text();
    queryEdit -> setEnabled(false);
  }
}


void MainWindow::OpenClear(){
  //若已經打開一個資料庫則進行清理
  if(DB.isValid()){
    int tabsNum = tabs->count();
    for(int i=tabsNum; i >= 1; i--){
      tabs -> removeTab(i);
    }
    tableView.clear();
    tableModel.clear();
    queryList.clear();
    queryModel.clear();
    tableDirty.clear();
    tabType.clear();
    tableDirty<<false;
    tabType<<1;
    OpenedTableList.clear();
    DB.close();
    DB = QSqlDatabase();
    /* Special handle for Mysql */
    if(QSqlDatabase::contains(DBNAME)){
      DB.removeDatabase(DBNAME);
    }else{
      DB.removeDatabase("qt_sql_default_connection");
    }
    qDebug()<<"Cleaning";
    ListDebugging();
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
  tableViewa -> setMouseTracking(true);
  tableView.append(tableViewa);
  connect(tableViewa->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(CheckDeletable()) );

  //List
  tableDirty<<false;
  tabType<<0;

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
  switch(tabType[whichTab]){
  case 0: //Table
    if(tableDirty[whichTab]){
      submitAction -> setEnabled(true);
      revertAction -> setEnabled(true);
      qDebug()<<"Table "<<whichTab<<":Dirty";
    }else{
      submitAction -> setEnabled(false);
      revertAction -> setEnabled(false);
      qDebug()<<"Table "<<whichTab<<":Non-dirty";
    }
    addAction -> setEnabled(true);
    refreshAction -> setEnabled(true);
    CheckDeletable();
    break;
  case 1 ... 2 : //TableList,QueryResult
    submitAction -> setEnabled(false);
    revertAction -> setEnabled(false);
    refreshAction -> setEnabled(false);
    addAction -> setEnabled(false);
    delAction -> setEnabled(false);
    qDebug()<<"Tab "<<whichTab<<":TableList/QueryList";
    break;
  }
}


void MainWindow::tabClosing(int whichTab){
  int tType = tabType[whichTab];
  if(tType != 1){
  tableView.removeAt(whichTab-1);
  tableModel.removeAt(whichTab-1); //sb已補
  tableDirty.removeAt(whichTab);
  tabType.removeAt(whichTab);
  if(tType == 2){
    QString queryTemp = tabs->tabText(whichTab);
    int queryNum = queryTemp.remove("Query").toInt();
    qDebug()<<"Closing Query Tab "<<whichTab;
    qDebug()<<"queryTemp"<<queryTemp;
    qDebug()<<"queryNum"<<queryNum;
    qDebug()<<"queryCount"<<queryList.count();
    queryList.removeAt(queryNum);
    queryModel.removeAt(queryNum);
  }else{
    OpenedTableList.removeOne(tabs->tabText(whichTab));
  }
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


void MainWindow::RefreshCurrent(){
  int rf_index = tableIndex();
  tableModel[rf_index] -> revertAll();
  tableModel[rf_index] -> select();
//tableView[rf_index] -> setModel(tableModel[rf_index]);
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
  }else{
      delAction -> setEnabled(false);
  }
}


void MainWindow::do_query(){
  QSqlQuery Query;
  QString queryStr = queryEdit->text();
  if(!Query.exec(queryStr) ){
    qDebug()<<"Error while executing SQL command";
    qDebug()<<Query.lastError().text();
  }else{
    qDebug()<< Query.numRowsAffected() << "Row(s) affected";
    dock -> setWindowTitle(tr("SqlQuery : %n row(s) affected.", 0, Query.numRowsAffected() ));
    queryEdit -> setText("");
    tableListModel -> setStringList(DB.tables()); // Refresh Table List
    queryList.append(&Query);
    if(Query.isSelect()){
      //創建SqlQueryModel
      QSqlQueryModel *qModela;
      QSqlTableModel *sbModela;
      qModela = new QSqlQueryModel(this);
      sbModela = new QSqlTableModel(this,DB);
      qModela -> setQuery(Query);
      tableModel.append(sbModela);
      queryModel.append(qModela);
      
      //創建對應的TableView
      QTableView *qViewa;
      qViewa = new QTableView(this);
      qViewa -> setModel(qModela);
      qViewa -> resizeColumnsToContents();
      tableView.append(qViewa);

      //List
      tableDirty<<false;
      tabType<<2;

      //新加標籤
      tabs -> addTab(qViewa,"Query"+QString::number(queryList.count()-1));
      //設定工具提示
      tabs -> setTabToolTip(tabs->count()-1, queryStr);
      //設定圖標
      tabs -> setTabIcon(tabs->count()-1, QIcon::fromTheme("mail-read"));
      //轉到標籤
      tabs -> setCurrentIndex(tabs->count()-1);
    }
  }
}

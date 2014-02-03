#include <QDesktopWidget>
#include <QApplication>
#include <QVBoxLayout>
#include <QLocale>
#include <QMessageBox>
#include "hello.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent){
  //創建標籤，綁定信號
  label = new QLabel(tr("<h2><font color='red'>Hello</font> world!</h2><br /><a href='locale_msg'>View Locale</a>&nbsp;&nbsp;<a href='quit'>Quit</a>"));
  connect(label, SIGNAL(linkActivated(const QString &)), this, SLOT(LinkClicked(const QString &)) );

  //佈局處理
  QVBoxLayout *vLay = new QVBoxLayout;
  vLay -> addWidget(label);
  QWidget *cw = new QWidget;
  cw -> setLayout(vLay);
  setCentralWidget(cw);

  //大小位置
  setFixedSize(sizeHint());
  QDesktopWidget *desktop = QApplication::desktop();
  move((desktop->width() - this->width())/2, (desktop->height() - this->height())/2);
  setWindowTitle(tr("Hello world!"));
}

MainWindow::~MainWindow(){

}

void MainWindow::LinkClicked(QString href){
  if (href == "locale_msg"){
    //消息框：本地語言環境
    QMessageBox msgBox(QMessageBox::Information, QLocale::system().name(), "");
    msgBox.setText(QString(tr("Your locale:")) + QLocale::system().name() );
    msgBox.exec();
  }else{
    //向外發送退出信號
    this -> ExitSignal();
  }
}

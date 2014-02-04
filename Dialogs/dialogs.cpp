#include <QApplication>
#include <QDesktopWidget>
#include <QTextEdit>
#include <QAction>
#include <QActionGroup>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QKeySequence>
#include <QFontDialog>
#include <QColorDialog>
#include "dialogs.h"


MainWindow::MainWindow(QWidget * parent) : QMainWindow(parent)
{
  //TextEdit
  textedit = new QTextEdit(tr("Input text here"),this);
  setCentralWidget(textedit);

  //Actions
  FontDialogAction = new QAction(tr("Choose &Font..."),this);
  connect(FontDialogAction, SIGNAL(triggered()), this, SLOT(setFont()) );
  ColorDialogAction = new QAction(tr("Choose &Color..."),this);
  connect(ColorDialogAction, SIGNAL(triggered()), this, SLOT(setColor()) );
  ExitAction = new QAction(tr("&Quit..."),this);
  connect(ExitAction, SIGNAL(triggered()), this, SIGNAL(ExitSignal()) );
  //--
  LC_en_US_Action = new QAction("&English",this);
  LC_en_US_Action -> setCheckable(true);
  LC_ja_JP_Action = new QAction("日本語(&J)",this);
  LC_ja_JP_Action -> setCheckable(true);
  LC_zh_CN_Action = new QAction("简体中文(&C)",this);
  LC_zh_CN_Action -> setCheckable(true);
  LC_zh_TW_Action = new QAction("正體中文(&T)",this);
  LC_zh_TW_Action -> setCheckable(true);
  LC_Group = new QActionGroup(this);
  LC_Group -> addAction(LC_en_US_Action);
  LC_Group -> addAction(LC_ja_JP_Action);
  LC_Group -> addAction(LC_zh_CN_Action);
  LC_Group -> addAction(LC_zh_TW_Action);
  LC_Group -> setExclusive(true);
  connect(LC_Group, SIGNAL(triggered()), this, SLOT(LC_Changed()) );

  //Menus
  QMenu *menu1 = menuBar() -> addMenu(tr("&Menu"));
  menu1 -> addAction(FontDialogAction);
  menu1 -> addAction(ColorDialogAction);
  menu1 -> addAction(ExitAction);
  QMenu *menu2 = menuBar() -> addMenu(tr("&Locale"));
  menu2 -> addAction(LC_en_US_Action);
  menu2 -> addAction(LC_ja_JP_Action);
  menu2 -> addAction(LC_zh_CN_Action);
  menu2 -> addAction(LC_zh_TW_Action);

  //Size and Position
  setFixedSize(sizeHint());
  QDesktopWidget *desktop = QApplication::desktop();
  move((desktop->width() - this->width())/2, (desktop->height() - this->height())/2);
}


MainWindow::~MainWindow(){

}


void MainWindow::LC_Changed(){
  if(LC_en_US_Action -> isChecked())
    LC_Change_Signal("en_US");
  else if(LC_ja_JP_Action -> isChecked())
    LC_Change_Signal("ja_JP");
  else if(LC_zh_CN_Action -> isChecked())
    LC_Change_Signal("zh_CN");
  else if(LC_zh_TW_Action -> isChecked())
    LC_Change_Signal("zh_TW");
}


void MainWindow::setFont(){
  bool ok;
  QFont font = QFontDialog::getFont(&ok, textedit->currentFont(), this);
  if(ok){
    textedit -> setCurrentFont(font);
  }
}


void MainWindow::setColor(){
  QColor color = QColorDialog::getColor(textedit->textColor(), this);
  if(color.isValid()){
    textedit -> setTextColor(color);
  }
}

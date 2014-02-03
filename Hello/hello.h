#include <QMainWindow>
#include <QLabel>

class MainWindow : public QMainWindow
{
  Q_OBJECT
public:
  MainWindow(QWidget *parent = 0);
  ~MainWindow();
signals:
  void ExitSignal();
public slots:
  void LinkClicked(QString href);
private:
  QLabel *label;
};

#pragma once

#include <QMainWindow>

class bootloader_window : public QMainWindow
{
  Q_OBJECT

public:
  bootloader_window(QWidget * parent = 0);

private:
  void setup_window();
};

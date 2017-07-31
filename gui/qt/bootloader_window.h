#pragma once

#include <QMainWindow>

class QLineEdit;

class bootloader_window : public QMainWindow
{
  Q_OBJECT

public:
  bootloader_window(QWidget * parent = 0);

private:
  QLineEdit * filename_input;

  void setup_window();
};

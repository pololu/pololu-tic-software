#pragma once

#include <QMainWindow>

class QComboBox;
class QLabel;
class QLineEdit;
class QProgressBar;
class QPushButton;

class bootloader_window : public QMainWindow
{
  Q_OBJECT

public:
  bootloader_window(QWidget * parent = 0);

private:
  QLineEdit * filename_input;
  QPushButton * browse_button;
  QComboBox * device_chooser;
  QLabel * progress_label;
  QProgressBar * progress_bar;
  QPushButton * program_button;

  void setup_window();

private slots:
  void on_browse_button_clicked();
};

#pragma once

#include <bootloader.h>

#include <QMainWindow>

class QComboBox;
class QLabel;
class QLineEdit;
class QProgressBar;
class QPushButton;

class bootloader_window : public QMainWindow, PloaderStatusListener
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

  void show_error_message(const std::string &);

  void set_status(const char * status, uint32_t progress, uint32_t max_progress);

private slots:
  void on_browse_button_clicked();
  void on_program_button_clicked();
};

#pragma once

#include <QMainWindow>

#include <tic.hpp>

// class QCheckBox;
// class QComboBox;
class QGridLayout;
class QGroupBox;
class QLabel;
class QMainWindow;
class QPushButton;
class QSpinBox;

class main_window : public QMainWindow
{
  Q_OBJECT

public:
  main_window(QWidget *parent = 0);
  
  void show_error_message(const std::string & message);
  void show_exception(const std::exception & e,
    const std::string & context = "");
    
  void set_connection_status(const std::string & status, bool error);
  
  void set_accel_max(uint32_t accel_max);

protected:
  /** This is called by Qt just before the window is shown for the first time,
   * and is also called whenever the window becomes unminimized. */
  void showEvent(QShowEvent *) override;
  
private slots:
  void on_connect_action_triggered();
  
  /** This is called by Qt when the user wants to apply settings. */
  void on_apply_settings_action_triggered();
  
  void on_accel_max_value_valueChanged(int value);
  
private:
  bool start_event_reported = false;
  
  /* We set this to true temporarily when programmatically setting the value
   * of an input in order to suppress sending a spurious user-input event to
   * the rest of the program. */
  bool suppress_events = false;
  
  void setup_window();
  void setup_menu_bar();
  // QWidget * setupDeviceInfoBox();
  // QWidget * setupProgrammingResultsBox();
  // QWidget * setupCurrentStatusBox();
  QWidget * setup_settings_widget();
  QWidget * setup_settings_box();
  QWidget * setup_footer();
  QWidget * setup_connection_status();
  // QWidget * setupCancelChangesButton();
  QWidget * setup_defaults_button();
  QWidget * setup_apply_button();
  void retranslate();
    
  QMenuBar * menu_bar;
  QMenu * file_menu;
  QAction * exit_action;
  QMenu * device_menu;
  QAction * connect_action;
  QAction * disconnect_action;
  QAction * reload_settings_action;
  QAction * restore_defaults_action;
  QAction * apply_settings_action;
  QMenu * help_menu;
  QAction * documentation_action;
  QAction * about_action;
  
  QWidget * central_widget;
  QGridLayout * central_widget_layout;
  
  QWidget * settings_widget;
  QGridLayout * settings_widget_layout;

  // [all-settings]
  QGroupBox * settings_box;
  QGridLayout * settings_box_layout;
  QLabel * accel_max_label;
  QSpinBox * accel_max_value;
  
  QWidget * footer_widget;
  QGridLayout * footer_widget_layout;
  QLabel * connection_status_value;
  QPushButton * apply_settings_button;
  
  // todo: comment nicely
  std::vector<tic::device> device_list;
  tic::handle device_handle;
  bool connection_error = false;
  std::string connection_error_message;
  bool disconnected_by_user = false;
  
  tic::settings settings;
  
  bool settings_modified = false;
  
  void start_controller();
  bool connected() { return device_handle; }
  void connect_to_device();
  void really_connect();
  bool try_update_device_list();
  void handle_model_changed();
  void handle_device_changed();
  //void handleVariablesChanged();
  void handle_settings_changed();
  void handle_accel_max_input(uint32_t accel_max);
  void apply_settings();
};

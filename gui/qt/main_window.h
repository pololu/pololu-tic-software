#pragma once

#include <QMainWindow>

#include <tic.hpp>

// class QCheckBox;
// class QComboBox;
class QGridLayout;
class QGroupBox;
class QHBoxLayout;
class QLabel;
class QMainWindow;
class QPushButton;
class QSpinBox;
class QVBoxLayout;

class main_window : public QMainWindow
{
  Q_OBJECT

public:
  main_window(QWidget *parent = 0);
  
  void show_error_message(const std::string & message);
  void show_exception(const std::exception & e,
    const std::string & context = "");
    
  void set_connection_status(const std::string & status, bool error);
  
  void set_input_min(uint32_t input_min);
  void set_input_neutral_min(uint32_t input_neutral_min);
  void set_input_neutral_max(uint32_t input_neutral_max);
  void set_input_max(uint32_t input_max);
  void set_output_min(uint32_t output_min);
  void set_output_max(uint32_t output_max);
  void set_accel_max(uint32_t accel_max);

private:
  void set_spin_box(QSpinBox * box, int value);
  
protected:
  /** This is called by Qt just before the window is shown for the first time,
   * and is also called whenever the window becomes unminimized. */
  void showEvent(QShowEvent *) override;
  
private slots:
  void on_connect_action_triggered();
  
  /** This is called by Qt when the user wants to apply settings. */
  void on_apply_settings_action_triggered();
  
  void on_input_min_value_valueChanged(int value);
  void on_input_neutral_min_value_valueChanged(int value);
  void on_input_neutral_max_value_valueChanged(int value);
  void on_input_max_value_valueChanged(int value);
  void on_output_min_value_valueChanged(int value);
  void on_output_max_value_valueChanged(int value);
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
  QWidget * setup_motor_settings_box();
  QWidget * setup_scaling_settings_box();
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
  QVBoxLayout * settings_widget_layout;

  // [all-settings]
  QGroupBox * motor_settings_box;
  QGridLayout * motor_settings_box_layout;
  QLabel * accel_max_label;
  QSpinBox * accel_max_value;
  
  QGroupBox * scaling_settings_box;
  QGridLayout * scaling_settings_box_layout;
  QLabel * scaling_input_label;
  QLabel * scaling_target_label;
  QLabel * scaling_min_label;
  QLabel * scaling_neutral_min_label;
  QLabel * scaling_neutral_max_label;
  QLabel * scaling_max_label;
  QSpinBox * input_min_value;
  QSpinBox * input_neutral_min_value;
  QSpinBox * input_neutral_max_value;
  QSpinBox * input_max_value;
  QSpinBox * output_min_value;
  QSpinBox * output_max_value;
  
  QWidget * footer_widget;
  QHBoxLayout * footer_widget_layout;
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
  void handle_input_min_input(uint16_t input_min);
  void handle_input_neutral_min_input(uint16_t input_neutral_min);
  void handle_input_neutral_max_input(uint16_t input_neutral_max);
  void handle_input_max_input(uint16_t input_max);
  void handle_output_min_input(int32_t output_min);
  void handle_output_max_input(int32_t output_max);
  void handle_accel_max_input(uint32_t accel_max);
  void apply_settings();
};

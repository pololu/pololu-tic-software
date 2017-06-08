#pragma once

#include <QMainWindow>

// todo remove unnecessary lines
// class QCheckBox;
class QComboBox;
class QGridLayout;
class QGroupBox;
class QHBoxLayout;
class QLabel;
class QMainWindow;
class QPushButton;
class QSpinBox;
class QVBoxLayout;
class main_controller;

class main_window : public QMainWindow
{
  Q_OBJECT

public:
  main_window(QWidget *parent = 0);
  
  /** Stores a pointer to the main_controller object so that we can report events. **/
  void set_controller(main_controller * controller);
  
  void show_error_message(const std::string & message);
    
  void set_connection_status(const std::string & status, bool error);
  
  void set_control_mode(uint8_t control_mode);
  void set_input_min(uint32_t input_min);
  void set_input_neutral_min(uint32_t input_neutral_min);
  void set_input_neutral_max(uint32_t input_neutral_max);
  void set_input_max(uint32_t input_max);
  void set_output_min(uint32_t output_min);
  void set_output_max(uint32_t output_max);
  void set_speed_max(uint32_t speed_max);
  void set_speed_min(uint32_t speed_min);
  void set_accel_max(uint32_t accel_max);
  void set_decel_max(uint32_t decel_max);

private:
  /** Helper method for setting the index of a combo box, given the desired
   * uint8_t item value.  Defaults to using the first entry in the combo box if
   * the specified value is not found. */
  void set_u8_combo_box(QComboBox * combo, uint8_t value);

  void set_spin_box(QSpinBox * box, int value);
  
protected:
  /** This is called by Qt just before the window is shown for the first time,
   * and is also called whenever the window becomes unminimized. */
  void showEvent(QShowEvent *) override;
  
private slots:
  void on_connect_action_triggered();
  
  /** This is called by Qt when the user wants to apply settings. */
  void on_apply_settings_action_triggered();
  
  void on_control_mode_value_currentIndexChanged(int index);
  void on_input_min_value_valueChanged(int value);
  void on_input_neutral_min_value_valueChanged(int value);
  void on_input_neutral_max_value_valueChanged(int value);
  void on_input_max_value_valueChanged(int value);
  void on_output_min_value_valueChanged(int value);
  void on_output_max_value_valueChanged(int value);
  void on_speed_max_value_valueChanged(int value);
  void on_speed_min_value_valueChanged(int value);
  void on_accel_max_value_valueChanged(int value);
  void on_decel_max_value_valueChanged(int value);
  
private:
  bool start_event_reported = false;
  
  /* We set this to true temporarily when programmatically setting the value
   * of an input in order to suppress sending a spurious user-input event to
   * the rest of the program. */
  bool suppress_events = false;
  
  void setup_window();
  void setup_menu_bar();
  // todo remove unnecessary lines
  // QWidget * setupDeviceInfoBox();
  // QWidget * setupProgrammingResultsBox();
  // QWidget * setupCurrentStatusBox();
  QWidget * setup_settings_widget();
  QWidget * setup_control_mode_widget();
  QWidget * setup_scaling_settings_box();
  QWidget * setup_motor_settings_box();
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
  QWidget * control_mode_widget;
  QGridLayout * control_mode_widget_layout;
  QLabel * control_mode_label;
  QComboBox * control_mode_value;
  
  QGroupBox * motor_settings_box;
  QGridLayout * motor_settings_box_layout;
  QLabel * speed_max_label;
  QSpinBox * speed_max_value;
  QLabel * speed_min_label;
  QSpinBox * speed_min_value;
  QLabel * accel_max_label;
  QSpinBox * accel_max_value;
  QLabel * decel_max_label;
  QSpinBox * decel_max_value;
  
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
  
  main_controller * controller;
};
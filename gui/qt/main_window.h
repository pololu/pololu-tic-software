#pragma once

#include <array>

#include "tic.hpp"

#include "bootloader_window.h"
#include "elided_label.h"
#include "InputWizard.h"

#include <QMainWindow>

class BallScrollBar;
class QButtonGroup;
class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QFrame;
class QGridLayout;
class QGroupBox;
class QHBoxLayout;
class QLabel;
class QPushButton;
class QRadioButton;
class QShortcut;
class QSpinBox;
class QTabWidget;
class QVBoxLayout;

class current_spin_box;
class time_spin_box;
class main_controller;

struct tab_spec {
  QWidget * tab = NULL;
  QString name;
  bool hidden = false;

  tab_spec(QWidget * tab, QString name, bool hidden = false) :
    tab(tab), name(name), hidden(hidden)
  {
  }
};

struct error_row
{
  unsigned int count;
  QLabel * name_label = NULL;
  QLabel * stopping_value = NULL;
  QLabel * count_value = NULL;
  QFrame * background = NULL;
};

class pin_config_row : QObject
{
  Q_OBJECT

public:
  explicit pin_config_row(QObject * parent = Q_NULLPTR) : QObject(parent)
    {}
  explicit pin_config_row(uint8_t pin, QObject * parent = Q_NULLPTR)
    : pin(pin), QObject(parent)
    {}

  void setup(QGridLayout * layout, int row,
    const char * pullup_message = NULL, bool hide_analog = false);
  void add_funcs(uint16_t funcs);

private:
  bool window_suppress_events() const;
  void set_window_suppress_events(bool suppress_events);
  main_controller * window_controller() const;

  uint8_t pin = 255;
  QLabel * name_label = NULL;
  QComboBox * func_value = NULL;
  QCheckBox * pullup_check = NULL;
  QCheckBox * polarity_check = NULL;
  QCheckBox * analog_check = NULL;

private slots:
  void on_func_value_currentIndexChanged(int index);
  void on_pullup_check_stateChanged(int state);
  void on_polarity_check_stateChanged(int state);
  void on_analog_check_stateChanged(int state);

private:
  friend class main_window;
};

class main_window : public QMainWindow
{
  Q_OBJECT

public:
  main_window(QWidget * parent = 0);

  // Stores a pointer to the controller so we can send user input events.
  void set_controller(main_controller * controller);

  bootloader_window * open_bootloader_window();

  // This causes the window to call the controller's update() function
  // periodically, on the same thread as everything else.
  //
  // interval_ms is the amount of time between updates, in milliseconds.
  void set_update_timer_interval(uint32_t interval_ms);
  void start_update_timer();

  void show_error_message(const std::string & message);
  void show_warning_message(const std::string & message);
  void show_info_message(const std::string & message);

  // Show an OK/Cancel dialog, return true if the user selects OK.
  bool confirm(const std::string & question);

  // Like confirm() but with a warning icon.
  bool warn_and_confirm(const std::string & question);

  void set_device_list_contents(const std::vector<tic::device> & device_list);
  void set_device_list_selected(const tic::device & device);

  // Sets the label that shows the connection status/error.
  void set_connection_status(const std::string & status, bool error);

  // Changes a few controls that need to be changed depending on what product
  // (e.g. Tic T825, Tic T834) we are talking to.
  void adjust_ui_for_product(uint8_t product);

  void update_shown_tabs();

  void update_current_limit_table(uint8_t product);

  void update_current_limit_warnings();

  // Controls whether the main controls of the application are enabled or
  // disabled.
  void set_tab_pages_enabled(bool enabled);

  void set_manual_target_enabled(bool enabled);
  void set_deenergize_button_enabled(bool enabled);
  void set_resume_button_enabled(bool enabled);

  // Controls whether the apply settings action/button is enabled or
  // disabled.
  void set_apply_settings_enabled(bool enabled);

  // Controls whether the open and save settings file actions are enabled or
  // disabled.
  void set_open_save_settings_enabled(bool enabled);

  // Controls whether the disconnect action is enabled or disabled.
  void set_disconnect_enabled(bool enabled);

  // Controls whether the clear driver error option is enabled.
  void set_clear_driver_error_enabled(bool enabled);

  //  Controls whether the "Go home" actions are enabled.
  void set_go_home_enabled(bool reverse_enabled, bool forward_enabled);

  // Controls whether the reload settings from device action is enabled.
  void set_reload_settings_enabled(bool enabled);

  // Controls whether the restore defaults option is enabled.
  void set_restore_defaults_enabled(bool enabled);

  void set_device_name(const std::string & name, bool link_enabled);
  void set_serial_number(const std::string & serial_number);
  void set_firmware_version(const std::string & firmware_version);
  void set_device_reset(const std::string & device_reset);
  void set_up_time(uint32_t up_time);

  void set_encoder_position(int32_t encoder_position);
  void set_input_state(const std::string & input_state, uint8_t input_state_raw);
  void set_input_after_averaging(uint16_t input_after_averaging);
  void set_input_after_hysteresis(uint16_t input_after_hysteresis);
  void set_input_before_scaling(uint16_t input_before_scaling, uint8_t control_mode);
  void set_input_after_scaling(int32_t input_after_scaling);

  void set_vin_voltage(uint32_t vin_voltage);
  void set_operation_state(const std::string & operation_state);
  void set_energized(bool energized);
  void set_limit_active(bool forward, bool reverse);
  void disable_limit_active();
  void set_homing_active(bool active);
  void set_last_motor_driver_error(const char * str);
  void set_last_hp_driver_errors(uint8_t errors);
  void set_target_position(int32_t target_position);
  void set_target_velocity(int32_t target_velocity);
  void set_target_none();
  void set_current_position(int32_t current_position);
  void set_position_uncertain(bool position_uncertain);
  void set_current_velocity(int32_t current_velocity);

  void set_error_status(uint16_t error_status);
  void increment_errors_occurred(uint32_t errors_occurred);
  void reset_error_counts();

  void set_manual_target_position_mode();
  void set_manual_target_velocity_mode();
  void set_manual_target_range(int32_t target_min, int32_t target_max);
  void set_displayed_manual_target(int32_t target);
  void set_manual_target_ball_position(int32_t current_position, bool on_target);
  void set_manual_target_ball_velocity(int32_t current_velocity, bool on_target);

  void set_apply_settings_button_stylesheet(int offset);
  void animate_apply_settings_button();

  // [all-settings]

  void set_control_mode(uint8_t control_mode);

  void set_serial_baud_rate(uint32_t serial_baud_rate);
  void set_serial_device_number(uint16_t number);
  void set_serial_alt_device_number(uint16_t number);
  void set_serial_enable_alt_device_number(bool enable);
  void set_serial_14bit_device_number(bool enable);
  void set_command_timeout(uint16_t command_timeout);
  void set_serial_crc_for_commands(bool enable);
  void set_serial_crc_for_responses(bool enable);
  void set_serial_7bit_responses(bool enabled);
  void set_serial_response_delay(uint8_t serial_response_delay);

  void set_encoder_prescaler(uint32_t encoder_prescaler);
  void set_encoder_postscaler(uint32_t encoder_postscaler);
  void set_encoder_unlimited(bool encoder_unlimited);

  void set_input_averaging_enabled(bool input_averaging_enabled);
  void set_input_hysteresis(uint16_t input_hysteresis);

  void set_input_invert(bool input_invert);
  void set_input_min(uint16_t input_min);
  void set_input_neutral_min(uint16_t input_neutral_min);
  void set_input_neutral_max(uint16_t input_neutral_max);
  void set_input_max(uint16_t input_max);
  void set_output_min(int32_t output_min);
  void set_output_max(int32_t output_max);
  void set_input_scaling_degree(uint8_t input_scaling_degree);

  void run_input_wizard(uint8_t control_mode);

  void set_invert_motor_direction(bool invert_motor_direction);
  void set_speed_max(uint32_t speed_max);
  void set_starting_speed(uint32_t starting_speed);
  void set_accel_max(uint32_t accel_max);
  void set_decel_max(uint32_t decel_max);
  void set_decel_accel_max_same(bool decel_accel_max_same);
  void set_step_mode(uint8_t step_mode);
  void set_current_limit(uint32_t current_limit);
  void set_decay_mode(uint8_t decay_mode);
  void set_agc_mode(uint8_t);
  void set_agc_bottom_current_limit(uint8_t);
  void set_agc_current_boost_steps(uint8_t);
  void set_agc_frequency_limit(uint8_t);
  void set_hp_toff(uint8_t);
  void set_hp_tblank(uint8_t);
  void set_hp_abt(bool);
  void set_hp_tdecay(uint8_t);

  void set_soft_error_response(uint8_t soft_error_response);
  void set_soft_error_position(int32_t soft_error_position);
  void set_current_limit_during_error(int32_t current_limit_during_error);

  void set_disable_safe_start(bool disable_safe_start);
  void set_ignore_err_line_high(bool ignore_err_line_high);
  void set_auto_clear_driver_error(bool auto_clear_driver_error);
  void set_never_sleep(bool never_sleep);
  void set_hp_enable_unrestricted_current_limits(bool enable);
  void set_vin_calibration(int16_t vin_calibration);

  void set_auto_homing(bool auto_homing);
  void set_auto_homing_forward(bool forward);
  void set_homing_speed_towards(uint32_t speed);
  void set_homing_speed_away(uint32_t speed);

  void set_pin_func(uint8_t pin, uint8_t func);
  void set_pin_pullup(uint8_t pin, bool pullup, bool enabled);
  void set_pin_polarity(uint8_t pin, bool polarity, bool enabled);
  void set_pin_analog(uint8_t pin, bool analog, bool enabled);

  void set_motor_status_message(const std::string & message, bool stopped = true);

private:

  void set_combo_items(QComboBox * combo,
    std::vector<std::pair<const char *, uint32_t>> items);

  // Helper method for setting the index of a combo box, given the desired
  // uint8_t item value. Sets index of -1 for no selection if the specified
  // value is not found.
  void set_combo(QComboBox * combo, uint32_t value);
  void set_spin_box(QSpinBox * box, int value);
  void set_double_spin_box(QDoubleSpinBox * spin, double value);
  void set_check_box(QCheckBox * check, bool value);

  void update_manual_target_controls();

  void center_at_startup_if_needed();

protected:
  // This is called by Qt just before the window is shown for the first time,
  // and is also called whenever the window becomes unminimized.
  void showEvent(QShowEvent *) override;

  // This is called by Qt when the "close" slot is triggered, meaning that
  // the user wants to close the window.
  void closeEvent(QCloseEvent *) override;

private slots:
  void on_open_settings_action_triggered();
  void on_save_settings_action_triggered();
  void on_disconnect_action_triggered();
  void on_clear_driver_error_action_triggered();
  void on_go_home_reverse_action_triggered();
  void on_go_home_forward_action_triggered();
  void on_reload_settings_action_triggered();
  void on_restore_defaults_action_triggered();
  void on_update_timer_timeout();
  void on_device_name_value_linkActivated();
  void on_documentation_action_triggered();
  void on_about_action_triggered();

  void on_device_list_value_currentIndexChanged(int index);
  void on_deenergize_button_clicked();
  void on_resume_button_clicked();

  void on_errors_reset_counts_button_clicked();
  void on_manual_target_position_mode_radio_toggled(bool checked);
  void on_manual_target_scroll_bar_valueChanged(int value);
  void on_manual_target_scroll_bar_scrollingFinished();
  void on_manual_target_min_value_valueChanged(int value);
  void on_manual_target_max_value_valueChanged(int value);
  void on_manual_target_entry_value_valueChanged(int value);
  void on_manual_target_return_key_shortcut_activated();
  void on_set_target_button_clicked();
  void on_auto_set_target_check_stateChanged(int state);
  void on_auto_zero_target_check_stateChanged(int state);
  void on_set_current_position_button_clicked();
  void on_halt_button_clicked();
  void on_decelerate_button_clicked();
  void on_apply_settings_action_triggered();
  void on_upgrade_firmware_action_triggered();

  // [all-settings]

  void on_control_mode_value_currentIndexChanged(int index);

  void on_serial_baud_rate_value_valueChanged(int value);
  void on_serial_baud_rate_value_editingFinished();
  void on_serial_device_number_value_valueChanged(int value);
  void on_serial_alt_device_number_value_valueChanged(int value);
  void on_serial_enable_alt_device_number_check_stateChanged(int state);
  void on_serial_14bit_device_number_check_stateChanged(int state);
  void on_command_timeout_check_stateChanged(int state);
  void on_command_timeout_value_valueChanged(double value);
  void on_serial_crc_for_commands_check_stateChanged(int state);
  void on_serial_crc_for_responses_check_stateChanged(int state);
  void on_serial_7bit_responses_check_stateChanged(int state);
  void on_serial_response_delay_value_valueChanged(int value);

  void on_encoder_prescaler_value_valueChanged(int value);
  void on_encoder_postscaler_value_valueChanged(int value);
  void on_encoder_unlimited_check_stateChanged(int state);

  void on_input_hysteresis_value_valueChanged(int value);
  void on_input_averaging_enabled_check_stateChanged(int state);

  void on_input_learn_button_clicked();
  void on_input_invert_check_stateChanged(int state);
  void on_input_min_value_valueChanged(int value);
  void on_input_neutral_min_value_valueChanged(int value);
  void on_input_neutral_max_value_valueChanged(int value);
  void on_input_max_value_valueChanged(int value);
  void on_output_min_value_valueChanged(int value);
  void on_output_max_value_valueChanged(int value);
  void on_input_scaling_degree_value_currentIndexChanged(int index);

  void on_invert_motor_direction_check_stateChanged(int state);
  void on_speed_max_value_valueChanged(int value);
  void on_starting_speed_value_valueChanged(int value);
  void on_accel_max_value_valueChanged(int value);
  void on_decel_max_value_valueChanged(int value);
  void on_decel_accel_max_same_check_stateChanged(int state);
  void on_step_mode_value_currentIndexChanged(int index);
  void on_current_limit_value_valueChanged(int value);
  void on_decay_mode_value_currentIndexChanged(int index);
  void on_agc_mode_value_currentIndexChanged(int index);
  void on_agc_bottom_current_limit_value_currentIndexChanged(int index);
  void on_agc_current_boost_steps_value_currentIndexChanged(int index);
  void on_agc_frequency_limit_value_currentIndexChanged(int index);
  void on_hp_toff_value_valueChanged(int value);
  void on_hp_tblank_value_valueChanged(int value);
  void on_hp_abt_check_stateChanged(int state);
  void on_hp_tdecay_value_valueChanged(int value);

  void on_soft_error_response_radio_group_buttonToggled(int id, bool checked);
  void on_soft_error_position_value_valueChanged(int value);
  void on_current_limit_during_error_check_stateChanged(int state);
  void on_current_limit_during_error_value_valueChanged(int value);

  void on_disable_safe_start_check_stateChanged(int state);
  void on_ignore_err_line_high_check_stateChanged(int state);
  void on_auto_clear_driver_error_check_stateChanged(int state);
  void on_never_sleep_check_stateChanged(int state);
  void on_hp_enable_unrestricted_current_limits_check_stateChanged(int state);
  void on_vin_calibration_value_valueChanged(int value);

  void on_auto_homing_check_stateChanged(int state);
  void on_auto_homing_direction_value_currentIndexChanged(int index);
  void on_homing_speed_towards_value_valueChanged(int value);
  void on_homing_speed_away_value_valueChanged(int value);

  void upload_complete();

private:
  bool start_event_reported = false;

  // We set this to true temporarily when programmatically setting the value of
  // an input in order to suppress sending a spurious user-input event to the
  // rest of the program.
  bool suppress_events = false;

  QTimer * update_timer = NULL;

  // These are low-level functions called in the constructor that set up the
  // GUI elements.
  void setup_window();
  void setup_menu_bar();
  QLayout * setup_header();
  void add_tab(QWidget * tab, QString name, bool hidden = false);
  tab_spec & find_tab_spec(QWidget * tab);
  QWidget * setup_tab_widget();

  QWidget * setup_status_page_widget();
  QLayout * setup_status_left_column();
  QLayout * setup_status_right_column();
  QWidget * setup_device_info_box();
  QWidget * setup_input_status_box();
  QWidget * setup_operation_status_box();
  QLayout * setup_manual_target_layout();
  QWidget * setup_manual_target_box();
  QWidget * setup_manual_target_widget();
  QLayout * setup_errors_layout();
  QWidget * setup_errors_box();
  QWidget * setup_errors_widget();
  QLayout * setup_error_table_layout();

  QWidget * setup_input_motor_settings_page_widget();
  QWidget * setup_control_mode_widget();
  QWidget * setup_serial_settings_box();
  QWidget * setup_encoder_settings_box();
  QWidget * setup_conditioning_settings_box();
  QWidget * setup_scaling_settings_box();
  QWidget * setup_hp_motor_widget();
  QLayout * setup_motor_settings_layout();
  QWidget * setup_motor_settings_box();
  QWidget * setup_motor_settings_widget();

  QWidget * setup_advanced_settings_page_widget();
  QWidget * setup_pin_config_box();
  QWidget * setup_error_settings_box();
  QWidget * setup_misc_settings_box();
  QLayout * setup_homing_settings_layout();
  QWidget * setup_homing_settings_box();
  QWidget * setup_homing_settings_widget();

  QLayout * setup_footer();

  void retranslate();
  void adjust_sizes();

  QIcon program_icon;

  QString directory_hint;

  QMenuBar * menu_bar;
  QMenu * file_menu;
  QAction * open_settings_action;
  QAction * save_settings_action;
  QAction * exit_action;
  QMenu * device_menu;
  QAction * disconnect_action;
  QAction * clear_driver_error_action;
  QAction * go_home_forward_action;
  QAction * go_home_reverse_action;
  QAction * reload_settings_action;
  QAction * restore_defaults_action;
  QAction * apply_settings_action;
  QAction * upgrade_firmware_action;
  QMenu * help_menu;
  QAction * documentation_action;
  QAction * about_action;

  // True if we are using the compact layout.
  bool compact = false;

  QWidget * central_widget;
  QVBoxLayout * central_widget_layout;

  QHBoxLayout * header_layout;
  QLabel * device_list_label;
  QComboBox * device_list_value;
  QLabel * connection_status_value;

  QTabWidget * tab_widget;
  QList<tab_spec> tab_specs;

  //// status page

  QWidget * status_page_widget;
  QGridLayout * status_page_layout;

  QGroupBox * device_info_box;
  QGridLayout * device_info_box_layout;
  QLabel * device_name_label;
  QLabel * device_name_value;
  QLabel * serial_number_label;
  QLabel * serial_number_value;
  QLabel * firmware_version_label;
  QLabel * firmware_version_value;
  QLabel * device_reset_label;
  QLabel * device_reset_value;
  QLabel * up_time_label;
  QLabel * up_time_value;

  QGroupBox * input_status_box;
  QGridLayout * input_status_box_layout;
  QLabel * encoder_position_label;
  QLabel * encoder_position_value;
  QLabel * input_state_label;
  QLabel * input_state_value;
  QLabel * input_after_averaging_label;
  QLabel * input_after_averaging_value;
  QLabel * input_after_hysteresis_label;
  QLabel * input_after_hysteresis_value;
  QLabel * input_before_scaling_label;
  QLabel * input_before_scaling_value;
  QLabel * input_before_scaling_pretty;
  QLabel * input_after_scaling_label;
  QLabel * input_after_scaling_value;

  QGroupBox * operation_status_box;
  QGridLayout * operation_status_box_layout;
  QLabel * vin_voltage_label;
  QLabel * vin_voltage_value;
  QLabel * operation_state_label;
  QLabel * operation_state_value;
  QLabel * energized_label;
  QLabel * energized_value;
  QLabel * limit_active_label;
  QLabel * limit_active_value;
  QLabel * homing_active_label;
  QLabel * homing_active_value;
  QLabel * last_motor_driver_error_label;
  QLabel * last_motor_driver_error_value;
  QLabel * target_label;
  QLabel * target_value;
  QLabel * target_velocity_pretty;
  QLabel * current_position_label;
  QLabel * current_position_value;
  QLabel * position_uncertain_label;
  QLabel * position_uncertain_value;
  QLabel * current_velocity_label;
  QLabel * current_velocity_value;
  QLabel * current_velocity_pretty;

  int32_t manual_target_position_min = -200;
  int32_t manual_target_position_max = 200;
  int32_t manual_target_velocity_min = -2000000; // -200 pps
  int32_t manual_target_velocity_max = 2000000;  //  200 pps
  uint8_t cached_input_state = 0;
  int32_t cached_input_after_scaling;

  QGroupBox * manual_target_box = NULL;
  QWidget * manual_target_widget;
  QVBoxLayout * manual_target_mode_layout;

  QRadioButton * manual_target_position_mode_radio;
  QRadioButton * manual_target_velocity_mode_radio;

  BallScrollBar * manual_target_scroll_bar;
  QSpinBox * manual_target_min_value;
  QLabel * manual_target_min_pretty;
  QSpinBox * manual_target_max_value;
  QLabel * manual_target_max_pretty;
  QSpinBox * manual_target_entry_value;
  QLabel * manual_target_entry_pretty;
  QPushButton * set_target_button;
  QShortcut * manual_target_return_key_shortcut;
  QShortcut * manual_target_enter_key_shortcut;

  QCheckBox * auto_set_target_check;
  QCheckBox * auto_zero_target_check;

  QSpinBox * current_position_entry_value;
  QPushButton * set_current_position_button;
  QLabel * current_position_halts_label;

  QPushButton * halt_button;
  QPushButton * decelerate_button;

  QGroupBox * errors_box = NULL;
  QLabel * errors_stopping_header_label;
  QLabel * errors_count_header_label;
  std::array<error_row, 32> error_rows;
  QPushButton * errors_reset_counts_button;

  // [all-settings]

  //// input and motor settings page

  QWidget * input_motor_settings_page_widget;
  QGridLayout * input_motor_settings_page_layout;

  QWidget * control_mode_widget;
  QGridLayout * control_mode_widget_layout;
  QLabel * control_mode_label;
  QComboBox * control_mode_value;

  QGroupBox * serial_settings_box;
  QGridLayout * serial_settings_box_layout;
  QLabel * serial_baud_rate_label;
  QSpinBox * serial_baud_rate_value;
  QLabel * serial_device_number_label;
  QSpinBox * serial_device_number_value;
  QCheckBox * serial_enable_alt_device_number_check;
  QSpinBox * serial_alt_device_number_value;
  QCheckBox * serial_14bit_device_number_check;
  QCheckBox * command_timeout_check;
  QDoubleSpinBox * command_timeout_value;
  QCheckBox * serial_crc_for_commands_check;
  QCheckBox * serial_crc_for_responses_check;
  QCheckBox * serial_7bit_responses_check;
  QLabel * serial_response_delay_label;
  QSpinBox * serial_response_delay_value;

  QGroupBox * encoder_settings_box;
  QGridLayout * encoder_settings_box_layout;
  QLabel * encoder_prescaler_label;
  QSpinBox * encoder_prescaler_value;
  QLabel * encoder_postscaler_label;
  QSpinBox * encoder_postscaler_value;
  QCheckBox * encoder_unlimited_check;

  QGroupBox * conditioning_settings_box;
  QGridLayout * conditioning_settings_box_layout;
  QCheckBox * input_averaging_enabled_check;
  QLabel * input_hysteresis_label;
  QSpinBox * input_hysteresis_value;

  QGroupBox * scaling_settings_box;
  QGridLayout * scaling_settings_box_layout;
  QPushButton * input_learn_button;
  QCheckBox * input_invert_check;
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
  QLabel * input_scaling_degree_label;
  QComboBox * input_scaling_degree_value;

  InputWizard * input_wizard;

  QGroupBox * motor_settings_box = NULL;
  QCheckBox * invert_motor_direction_check;
  QLabel * speed_max_label;
  QSpinBox * speed_max_value;
  QLabel * speed_max_value_pretty;
  QLabel * starting_speed_label;
  QSpinBox * starting_speed_value;
  QLabel * starting_speed_value_pretty;
  QLabel * accel_max_label;
  QSpinBox * accel_max_value;
  QLabel * accel_max_value_pretty;
  QLabel * decel_max_label;
  QSpinBox * decel_max_value;
  QLabel * decel_max_value_pretty;
  QCheckBox * decel_accel_max_same_check;
  QLabel * step_mode_label;
  QComboBox * step_mode_value;
  QLabel * current_limit_label;
  current_spin_box * current_limit_value;
  QLabel * current_limit_warning_label;
  QLabel * decay_mode_label;
  QComboBox * decay_mode_value;
  QLabel * agc_mode_label;
  QComboBox * agc_mode_value;
  QLabel * agc_bottom_current_limit_label;
  QComboBox * agc_bottom_current_limit_value;
  QLabel * agc_current_boost_steps_label;
  QComboBox * agc_current_boost_steps_value;
  QLabel * agc_frequency_limit_label;
  QComboBox * agc_frequency_limit_value;
  QWidget * hp_motor_widget;
  QLabel * hp_toff_label;
  time_spin_box * hp_toff_value;
  QLabel * hp_tblank_label;
  time_spin_box * hp_tblank_value;
  QCheckBox * hp_abt_check;
  QLabel * hp_tdecay_label;
  time_spin_box * hp_tdecay_value;


  //// advanced settings page

  QWidget * advanced_settings_page_widget;
  QGridLayout * advanced_settings_page_layout;

  QGroupBox * pin_config_box;
  QGridLayout * pin_config_box_layout;
  std::array<pin_config_row *, 5> pin_config_rows;

  QGroupBox * error_settings_box;
  QGridLayout * error_settings_box_layout;
  QButtonGroup * soft_error_response_radio_group;
  QSpinBox * soft_error_position_value;
  QCheckBox * current_limit_during_error_check;
  current_spin_box * current_limit_during_error_value;
  QLabel * current_limit_during_error_warning_label;

  QGroupBox * misc_settings_box;
  QCheckBox * disable_safe_start_check;
  QCheckBox * ignore_err_line_high_check;
  QCheckBox * auto_clear_driver_error_check;
  QCheckBox * never_sleep_check;
  QCheckBox * hp_enable_unrestricted_current_limits_check;
  QLabel * vin_calibration_label;
  QSpinBox * vin_calibration_value;

  QGroupBox * homing_settings_box = nullptr;
  QLabel * auto_homing_label;
  QCheckBox * auto_homing_check;
  QLabel * auto_homing_direction_label;
  QComboBox * auto_homing_direction_value;
  QLabel * homing_speed_towards_label;
  QSpinBox * homing_speed_towards_value;
  QLabel * homing_speed_towards_value_pretty;
  QLabel * homing_speed_away_label;
  QSpinBox * homing_speed_away_value;
  QLabel * homing_speed_away_value_pretty;

  //// end of pages

  QHBoxLayout * footer_layout;
  QPushButton * deenergize_button;
  QPushButton * resume_button;
  elided_label * motor_status_value;
  QLabel * apply_settings_label;
  QPushButton * apply_settings_button;
  uint32_t apply_settings_animation_count = 0;

  main_controller * controller;

  friend class pin_config_row;
};

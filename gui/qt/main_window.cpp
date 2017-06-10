#include "main_window.h"
#include "main_controller.h"

#include "tic.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollBar>
#include <QSpinBox>
#include <QTimer>
#include <QVBoxLayout>

#include <cassert>

main_window::main_window(QWidget *parent)
  : QMainWindow(parent)
{
  setup_window();
}

void main_window::set_controller(main_controller * controller)
{
  this->controller = controller;
}

void main_window::start_update_timer(uint32_t interval_ms)
{
  assert(interval_ms <= std::numeric_limits<int>::max());
  update_timer->start(interval_ms);
}

void main_window::show_error_message(const std::string & message)
{
  QMessageBox mbox(QMessageBox::Critical, windowTitle(),
    QString(message.c_str()));
  mbox.exec();
}

void main_window::show_warning_message(const std::string & message)
{
  QMessageBox mbox(QMessageBox::Warning, windowTitle(),
    QString(message.c_str()));
  mbox.exec();
}

void main_window::show_info_message(const std::string & message)
{
  QMessageBox mbox(QMessageBox::Information, windowTitle(),
    QString(message.c_str()));
  mbox.exec();
}

bool main_window::confirm(const std::string & question)
{
  QMessageBox mbox(QMessageBox::Question, windowTitle(),
    QString(question.c_str()), QMessageBox::Ok | QMessageBox::Cancel);
  int button = mbox.exec();
  return button == QMessageBox::Ok;
}

void main_window::set_connection_status(const std::string & status, bool error)
{
  if (error)
  {
    connection_status_value->setStyleSheet("QLabel { color: red; }");
  }
  else
  {
    connection_status_value->setStyleSheet("");
  }
  connection_status_value->setText(QString(status.c_str()));
}

void main_window::set_device_name(const std::string & name, bool link_enabled)
{
  QString text = QString(name.c_str());
  if (link_enabled)
  {
      text = "<a href=\"#doc\">" + text + "</a>";
  }

  device_name_value->setText(text);
}

void main_window::set_serial_number(const std::string & serial_number)
{
  serial_number_value->setText(QString(serial_number.c_str()));
}

void main_window::set_firmware_version(const std::string & firmware_version)
{
  firmware_version_value->setText(QString(firmware_version.c_str()));
}

void main_window::set_control_mode(uint8_t control_mode)
{
  set_u8_combo_box(control_mode_value, control_mode);
}

void main_window::set_input_min(uint32_t input_min)
{
  set_spin_box(input_min_value, input_min);
}

void main_window::set_input_neutral_min(uint32_t input_neutral_min)
{
  set_spin_box(input_neutral_min_value, input_neutral_min);
}

void main_window::set_input_neutral_max(uint32_t input_neutral_max)
{
  set_spin_box(input_neutral_max_value, input_neutral_max);
}

void main_window::set_input_max(uint32_t input_max)
{
  set_spin_box(input_max_value, input_max);
}

void main_window::set_output_min(uint32_t output_min)
{
  set_spin_box(output_min_value, output_min);
}

void main_window::set_output_max(uint32_t output_max)
{
  set_spin_box(output_max_value, output_max);
}

void main_window::set_speed_max(uint32_t speed_max)
{
  set_spin_box(speed_max_value, speed_max);
}

void main_window::set_speed_min(uint32_t speed_min)
{
  set_spin_box(speed_min_value, speed_min);
}

void main_window::set_accel_max(uint32_t accel_max)
{
  set_spin_box(accel_max_value, accel_max);
}

void main_window::set_decel_max(uint32_t decel_max)
{
  set_spin_box(decel_max_value, decel_max);
}

void main_window::set_step_mode(uint8_t step_mode)
{
  set_u8_combo_box(step_mode_value, step_mode);
}

void main_window::set_current_limit(uint32_t current_limit)
{
  set_spin_box(current_limit_value, current_limit);
}

void main_window::set_decay_mode(uint8_t decay_mode)
{
  set_u8_combo_box(decay_mode_value, decay_mode);
}

void main_window::set_u8_combo_box(QComboBox * combo, uint8_t value)
{
    int index = 0;
    for (int i = 0; i < combo->count(); i++)
    {
        if (value == combo->itemData(i).toUInt())
        {
            index = i;
            break;
        }
    }
    suppress_events = true;
    combo->setCurrentIndex(index);
    suppress_events = false;
}

void main_window::set_spin_box(QSpinBox * spin, int value)
{
  suppress_events = true;
  spin->setValue(value);
  suppress_events = false;
}

void main_window::showEvent(QShowEvent * event)
{
  Q_UNUSED(event);
  if (!start_event_reported)
  {
    start_event_reported = true;
    controller->start();
  }
}

void main_window::on_connect_action_triggered()
{
  controller->connect_device();
}

void main_window::on_disconnect_action_triggered()
{
   controller->disconnect_device();
}

void main_window::on_update_timer_timeout()
{
    controller->update();
}

void main_window::on_apply_settings_action_triggered()
{
  controller->apply_settings();
}

void main_window::on_target_position_mode_radio_toggled(bool checked)
{
  update_target_box(checked);
}

void main_window::on_set_target_button_clicked()
{
  if (target_position_mode)
  {
    controller->set_target_position(target_numeric_value->value());
  }
  else
  {
    controller->set_target_velocity(target_numeric_value->value());
  }
}

void main_window::on_control_mode_value_currentIndexChanged(int index)
{
  if (suppress_events) { return; }
  uint8_t control_mode = control_mode_value->itemData(index).toUInt();
  controller->handle_control_mode_input(control_mode);
}

void main_window::on_input_min_value_editingFinished()
{
  if (suppress_events) { return; }
  controller->handle_input_min_input(input_min_value->value());
}

void main_window::on_input_neutral_min_value_editingFinished()
{
  if (suppress_events) { return; }
  controller->handle_input_neutral_min_input(input_neutral_min_value->value());
}

void main_window::on_input_neutral_max_value_editingFinished()
{
  if (suppress_events) { return; }
  controller->handle_input_neutral_max_input(input_neutral_max_value->value());
}

void main_window::on_input_max_value_editingFinished()
{
  if (suppress_events) { return; }
  controller->handle_input_max_input(input_max_value->value());
}

void main_window::on_output_min_value_editingFinished()
{
  if (suppress_events) { return; }
  controller->handle_output_min_input(output_min_value->value());
}

void main_window::on_output_max_value_editingFinished()
{
  if (suppress_events) { return; }
  controller->handle_output_max_input(output_max_value->value());
}

void main_window::on_speed_max_value_editingFinished()
{
  if (suppress_events) { return; }
  controller->handle_speed_max_input(speed_max_value->value());
}

void main_window::on_speed_min_value_editingFinished()
{
  if (suppress_events) { return; }
  controller->handle_speed_min_input(speed_min_value->value());
}

void main_window::on_accel_max_value_editingFinished()
{
  if (suppress_events) { return; }
  controller->handle_accel_max_input(accel_max_value->value());
}

void main_window::on_decel_max_value_editingFinished()
{
  if (suppress_events) { return; }
  controller->handle_decel_max_input(decel_max_value->value());
}

void main_window::on_step_mode_value_currentIndexChanged(int index)
{
  if (suppress_events) { return; }
  uint8_t step_mode = step_mode_value->itemData(index).toUInt();
  controller->handle_step_mode_input(step_mode);
}

void main_window::on_current_limit_value_editingFinished()
{
  if (suppress_events) { return; }
  controller->handle_current_limit_input(current_limit_value->value());
}

void main_window::on_decay_mode_value_currentIndexChanged(int index)
{
  if (suppress_events) { return; }
  uint8_t decay_mode = decay_mode_value->itemData(index).toUInt();
  controller->handle_decay_mode_input(decay_mode);
}

// On Mac OS X, field labels are usually right-aligned.
#ifdef __APPLE__
#define FIELD_LABEL_ALIGNMENT Qt::AlignRight
#else
#define FIELD_LABEL_ALIGNMENT Qt::AlignLeft
#endif

static void setup_read_only_text_field(QGridLayout * layout, int row,
  QLabel ** label, QLabel ** value)
{
  QLabel * new_value = new QLabel();
  new_value->setTextInteractionFlags(Qt::TextSelectableByMouse);

  QLabel * new_label = new QLabel();
  new_label->setBuddy(new_value);

  layout->addWidget(new_label, row, 0, FIELD_LABEL_ALIGNMENT);
  layout->addWidget(new_value, row, 1);

  if (label) { *label = new_label; }
  if (value) { *value = new_value; }
}

void main_window::setup_window()
{
  setup_menu_bar();
  
  central_widget = new QWidget();
  QGridLayout * layout = central_widget_layout = new QGridLayout();
  
  int row = 0;
  // todo: will require rework if we add more columns
  layout->addLayout(setup_left_column(), row, 0);
  layout->addLayout(setup_right_column(), row++, 1);
  layout->addLayout(setup_footer(), row++, 0, 1, 2);
  
  central_widget->setLayout(layout);
  setCentralWidget(central_widget);
  
  retranslate();
  
  // Make the window non-resizable.
  setFixedSize(sizeHint());
  
  program_icon = QIcon(":app_icon");
  setWindowIcon(program_icon);
  
  update_timer = new QTimer(this);
  update_timer->setObjectName("update_timer");
  
  QMetaObject::connectSlotsByName(this);
}

void main_window::setup_menu_bar()
{
  menu_bar = new QMenuBar();
      
  file_menu = menu_bar->addMenu("");

  exit_action = new QAction(this);
  exit_action->setObjectName("exit_action");
  connect(exit_action, SIGNAL(triggered()), this, SLOT(close()));
  file_menu->addAction(exit_action);

  device_menu = menu_bar->addMenu("");

  connect_action = new QAction(this);
  connect_action->setObjectName("connect_action");
  connect_action->setShortcut(Qt::CTRL + Qt::Key_N);
  device_menu->addAction(connect_action);

  disconnect_action = new QAction(this);
  disconnect_action->setObjectName("disconnect_action");
  disconnect_action->setShortcut(Qt::CTRL + Qt::Key_D);
  device_menu->addAction(disconnect_action);

  device_menu->addSeparator();

  reload_settings_action = new QAction(this);
  reload_settings_action->setObjectName("reload_settings_action");
  device_menu->addAction(reload_settings_action);

  restore_defaults_action = new QAction(this);
  restore_defaults_action->setObjectName("restore_defaults_action");
  device_menu->addAction(restore_defaults_action);

  apply_settings_action = new QAction(this);
  apply_settings_action->setObjectName("apply_settings_action");
  apply_settings_action->setShortcut(Qt::CTRL + Qt::Key_P);
  device_menu->addAction(apply_settings_action);

  help_menu = menu_bar->addMenu("");

  documentation_action = new QAction(this);
  documentation_action->setObjectName("documentation_action");
  documentation_action->setShortcut(QKeySequence::HelpContents);
  help_menu->addAction(documentation_action);

  about_action = new QAction(this);
  about_action->setObjectName("about_action");
  about_action->setShortcut(QKeySequence::WhatsThis);
  help_menu->addAction(about_action);

  setMenuBar(menu_bar);
}

QLayout * main_window::setup_left_column()
{
  QVBoxLayout * layout = left_column_layout = new QVBoxLayout();

  layout->addWidget(setup_device_info_box());
  layout->addWidget(setup_status_box());
  layout->addWidget(setup_control_mode_widget());
  layout->addWidget(setup_target_box());
  
  return left_column_layout;
}

QLayout * main_window::setup_right_column()
{
  QVBoxLayout * layout = right_column_layout = new QVBoxLayout();
  
  layout->addWidget(setup_scaling_settings_box());
  layout->addWidget(setup_motor_settings_box());
  layout->addStretch(1);
  
  return right_column_layout;
}

QWidget * main_window::setup_device_info_box()
{
  device_info_box = new QGroupBox();
  QGridLayout * layout = device_info_box_layout = new QGridLayout();
  layout->setColumnStretch(1, 1);
  int row = 0;
  
  setup_read_only_text_field(layout, row++, &device_name_label, &device_name_value);
  device_name_value->setObjectName("device_name_value");
  device_name_value->setTextInteractionFlags(Qt::TextBrowserInteraction);
  
  setup_read_only_text_field(layout, row++, &serial_number_label, &serial_number_value);
  setup_read_only_text_field(layout, row++, &firmware_version_label, &firmware_version_value);
  
  // Make the right column wide enough to display the name of the Tic,
  // which should be the widest thing that needs to fit in that column.
  // This is important for making sure that the sizeHint of the overall main
  // window has a good width before we set the window to be a fixed size.
  {
    QLabel tmpLabel;
    tmpLabel.setText("Tic USB Stepper Motor Controller TXXXXX");
    layout->setColumnMinimumWidth(1, tmpLabel.sizeHint().width());
  }
  
  device_info_box->setLayout(layout);
  return device_info_box;
}

QWidget * main_window::setup_status_box()
{
  status_box = new QGroupBox();
  QGridLayout * layout = status_box_layout = new QGridLayout();
  layout->setColumnStretch(1, 1);
  int row = 0;
  
  status_box->setLayout(layout);
  return status_box;
}

QWidget * main_window::setup_target_box()
{
  target_box = new QGroupBox();
  QVBoxLayout * layout = target_box_layout = new QVBoxLayout();
  
  target_scrollbar = new QScrollBar(Qt::Horizontal);
  target_scrollbar->setObjectName("target_scrollbar");
  set_target_button = new QPushButton();
  set_target_button->setObjectName("set_target_button");
  auto_set_target_checkbox = new QCheckBox();
  auto_zero_target_checkbox = new QCheckBox();
  layout->addLayout(setup_target_mode_layout());
  layout->addSpacing(central_widget->fontMetrics().height());
  layout->addWidget(target_scrollbar);
  layout->addLayout(setup_target_range_layout());
  layout->addWidget(set_target_button, 0, Qt::AlignCenter);
  layout->addSpacing(central_widget->fontMetrics().height());
  layout->addWidget(auto_set_target_checkbox, 0, Qt::AlignLeft);
  layout->addWidget(auto_zero_target_checkbox, 0, Qt::AlignLeft);
  
  target_box->setLayout(layout);
  return target_box;
}

QLayout * main_window::setup_target_mode_layout()
{
  QHBoxLayout * layout = target_mode_layout = new QHBoxLayout();
  
  target_position_mode_radio = new QRadioButton();
  target_position_mode_radio->setObjectName("target_position_mode_radio");
  target_position_mode_radio->setChecked(true);
  target_speed_mode_radio = new QRadioButton();
  target_speed_mode_radio->setObjectName("target_speed_mode_radio");
  layout->addStretch(1);
  layout->addWidget(target_position_mode_radio);
  layout->addSpacing(central_widget->fontMetrics().height());
  layout->addWidget(target_speed_mode_radio);
  layout->addStretch(1);
  
  return target_mode_layout;
}

QLayout * main_window::setup_target_range_layout()
{
  QHBoxLayout * layout = target_range_layout = new QHBoxLayout();

  target_min_label = new QLabel();
  target_max_label = new QLabel();
  target_numeric_value = new QSpinBox();
  target_numeric_value->setObjectName("target_numeric_value");
  target_numeric_value->setRange(-0x7FFFFFF, 0x7FFFFFF);
  layout->addWidget(target_min_label);
  layout->addWidget(target_numeric_value, 1, Qt::AlignCenter);
  layout->addWidget(target_max_label);
  
  return target_range_layout;
}

// [all-settings]

QWidget * main_window::setup_control_mode_widget()
{
  control_mode_widget = new QWidget();
  QGridLayout * layout = control_mode_widget_layout = new QGridLayout();
  layout->setColumnStretch(1, 1);
  int row = 0;
  
  {
    control_mode_value = new QComboBox();
    control_mode_value->setObjectName("control_mode_value");
    control_mode_value->addItem("Serial\u2009/\u2009I\u00B2C\u2009/\u2009USB", TIC_CONTROL_MODE_SERIAL);
    control_mode_value->addItem("RC position", TIC_CONTROL_MODE_RC_POSITION);
    control_mode_value->addItem("RC speed", TIC_CONTROL_MODE_RC_SPEED);
    control_mode_value->addItem("Analog position", TIC_CONTROL_MODE_ANALOG_POSITION);
    control_mode_value->addItem("Analog speed", TIC_CONTROL_MODE_ANALOG_SPEED);
    control_mode_label = new QLabel();
    control_mode_label->setBuddy(control_mode_value);
    layout->addWidget(control_mode_label, row, 0, FIELD_LABEL_ALIGNMENT);
    layout->addWidget(control_mode_value, row, 1, Qt::AlignLeft);
    row++;
  }
  
  //layout->setRowStretch(row, 1);

  control_mode_widget->setLayout(layout);
  return control_mode_widget;
}

QWidget * main_window::setup_scaling_settings_box()
{
  scaling_settings_box = new QGroupBox();
  QGridLayout * layout = scaling_settings_box_layout = new QGridLayout();
  layout->setColumnStretch(2, 1);
  int row = 0;
  
  {
    scaling_input_label = new QLabel();
    scaling_target_label = new QLabel();
    layout->addWidget(scaling_input_label, row, 1, Qt::AlignLeft);
    layout->addWidget(scaling_target_label, row, 2, Qt::AlignLeft);
    row++;
  }
  
  {
    scaling_max_label = new QLabel();
    input_max_value = new QSpinBox();
    input_max_value->setObjectName("input_max_value");
    input_max_value->setRange(0, 0xFFFF);
    output_max_value = new QSpinBox();
    output_max_value->setObjectName("output_max_value");
    output_max_value->setRange(0, 0x7FFFFFF);
    layout->addWidget(scaling_max_label, row, 0, FIELD_LABEL_ALIGNMENT);
    layout->addWidget(input_max_value, row, 1, Qt::AlignLeft);
    layout->addWidget(output_max_value, row, 2, Qt::AlignLeft);
    row++;
  }

  {
    scaling_neutral_max_label = new QLabel();
    input_neutral_max_value = new QSpinBox();
    input_neutral_max_value->setObjectName("input_neutral_max_value");
    input_neutral_max_value->setRange(0, 0xFFFF);
    layout->addWidget(scaling_neutral_max_label, row, 0, FIELD_LABEL_ALIGNMENT);
    layout->addWidget(input_neutral_max_value, row, 1, Qt::AlignLeft);
    row++;
  }

  {
    scaling_neutral_min_label = new QLabel();
    input_neutral_min_value = new QSpinBox();
    input_neutral_min_value->setObjectName("input_neutral_min_value");
    input_neutral_min_value->setRange(0, 0xFFFF);
    layout->addWidget(scaling_neutral_min_label, row, 0, FIELD_LABEL_ALIGNMENT);
    layout->addWidget(input_neutral_min_value, row, 1, Qt::AlignLeft);
    row++;
  }
  
  {
    scaling_min_label = new QLabel();
    input_min_value = new QSpinBox();
    input_min_value->setObjectName("input_min_value");
    input_min_value->setRange(0, 0xFFFF);
    output_min_value = new QSpinBox();
    output_min_value->setObjectName("output_min_value");
    output_min_value->setRange(-0x7FFFFFF, 0);
    layout->addWidget(scaling_min_label, row, 0, FIELD_LABEL_ALIGNMENT);
    layout->addWidget(input_min_value, row, 1, Qt::AlignLeft);
    layout->addWidget(output_min_value, row, 2, Qt::AlignLeft);
    row++;
  }
  
  //layout->setRowStretch(row, 1);

  scaling_settings_box->setLayout(layout);
  return scaling_settings_box;
}

QWidget * main_window::setup_motor_settings_box()
{
  motor_settings_box = new QGroupBox();
  QGridLayout * layout = motor_settings_box_layout = new QGridLayout();
  layout->setColumnStretch(1, 1);
  int row = 0;

  {
    speed_max_value = new QSpinBox();
    speed_max_value->setObjectName("speed_max_value");
    speed_max_value->setRange(0, TIC_MAX_ALLOWED_SPEED);
    speed_max_label = new QLabel();
    speed_max_label->setBuddy(speed_max_value);
    layout->addWidget(speed_max_label, row, 0, FIELD_LABEL_ALIGNMENT);
    layout->addWidget(speed_max_value, row, 1, Qt::AlignLeft);
    row++;
  }
  
  {
    speed_min_value = new QSpinBox();
    speed_min_value->setObjectName("speed_min_value");
    speed_min_value->setRange(0, TIC_MAX_ALLOWED_SPEED);
    speed_min_label = new QLabel();
    speed_min_label->setBuddy(speed_min_value);
    layout->addWidget(speed_min_label, row, 0, FIELD_LABEL_ALIGNMENT);
    layout->addWidget(speed_min_value, row, 1, Qt::AlignLeft);
    row++;
  }
  
  {
    accel_max_value = new QSpinBox();
    accel_max_value->setObjectName("accel_max_value");
    accel_max_value->setRange(TIC_MIN_ALLOWED_ACCEL, TIC_MAX_ALLOWED_ACCEL);
    accel_max_label = new QLabel();
    accel_max_label->setBuddy(accel_max_value);
    layout->addWidget(accel_max_label, row, 0, FIELD_LABEL_ALIGNMENT);
    layout->addWidget(accel_max_value, row, 1, Qt::AlignLeft);
    row++;
  }
  
  {
    decel_max_value = new QSpinBox();
    decel_max_value->setObjectName("decel_max_value");
    decel_max_value->setRange(0, TIC_MAX_ALLOWED_ACCEL);
    decel_max_label = new QLabel();
    decel_max_label->setBuddy(decel_max_value);
    layout->addWidget(decel_max_label, row, 0, FIELD_LABEL_ALIGNMENT);
    layout->addWidget(decel_max_value, row, 1, Qt::AlignLeft);
    row++;
  }
  
  {
    step_mode_value = new QComboBox();
    step_mode_value->setObjectName("step_mode_value");
    step_mode_value->addItem("Full step", TIC_STEP_MODE_MICROSTEP1);
    step_mode_value->addItem("1/2 step", TIC_STEP_MODE_MICROSTEP2);
    step_mode_value->addItem("1/4 step", TIC_STEP_MODE_MICROSTEP4);
    step_mode_value->addItem("1/8 step", TIC_STEP_MODE_MICROSTEP8);
    step_mode_value->addItem("1/16 step", TIC_STEP_MODE_MICROSTEP16);
    step_mode_value->addItem("1/32 step", TIC_STEP_MODE_MICROSTEP32);
    step_mode_label = new QLabel();
    step_mode_label->setBuddy(step_mode_value);
    layout->addWidget(step_mode_label, row, 0, FIELD_LABEL_ALIGNMENT);
    layout->addWidget(step_mode_value, row, 1, Qt::AlignLeft);
    row++;
  }
  
  {
    current_limit_value = new QSpinBox();
    current_limit_value->setObjectName("current_limit_value");
    current_limit_value->setRange(0, 4000);
    current_limit_value->setSuffix(" mA");
    current_limit_label = new QLabel();
    current_limit_label->setBuddy(current_limit_value);
    layout->addWidget(current_limit_label, row, 0, FIELD_LABEL_ALIGNMENT);
    layout->addWidget(current_limit_value, row, 1, Qt::AlignLeft);
    row++;
  }
  
  {
    decay_mode_value = new QComboBox();
    decay_mode_value->setObjectName("decay_mode_value");
    decay_mode_value->addItem("Mixed", TIC_DECAY_MODE_MIXED);
    decay_mode_value->addItem("Slow", TIC_DECAY_MODE_SLOW);
    decay_mode_value->addItem("Fast", TIC_DECAY_MODE_FAST);
    decay_mode_label = new QLabel();
    decay_mode_label->setBuddy(decay_mode_value);
    layout->addWidget(decay_mode_label, row, 0, FIELD_LABEL_ALIGNMENT);
    layout->addWidget(decay_mode_value, row, 1, Qt::AlignLeft);
    row++;
  }
  
  //layout->setRowStretch(row, 1);

  motor_settings_box->setLayout(layout);
  return motor_settings_box;
}

QLayout * main_window::setup_footer()
{
  //footer_widget = new QWidget();
  QHBoxLayout * layout = footer_widget_layout = new QHBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);

  layout->addWidget(setup_connection_status(), 0, Qt::AlignLeft);
  layout->addWidget(setup_apply_button(), 0, Qt::AlignRight);
    
  //footer_widget->setLayout(layout);
  return footer_widget_layout;
}

QWidget * main_window::setup_connection_status()
{
  connection_status_value = new QLabel();
  return connection_status_value;
}

QWidget * main_window::setup_apply_button()
{
  apply_settings_button = new QPushButton();

  connect(apply_settings_button, SIGNAL(clicked()),
    this, SLOT(on_apply_settings_action_triggered()));
  return apply_settings_button;
}

void main_window::retranslate()
{
  setWindowTitle(tr("Pololu Tic Configuration Utility"));

  file_menu->setTitle(tr("&File"));
  exit_action->setText(tr("E&xit"));
  device_menu->setTitle(tr("&Device"));
  connect_action->setText(tr("&Connect"));
  disconnect_action->setText(tr("&Disconnect"));
  reload_settings_action->setText(tr("Re&load settings from device"));
  restore_defaults_action->setText(tr("&Restore default settings"));
  apply_settings_action->setText(tr("&Apply settings"));
  help_menu->setTitle(tr("&Help"));
  documentation_action->setText(tr("&Online documentation..."));
  about_action->setText(tr("&About..."));

  device_info_box->setTitle(tr("Device info"));
  device_name_label->setText(tr("Name:"));
  serial_number_label->setText(tr("Serial number:"));
  firmware_version_label->setText(tr("Firmware version:"));
    
  status_box->setTitle(tr("Status"));
  
  target_box->setTitle(tr("Set target (Serial\u2009/\u2009I\u00B2C\u2009/\u2009USB mode only)"));
  target_position_mode_radio->setText(tr("Set position"));
  target_speed_mode_radio->setText(tr("Set speed"));
  update_target_box(target_position_mode_radio->isChecked());
  auto_set_target_checkbox->setText(tr("Set target when slider or entry box are changed"));
  auto_zero_target_checkbox->setText(tr("Return slider to zero when it is released"));
  
  // [all-settings]
  control_mode_label->setText(tr("Control mode:"));
  
  scaling_settings_box->setTitle(tr("Scaling settings"));
  scaling_input_label->setText(tr("Input"));
  scaling_target_label->setText(tr("Target"));
  scaling_min_label->setText(tr("Minimum:"));
  scaling_neutral_min_label->setText(tr("Neutral min:"));
  scaling_neutral_max_label->setText(tr("Neutral max:"));
  scaling_max_label->setText(tr("Maximum:"));
  
  motor_settings_box->setTitle(tr("Motor settings"));
  speed_max_label->setText(tr("Speed max:"));
  speed_min_label->setText(tr("Speed min:"));
  accel_max_label->setText(tr("Acceleration max:"));
  decel_max_label->setText(tr("Deceleration max:"));
  step_mode_label->setText(tr("Step mode:"));
  current_limit_label->setText(tr("Current limit:"));
  decay_mode_label->setText(tr("Decay mode:"));

  // cancelChangesButton->setText("Cancel Changes"); // TODO: use same name as menu item
  // defaultsButton->setText("Defaults"); // TODO: use same name as menu item
  apply_settings_button->setText(apply_settings_action->text());
}

void main_window::update_target_box(bool position_mode)
{
  target_position_mode = position_mode;
  
  if (target_position_mode)
  {
    set_target_button->setText(tr("Set position"));
  }
  else
  {
    set_target_button->setText(tr("Set speed"));
  }
  //target_min_label->setText();
  //target_max_label->setText();
}

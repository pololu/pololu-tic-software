#include "main_window.h"
#include "main_controller.h"

#include "tic.hpp"

#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
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

void main_window::on_apply_settings_action_triggered()
{
  controller->apply_settings();
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

// On Mac OS X, field labels are usually right-aligned.
#ifdef __APPLE__
#define FIELD_LABEL_ALIGNMENT Qt::AlignRight
#else
#define FIELD_LABEL_ALIGNMENT Qt::AlignLeft
#endif

void main_window::setup_window()
{
  setup_menu_bar();
  
  central_widget = new QWidget();
  QGridLayout * layout = central_widget_layout = new QGridLayout();
  
  int row = 0;
  // todo: will require rework if we add more columns
  layout->addWidget(setup_settings_widget(), row++, 0, 0);
  layout->addWidget(setup_footer(), row++, 0, 0);
  
  central_widget->setLayout(layout);
  setCentralWidget(central_widget);
  
  retranslate();
  
  // Make the window non-resizable.
  setFixedSize(sizeHint());
  
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
  // todo: connect by name?
  //connect(connect_action, SIGNAL(triggered()),
  //  this, SLOT(on_connect_action_triggered()));
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

QWidget * main_window::setup_settings_widget()
{
  settings_widget = new QWidget();
  QVBoxLayout * layout = settings_widget_layout = new QVBoxLayout();
  
  layout->addWidget(setup_control_mode_widget());
  layout->addWidget(setup_scaling_settings_box());
  layout->addWidget(setup_motor_settings_box());
  
  settings_widget->setLayout(layout);
  return settings_widget;
}

// [all-settings]

QWidget * main_window::setup_control_mode_widget()
{
  control_mode_widget = new QWidget();
  QGridLayout * layout = control_mode_widget_layout = new QGridLayout();
  //layout->setColumnStretch(1, 1);
  int row = 0;
  
  {
    control_mode_value = new QComboBox();
    control_mode_value->setObjectName("control_mode_value");
    control_mode_value->addItem("Serial/I\u00B2C/USB", TIC_CONTROL_MODE_SERIAL);
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
  //layout->setColumnStretch(1, 1);
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
  //layout->setColumnStretch(1, 1);
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
  
  //layout->setRowStretch(row, 1);

  motor_settings_box->setLayout(layout);
  return motor_settings_box;
}

QWidget * main_window::setup_footer()
{
  footer_widget = new QWidget();
  QHBoxLayout * layout = footer_widget_layout = new QHBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);

  layout->addWidget(setup_connection_status(), 0, Qt::AlignLeft);
  layout->addWidget(setup_apply_button(), 0, Qt::AlignRight);
    
  footer_widget->setLayout(layout);
  return footer_widget;
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
  reload_settings_action->setText(tr("Re&load Settings from Device"));
  restore_defaults_action->setText(tr("&Restore Default Settings"));
  apply_settings_action->setText(tr("&Apply Settings"));
  help_menu->setTitle(tr("&Help"));
  documentation_action->setText(tr("&Online Documentation..."));
  about_action->setText(tr("&About..."));

  // [all-settings]
  control_mode_label->setText(tr("Control mode:"));
  
  scaling_settings_box->setTitle(tr("Scaling settings"));
  scaling_input_label->setText(tr("Input"));
  scaling_target_label->setText(tr("Target"));
  scaling_min_label->setText(tr("Minimum:"));
  scaling_neutral_min_label->setText(tr("Neutral Min:"));
  scaling_neutral_max_label->setText(tr("Neutral Max:"));
  scaling_max_label->setText(tr("Maximum:"));
  
  motor_settings_box->setTitle(tr("Motor settings"));
  speed_max_label->setText(tr("Max. speed:"));
  speed_min_label->setText(tr("Min. speed:"));
  accel_max_label->setText(tr("Max. acceleration:"));
  decel_max_label->setText(tr("Max. deceleration:"));

  // cancelChangesButton->setText("Cancel Changes"); // TODO: use same name as menu item
  // defaultsButton->setText("Defaults"); // TODO: use same name as menu item
  apply_settings_button->setText(apply_settings_action->text());
}

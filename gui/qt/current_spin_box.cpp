#include "nice_spin_box.h"
#include "main_controller.h"

#include <QLineEdit>
#include <QSpinBox>
#include <QtMath>
#include <QRegExpValidator>
#include <QDebug>
#include <iostream> //tmphax

nice_spin_box::nice_spin_box(int index, QWidget* parent)
  : index(index), QDoubleSpinBox(parent)
{
  connect(this, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &editing_finished);

  setRange(-1, 10000);
  setKeyboardTracking(false);
  setDecimals(3);
}

void nice_spin_box::set_controller(main_controller * controller)
{
  this->controller = controller;
}

void nice_spin_box::editing_finished(double entered_value)
{
  if (suppress_events) { return; }

  if (entered_value != 0)
    for (int i = 1; i < map_values.size() - 1; ++i)
    {
      if (entered_value >= map_values.at(i) && entered_value < map_values.at(i + 1))
      {
        current_index = mapping.key(map_values.at(i));
      }
    }

  if (entered_value == 0)
  {
    while (mapping.value(current_index) != 0)
      current_index--;
  }

  if (entered_value > mapping.values().last())
  {
    current_index = mapping.size() - 1;
  }

  set_code();
}

void nice_spin_box::set_display_value()
{
  suppress_events = true;
  setValue(mapping[current_index]);
  suppress_events = false;
}

void nice_spin_box::set_possible_values(uint16_t value)
{
  mapping.clear();
  mapping.insert(0, (controller->get_current_limit_value(0)/1000));

  for (int i = 1; i < 96; i++)
  {
    double display_value = (controller->get_current_limit_value(i)/1000);

    mapping.insert(i, display_value);
  }

  map_values = mapping.values();

  std::sort(map_values.begin(), map_values.end());

  current_index = value;

  set_display_value();
}

void nice_spin_box::stepBy(int step_value)
{
  if (mapping.values().at(qBound(0, (current_index + step_value),
    mapping.size() - 1)) == value())
  {
    current_index += (step_value * 2);
  }
  else
    current_index += step_value;

  current_index = qBound(0, current_index, mapping.size() - 1);

  setValue(mapping[current_index]);
}

QDoubleSpinBox::StepEnabled nice_spin_box::stepEnabled()
{
  StepEnabled enabled = StepUpEnabled | StepDownEnabled;
  if (qBound(0, current_index, mapping.size() - 1) == 0)
  {
    enabled ^= StepDownEnabled;
  }
  if (qBound(0, current_index, mapping.size() - 1) == mapping.size() - 1)
  {
    enabled ^= StepUpEnabled;
  }
  return enabled;
}

void nice_spin_box::set_code()
{
  if (suppress_events) { return; }
  controller->handle_current_limit_amps_spinbox_input(index, current_index);
}

double nice_spin_box::valueFromText(const QString& text) const
{
  QString copy = text.toUpper();
  if (copy.contains("M"))
  {
    copy.remove(QRegExp("(M|MA)"));
    double temp_num = copy.toDouble();
    temp_num /=1000;
    return temp_num;
  }
  return copy.toDouble();
}

QString nice_spin_box::textFromValue(double val) const
{
  return QString::number(val, 'f', 2);
}

QValidator::State nice_spin_box::validate(QString& input, int& pos) const
{
  QRegExp r = QRegExp("(\\d{0,6})(\\.\\d{0,4})?(\\s*)(m|ma|Ma|mA|MA)?");

  if (r.exactMatch(input))
  {
    return QValidator::Acceptable;
  }
  else
  {
    return QValidator::Invalid;
  }
}

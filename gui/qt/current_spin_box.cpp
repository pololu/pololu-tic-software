#include "current_spin_box.h"

#include <QLineEdit>
#include <QSpinBox>
#include <QtMath>
#include <QRegExpValidator>
#include <QDebug>
#include <iostream> //tmphax

current_spin_box::current_spin_box(QWidget* parent)
  : QDoubleSpinBox(parent)
{
  connect(this, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
    this, &editing_finished);

  setRange(-1, 10000);
  setKeyboardTracking(false);
  setDecimals(3);

  mapping = new QMultiMap<double, int>();
}

void current_spin_box::editing_finished(double entered_value)
{
  if (entered_value != 0)
    for (int i = 1; i < step_map.size() - 1; ++i)
    {
      if (entered_value >= step_map.values().at(i) && entered_value < step_map.values().at(i + 1))
      {
        current_index = mapping->value(step_map.values().at(i));
      }
    }

  if (entered_value == 0)
  {
    current_index = 0;
  }

  if (entered_value > step_map.values().last())
  {
    current_index = mapping->size() - 1;
  }

  set_display_value();

  emit send_code(current_index);
}

void current_spin_box::set_display_value()
{
  setValue(mapping->key(current_index));
}

void current_spin_box::set_possible_values(uint16_t value)
{
  step_map.clear();
  for (int i = 0; i < mapping->size(); i++)
  {
    step_map.insert(i, mapping->keys().at(i));
  }

  current_index = value;

  step_index = step_map.key(mapping->key(current_index));

  set_display_value();
}

void current_spin_box::stepBy(int step_value)
{
  if (step_map.values().at(qBound(0, (step_index + step_value),
    step_map.size() - 1)) == value())
  {
    step_index += (step_value * 2);
  }
  else
    step_index += step_value;

  current_index = mapping->value(step_map.value(step_index));

  set_display_value();
}

QDoubleSpinBox::StepEnabled current_spin_box::stepEnabled()
{
  StepEnabled enabled = StepUpEnabled | StepDownEnabled;
  if (qBound(0, current_index, mapping->size() - 1) == 0)
  {
    enabled ^= StepDownEnabled;
  }
  if (qBound(0, current_index, mapping->size() - 1) == step_map.size() - 1)
  {
    enabled ^= StepUpEnabled;
  }
  return enabled;
}

double current_spin_box::valueFromText(const QString& text) const
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

QString current_spin_box::textFromValue(double val) const
{
  return QString::number(val, 'f', 2);
}

QValidator::State current_spin_box::validate(QString& input, int& pos) const
{
  // TODO: allow it to end with just "M"
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

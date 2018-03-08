#pragma once

#include <QDoubleSpinBox>

class current_spin_box : public QDoubleSpinBox
{
  Q_OBJECT

public:
  current_spin_box(QWidget* parent = Q_NULLPTR);

  void set_possible_values(uint16_t value);

  QMap<double, int> * mapping;

signals:
  void send_code(uint16_t index);

private slots:
  void editing_finished(double entered_value);
  void set_display_value();

private:
  QMultiMap<int, double> step_map;
  int step_index = 0;
  int current_index = 0;
  int index;
  bool suppress_events = false;
  double entered_value = -1;
  QList<double> map_values;

protected:
  // Reimplemented QDoubleSpinBox functions
  virtual void stepBy(int step_value);
  virtual StepEnabled stepEnabled();
  double valueFromText(const QString& text) const;
  QString textFromValue(double val) const;
  QValidator::State validate(QString& input, int& pos) const;
};


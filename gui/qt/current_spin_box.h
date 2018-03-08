#pragma once

#include <QDoubleSpinBox>

class current_spin_box : public QDoubleSpinBox
{
  Q_OBJECT

public:
  current_spin_box(QWidget * parent = Q_NULLPTR);

  void set_possible_values(uint16_t value);

  QMap<double, int> * mapping;

signals:
  void send_code(uint16_t index);

private slots:
  void editing_finished();
  void set_display_value();

private:
  QMultiMap<int, double> step_map;  // TODO: QList<double> current_levels;
  int step_index = 0;
  int current_index = 0;
  QList<double> map_values;

protected:
  virtual void stepBy(int step_value);
  virtual StepEnabled stepEnabled();
  double valueFromText(const QString & text) const;
  QValidator::State validate(QString & input, int & pos) const;
};


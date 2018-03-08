#pragma once

#include <QDoubleSpinBox>
#include <QMap>

class nice_spin_box : public QDoubleSpinBox
{
  Q_OBJECT

public:
  nice_spin_box(int index = -1, QWidget* parent = Q_NULLPTR);

  void set_mapping(const QMap<int, double> &);

private slots:
  void editing_finished(double entered_value);
  void set_display_value();
  void set_code();

private:
  QMap<int, double> mapping;
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

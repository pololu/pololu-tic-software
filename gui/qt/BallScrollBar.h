#pragma once

#include <QScrollBar>

class BallScrollBar : public QScrollBar
{
  Q_OBJECT
  Q_PROPERTY(bool ballVisible READ ballVisible WRITE setBallVisible)
  Q_PROPERTY(QColor ballColor READ ballColor WRITE setBallColor)
  Q_PROPERTY(int ballValue READ ballValue WRITE setBallValue)

public:
  using QScrollBar::QScrollBar;

public Q_SLOTS:
  void setBallVisible(bool visible);
  bool ballVisible()                      { return _ballVisible; }

  void setBallColor(QColor const & color);
  QColor const & ballColor()              { return _ballColor; }

  void setBallValue(int value);
  int ballValue()                         { return _ballValue; }

protected:
  void paintEvent(QPaintEvent *) override;

private:
  bool _ballVisible = false;
  QColor _ballColor = Qt::black;
  int _ballValue = 0;
};
#include "BallScrollBar.h"

#include <QPainter>
#include <QPaintEvent>
#include <QStyleOptionSlider>

void BallScrollBar::setBallVisible(bool visible)
{
  _ballVisible = visible;
  update();
}

void BallScrollBar::setBallColor(QColor const & color)
{
  _ballColor = color;
  update();
}

void BallScrollBar::setBallValue(int value)
{
  _ballValue = value;
  update();
}

void BallScrollBar::paintEvent(QPaintEvent * event)
{
  QScrollBar::paintEvent(event);

  if (isEnabled() && _ballVisible &&
    (_ballValue >= minimum()) && (_ballValue <= maximum()))
  {
    QStyleOptionSlider opt;
    initStyleOption(&opt);

    QRect groove_rect = style()->subControlRect(QStyle::CC_ScrollBar, &opt,
      QStyle::SC_ScrollBarGroove, this);
    QRect slider_rect = style()->subControlRect(QStyle::CC_ScrollBar, &opt,
      QStyle::SC_ScrollBarSlider, this);

    int span, length_offset, thickness_offset, radius;
    QPainter painter(this);

    if (orientation() == Qt::Horizontal)
    {
      span = groove_rect.width() - slider_rect.width();
      length_offset = groove_rect.x() + slider_rect.width() / 2;
      thickness_offset = height() / 2;
      radius = height() / 4;
    }
    else
    {
      span = groove_rect.height() - slider_rect.height();
      length_offset = groove_rect.y() + slider_rect.height() / 2;
      thickness_offset = width() / 2;
      radius = width() / 4;

      painter.rotate(90);
      painter.scale(1, -1);
    }
    int pos = QStyle::sliderPositionFromValue(minimum(), maximum(),
      _ballValue, span);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(_ballColor);
    painter.translate(length_offset, thickness_offset);
    painter.drawEllipse(QPoint(pos, 0), radius, radius);
  }
}
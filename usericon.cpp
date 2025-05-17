#include "usericon.h"
#include <QPainter>
#include <QPaintEvent>

UserIcon::UserIcon(QWidget *parent)
    : QWidget{parent}
{
    setFixedSize(40, 40);
}

void UserIcon::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    p.setBrush(QColor("#cccccc"));
    p.setPen(Qt::NoPen);
    p.drawEllipse(rect());

    p.setBrush(Qt::white);
    p.drawEllipse(12,8,16,16);
    p.drawRoundedRect(8, 22, 24, 14, 10, 10);
}

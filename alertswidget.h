#ifndef ALERTSWIDGET_H
#define ALERTSWIDGET_H

#include <QWidget>

class AlertsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AlertsWidget(QWidget *parent = nullptr);

signals:
    void goHomeRequested(); // Сигнал для возврата на главный экран
};

#endif // ALERTSWIDGET_H

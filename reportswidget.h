#ifndef REPORTSWIDGET_H
#define REPORTSWIDGET_H

#include <QWidget>

class ReportsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ReportsWidget(QWidget *parent = nullptr);

signals:
    void goHomeRequested(); // Сигнал для возврата на главный экран
};

#endif // REPORTSWIDGET_H

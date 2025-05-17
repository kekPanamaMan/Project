#include "alertswidget.h"

#include <QVBoxLayout>
#include <Qlabel>
#include <QPushButton>
#include <QListWidget>

AlertsWidget::AlertsWidget(QWidget *parent)
    : QWidget{parent}
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Заголовок
    QLabel *title = new QLabel("Оповещения", this);
    title->setStyleSheet("font-size: 20px; font-weight: bold;");
    layout->addWidget(title);

    // Текст с информацией (заглушка)
    QListWidget *list = new QListWidget(this);
    list->addItems({
        "🚨 Угроза протестов в Европе",
        "🔥 Пожар на складе в Азии",
        "📉 Снижение ВВП на 2.1%"
    });
    layout->addWidget(list);

    // Кнопка "Закрыть"
    QPushButton *closeButton = new QPushButton("Закрыть", this);

    layout->addWidget(closeButton);

    connect(closeButton, &QPushButton::clicked, this, [this]() {
        emit goHomeRequested();
    });

    setLayout(layout);
}

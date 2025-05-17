#include "reportswidget.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>

ReportsWidget::ReportsWidget(QWidget *parent)
    : QWidget{parent}
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Заголовок
    QLabel *title = new QLabel("Отчёты", this);
    title->setStyleSheet("font-size: 20px; font-weight: bold;");
    layout->addWidget(title);

    // Текст с информацией (заглушка)
    QTextEdit *text = new QTextEdit(this);
    text->setReadOnly(true);
    text->setText(
        "Отчёт за неделю:\n"
        "- Инфляция выросла на 0.6%\n"
        "- Политические риски в регионе X усилились\n"
        "- Геоэкономическая нестабильность влияет на рынок"
        );
    layout->addWidget(text);

    // Кнопка "Закрыть"
    QPushButton *closeButton = new QPushButton("Закрыть", this);

    layout->addWidget(closeButton);

    connect(closeButton, &QPushButton::clicked, this, [this]() {
        emit goHomeRequested();
    });

    setLayout(layout);
}

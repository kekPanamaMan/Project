#include "loginwindow.h"
#include "mainwindow.h"
#include <QPushButton>
#include <QVBoxLayout>

loginwindow::loginwindow(QWidget *parent)
    : QWidget{parent}
{
    setWindowTitle("Вход в систему");
    resize(300, 150);

    auto *layout = new QVBoxLayout(this);

    auto *loginButton = new QPushButton("Войти");
    layout->addWidget(loginButton);

    connect(loginButton, &QPushButton::clicked, this, [this]() {
        auto *mainWin = new MainWindow();
        mainWin->show();
        this->close(); // Закрываем окно входа
    });
}

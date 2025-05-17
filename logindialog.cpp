#include "logindialog.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QSettings>

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Вход в RiskGuard");
    setFixedSize(360, 200);

    auto *layout = new QVBoxLayout(this);
    auto *label = new QLabel("Введите данные для входа:");
    usernameEdit = new QLineEdit;
    usernameEdit->setPlaceholderText("Логин");

    passwordEdit = new QLineEdit;
    passwordEdit->setPlaceholderText("Пароль");
    passwordEdit->setEchoMode(QLineEdit::Password);

    loginButton = new QPushButton("Войти");

    layout->addWidget(label);
    layout->addWidget(usernameEdit);
    layout->addWidget(passwordEdit);
    layout->addWidget(loginButton);

    // Загрузка сохранённого логина
    QSettings settings("RiskGuardCompany", "RiskGuardApp");
    QString savedLogin = settings.value("login/username").toString();
    usernameEdit->setText(savedLogin);

    connect(loginButton, &QPushButton::clicked, this, [this]() {
        const QString login = usernameEdit->text().trimmed();
        const QString password = passwordEdit->text();

        if (login == "admin" && password == "1234") {
            // Сохраняем логин
            QSettings settings("RiskGuardCompany", "RiskGuardApp");
            settings.setValue("login/username", login);

            accept(); // вход успешен
        } else {
            QMessageBox::warning(this, "Ошибка входа", "Неверный логин или пароль!");
        }
    });
}

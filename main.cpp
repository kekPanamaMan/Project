#include "mainwindow.h"
#include "logindialog.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setWindowIcon(QIcon("/Users/nikita/Documents/PSTU/Проект/RiskGuard/MyIcon.png"));

    LoginDialog login;
    if (login.exec() != QDialog::Accepted) {
        return 0; // закрыть приложение, если вход отменен
    }

    // --- Подключение QSS-файл ---
    QFile file("theme_light.qss"); // или "theme_dark.qss", если файл в ресурсах
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        a.setStyleSheet(styleSheet); // или qApp->setStyleSheet(styleSheet);
    }

    QApplication::setStyle("Fusion");
    QFont font("Inter", 11);
    QApplication::setFont(font);

    MainWindow w;
    w.show();
    return a.exec();
}

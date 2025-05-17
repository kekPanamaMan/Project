#include "settingsdialog.h"

#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Настройки");
    setFixedSize(350, 220);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *title = new QLabel("<h2>Настройки приложения</h2>");

    QLabel *themeLabel = new QLabel("🎨 <b>Тема оформления:</b>");
    themeComboBox = new QComboBox(this);
    themeComboBox->addItems({"Светлая", "Тёмная"});

    QPushButton *applyBtn = new QPushButton("Применить");
    QPushButton *closeBtn = new QPushButton("Закрыть");

    connect(applyBtn, &QPushButton::clicked, this, [=]() {
        QString selected = themeComboBox->currentText();
        QString file = selected == "Светлая" ? ":/theme_light.qss" : ":/theme_dark.qss";
        emit themeChanged(file);
    });

    connect(closeBtn, &QPushButton::clicked, this, &SettingsDialog::accept);

    layout->addWidget(title);
    layout->addSpacing(10);
    layout->addWidget(themeLabel);
    layout->addWidget(themeComboBox);
    layout->addSpacing(15);
    layout->addWidget(applyBtn);
    layout->addWidget(closeBtn);
}

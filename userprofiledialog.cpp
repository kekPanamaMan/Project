#include "userprofiledialog.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

UserProfileDialog::UserProfileDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Профиль пользователя");
    setFixedSize(350, 250);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *title = new QLabel("<h2>Профиль пользователя</h2>");
    QLabel *nameLabel = new QLabel("👤 <b>Имя:</b> Иван Иванов");
    QLabel *emailLabel = new QLabel("✉️ <b>Email:</b> ivan@example.com");
    QLabel *roleLabel = new QLabel("🛠 <b>Роль:</b> Аналитик рисков");
    QLabel *orgLabel = new QLabel("🏢 <b>Организация:</b> RiskGuard");

    QPushButton *closeBtn = new QPushButton("Закрыть");
    connect(closeBtn, &QPushButton::clicked, this, &UserProfileDialog::accept);

    layout->addWidget(title);
    layout->addSpacing(10);
    layout->addWidget(nameLabel);
    layout->addWidget(emailLabel);
    layout->addWidget(roleLabel);
    layout->addWidget(orgLabel);
    layout->addStretch();
    layout->addWidget(closeBtn);
}

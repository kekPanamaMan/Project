#include "eventanalysisdialog.h"

#include <QGraphicsDropShadowEffect>

// Конструктор диалогового окна выбора типа анализа
EventAnalysisDialog::EventAnalysisDialog(QWidget *parent)
    : QDialog(parent)
{
    // Установка заголовка окна и базовых параметров
    setWindowTitle("Выбор типа анализа");
    setModal(true);
    setFixedSize(400, 270);

    // Добавление теневого эффекта под диалог
    auto *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(24);
    shadow->setOffset(0, 8);
    shadow->setColor(QColor(80,120,255,40));
    this->setGraphicsEffect(shadow);


    QLabel *label = new QLabel("Выберите тип анализа:", this);
    label->setStyleSheet("font-size:18px; font-weight:600; margin-bottom:14px;");

    // Кнопка выбора макроэкономического анализа
    macroButton = new QPushButton(QIcon(":/icons/chart_modern.svg"), "Макроэкономический анализ", this);
    macroButton->setIconSize(QSize(64, 64));
    macroButton->setMinimumHeight(80);
    macroButton->setStyleSheet(R"(
    QPushButton {
        font-size: 18px;
        font-weight: 600;
        text-align: left;
        padding: 18px 24px;
        border-radius: 18px;
        background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #4f8cff, stop:1 #00e0c6);
        color: white;
        margin-bottom: 18px;
        box-shadow: 0 4px 24px rgba(80,120,255,0.13);
    }
    QPushButton:hover {
        background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #1976d2, stop:1 #00e0c6);
        color: #fff;
    }
)");

    // Кнопка выбора событийного анализа
    eventRiskButton = new QPushButton(QIcon(":/icons/bell_modern.svg"), "Событийный анализ (Event Risk)", this);
    eventRiskButton->setIconSize(QSize(64, 64));
    eventRiskButton->setMinimumHeight(80);
    eventRiskButton->setStyleSheet(R"(
    QPushButton {
        font-size: 18px;
        font-weight: 600;
        text-align: left;
        padding: 18px 24px;
        border-radius: 18px;
        background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #ff4d67, stop:1 #ffb731);
        color: white;
        margin-bottom: 18px;
        box-shadow: 0 4px 24px rgba(255,77,103,0.13);
    }
    QPushButton:hover {
        background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #d32f2f, stop:1 #f7b731);
        color: #fff;
    }
)");

    // Основной вертикальный макет диалога
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(label);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(macroButton);
    mainLayout->addWidget(eventRiskButton);
    mainLayout->addStretch(); // добавляет пространство между кнопками и нижним тулбаром

    // Нижний тулбар
    QToolBar *footerToolbar = new QToolBar(this);
    footerToolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    footerToolbar->setMovable(false);

    QAction *homeAction = new QAction(QIcon(":/icons/home_modern.svg"), "Главная", this);
    footerToolbar->addAction(homeAction);

    // Сигнал нажатия на Главную
    connect(homeAction, &QAction::triggered, this, &EventAnalysisDialog::goHome);

    mainLayout->addWidget(footerToolbar);

    connect(macroButton, &QPushButton::clicked, this, &EventAnalysisDialog::chooseMacro);
    connect(eventRiskButton, &QPushButton::clicked, this, &EventAnalysisDialog::chooseEventRisk);
}

void EventAnalysisDialog::chooseMacro() { // Метод, вызываемый при выборе макроэкономического анализа
    m_selected = Macro;
    accept();
}

void EventAnalysisDialog::chooseEventRisk() { // Метод для событийного анализа
    m_selected = EventRisk;
    accept();
}

void EventAnalysisDialog::goHome() { // Метод, вызываемый при возврате на главный экран
    m_selected = None;
    reject();
}

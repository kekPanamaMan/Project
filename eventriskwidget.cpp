#include "eventriskwidget.h"

#include <QMessageBox>
#include <QTime>
#include <QStringList>
#include <QListWidgetItem>
#include <QMenu>
#include <QToolButton>
#include <QVBoxLayout>

// Конструктор виджета событийного анализа
EventRiskWidget::EventRiskWidget(QWidget *parent) : QWidget(parent) {
    setupUI();

    // Автообновление данных каждые 3 часа
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &EventRiskWidget::fetchEvents);
    timer->start(3 * 60 * 60 * 1000);

    fetchEvents();
}

void EventRiskWidget::setupUI() {

    // Фильтры
    typeCombo = new QComboBox;
    typeCombo->addItems({"Все типы", "Политика 🌐", "Экономика 💰", "Природа 🌪", "Социальное 🔥"});

    threatLevelCombo = new QComboBox;
    threatLevelCombo->addItems({"Любой уровень", "Низкий", "Средний", "Высокий"});

    countryCombo = new QComboBox;
    countryCombo->addItems({"Все страны", "Украина", "Россия", "США", "Китай", "ЕС"});

    // Layout для фильтров
    QVBoxLayout *filterLayout = new QVBoxLayout;
    filterLayout->addWidget(new QLabel("Тип события:"));
    filterLayout->addWidget(typeCombo);
    filterLayout->addWidget(new QLabel("Уровень угрозы:"));
    filterLayout->addWidget(threatLevelCombo);
    filterLayout->addWidget(new QLabel("География:"));
    filterLayout->addWidget(countryCombo);
    filterLayout->addStretch();

    QGroupBox *filterBox = new QGroupBox("Фильтры"); // группируем фильтры
    filterBox->setLayout(filterLayout);

    // Карта
    mapWidget = new WorldMapWidget(this);
    mapWidget->setMinimumSize(480, 320);
    mapWidget->setStyleSheet("background:#eaf2ff; border:1px solid #4f8cff; border-radius:12px; font-size:16px;");

    // Обработка клика на стране на карте: обновляет события по стране
    connect(mapWidget, &WorldMapWidget::countryEventsChanged, this, [this](const QString& country, const QList<CountryEvent>& events){
        eventList->clear();
        for (const auto& ev : events) {
            eventList->addItem(QString("%1 (уровень: %2)").arg(ev.description, ev.riskLevel));
        }
    });

    // Лента событий
    eventList = new QListWidget;
    eventList->setSelectionMode(QAbstractItemView::MultiSelection);

    // Кнопки экспорта
    exportPdfBtn = new QPushButton("Экспорт в PDF");
    exportExcelBtn = new QPushButton("Экспорт в Excel");

    // Правая панель (события + экспорт)
    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(new QLabel("События (хронология):"));
    rightLayout->addWidget(eventList, 1);
    QHBoxLayout *exportLayout = new QHBoxLayout;
    exportLayout->addWidget(exportPdfBtn);
    exportLayout->addWidget(exportExcelBtn);
    rightLayout->addLayout(exportLayout);

    // Левая панель (фильтры + карта)
    QSplitter *splitter = new QSplitter;
    QWidget *leftPanel = new QWidget;
    QVBoxLayout *leftPanelLayout = new QVBoxLayout(leftPanel);
    leftPanelLayout->addWidget(filterBox);
    leftPanelLayout->addWidget(mapWidget, 1);
    splitter->addWidget(leftPanel);

    QWidget *rightPanel = new QWidget;
    rightPanel->setLayout(rightLayout);
    splitter->addWidget(rightPanel);

    // Сигналы фильтров
    connect(typeCombo, &QComboBox::currentTextChanged, this, &EventRiskWidget::updateFilters);
    connect(threatLevelCombo, &QComboBox::currentTextChanged, this, &EventRiskWidget::updateFilters);
    connect(countryCombo, &QComboBox::currentTextChanged, this, &EventRiskWidget::updateFilters);

    // Нажатие по событию
    connect(eventList, &QListWidget::itemClicked, [this](QListWidgetItem *item){
        int id = item->data(Qt::UserRole).toInt();
        for (const Event &ev : events) {
            if (ev.id == id) {
                showEventPopup(ev); // отображение окна с деталями
                break;
            }
        }
    });
    connect(exportPdfBtn, &QPushButton::clicked, this, &EventRiskWidget::exportToPDF);
    connect(exportExcelBtn, &QPushButton::clicked, this, &EventRiskWidget::exportToExcel);

    // Тулбар (нижняя навигация)
    QToolBar *footerToolbar = new QToolBar(this);
    footerToolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    footerToolbar->setMovable(false);

    QAction *homeAction = new QAction(QIcon(":/icons/home_modern.svg"), "Главная", this);
    footerToolbar->addAction(homeAction);
    QAction *analyticsAction = new QAction(QIcon(":/icons/chart_modern.svg"), "Аналитика", this);
    footerToolbar->addAction(analyticsAction);

    QAction *alertsAction = new QAction(QIcon(":/icons/bell_modern.svg"), "Оповещения", this);
    footerToolbar->addAction(alertsAction);

    QAction *reportsAction = new QAction(QIcon(":/icons/report_modern.svg"), "Отчёты", this);
    footerToolbar->addAction(reportsAction);

    // Меню настроек
    QMenu *settingsMenu = new QMenu(this);
    QAction *lightThemeAction = new QAction(QIcon(":/icons/theme_light.svg"), "Светлая тема", this);
    QAction *darkThemeAction  = new QAction(QIcon(":/icons/theme_dark.svg"),  "Тёмная тема", this);
    settingsMenu->addAction(lightThemeAction);
    settingsMenu->addAction(darkThemeAction);

    QToolButton *settingsButton = new QToolButton(this);
    settingsButton->setIcon(QIcon(":/icons/settings_modern.svg"));
    settingsButton->setText("Настройки");
    settingsButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    settingsButton->setPopupMode(QToolButton::InstantPopup);
    settingsButton->setMenu(settingsMenu);

    footerToolbar->addWidget(static_cast<QWidget*>(settingsButton));

    // --- Layout ---
    QVBoxLayout *mainLayout = new QVBoxLayout(this); // объявляем ОДИН раз!

    // --- Кнопка "На главную" ---
    QPushButton *homeButton = new QPushButton("🏠 На главную");
    homeButton->setStyleSheet("padding: 8px 16px; background-color: #4f8cff; color: white; border-radius: 8px;");
    connect(homeButton, &QPushButton::clicked, this, [this]() {
        emit goHome();
    });

    mainLayout->addWidget(homeButton, 0, Qt::AlignLeft);
    mainLayout->addWidget(splitter, 1);
    setLayout(mainLayout);

    // --- Сигналы ---
    connect(homeAction, &QAction::triggered, this, [this](){ emit goHome(); });
}

void EventRiskWidget::fetchEvents() { // Получение данных о событиях
    // Здесь интеграция с API новостей и базами рисков (заглушка)
    events.clear();

    // Пример события
    events.append(Event{
        1,
        "Экономика 💰",
        "Высокий",
        "Украина",
        "Риск disruptions поставок газа в ЕС — влияние на инфляцию +1.2%",
        QStringList() << "Санкции" << "Газ",
        1.2,
        {50.45, 30.52},
        "red"
    });
    events.append(Event{
        2,
        "Природа 🌪",
        "Средний",
        "США",
        "Ураган в Техасе — возможные перебои с поставками нефти",
        QStringList() << "Ураган" << "Нефть",
        0.7,
        {29.76, -95.36},
        "yellow"
    });

    updateFilters(); // фильтрация после загрузки

    // Обновим уровень риска
    int riskValue = 72;
    int riskChange = 4;
    QString description = "Риски возросли из-за природных и экономических событий";
    emit updateRisk(riskValue, riskChange, description);
}

void EventRiskWidget::updateFilters() {
    // Фильтрация событий по выбранным параметрам
    QString type = typeCombo->currentText();
    QString threat = threatLevelCombo->currentText();
    QString country = countryCombo->currentText();

    QList<Event> filtered;
    for (const Event &ev : events) {
        if ((type == "Все типы" || ev.type == type) &&
            (threat == "Любой уровень" || ev.threatLevel == threat) &&
            (country == "Все страны" || ev.country == country)) {
            filtered.append(ev);
        }
    }

    // Обновить карту и ленту событий
    updateMap();
    updateEventList(filtered);
}

void EventRiskWidget::updateMap() {
    // Для WorldMapWidget используйте свой метод для отображения статуса
    if (mapWidget)
        mapWidget->setStatusText("Обновлено: " + QTime::currentTime().toString("HH:mm:ss"));
}

void EventRiskWidget::updateEventList(const QList<Event> &filtered) { // Обновление списка событий (ленты)
    eventList->clear();
    for (const Event &ev : filtered) {
        QString txt = QString("%1 [%2] %3\n%4\nВлияние: %5%")
                          .arg(ev.type, ev.threatLevel, ev.country, ev.description)
                          .arg(ev.impactPercent, 0, 'f', 2);
        QListWidgetItem *item = new QListWidgetItem(txt);
        item->setData(Qt::UserRole, ev.id);
        // Цветовая иконка
        if (ev.markerColor == "red")
            item->setIcon(QIcon(":/icons/red_dot.png"));
        else if (ev.markerColor == "yellow")
            item->setIcon(QIcon(":/icons/yellow_dot.png"));
        else
            item->setIcon(QIcon(":/icons/blue_dot.png"));
        eventList->addItem(item);
    }
}
void EventRiskWidget::showEventPopup(const Event &ev) { // Отображение всплывающего окна по событию
    QString msg = QString("<b>%1</b><br>%2<br><i>Влияние: %3%</i><br><br>Теги: %4")
                      .arg(ev.type)
                      .arg(ev.description)
                      .arg(ev.impactPercent, 0, 'f', 2)
                      .arg(ev.tags.join(", "));
    QMessageBox::information(this, "Детали события", msg);
}

void EventRiskWidget::exportToPDF() {
    QMessageBox::information(this, "Экспорт", "Экспорт в PDF (заглушка)");
}

void EventRiskWidget::exportToExcel() {
    QMessageBox::information(this, "Экспорт", "Экспорт в Excel (заглушка)");
}

void EventRiskWidget::onMapEventClicked(int eventId) { // Клик на карте по событию
    // Реализация заглушка
    QMessageBox::information(this, "Клик по событию на карте", QString("ID события: %1").arg(eventId));
}

void EventRiskWidget::addToReport() { // Добавление события в отчет
    QMessageBox::information(this, "Добавить в отчёт", "Событие добавлено в отчёт (заглушка)");
}

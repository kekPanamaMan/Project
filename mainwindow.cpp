#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "usericon.h"
#include "macroanalysiswidget.h"
#include "logindialog.h"
#include "userprofiledialog.h"
#include "settingsdialog.h"
#include "searchresultsdialog.h"
#include "qcustomplot.h"

#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QTimer>
#include <QFile>
#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QPushButton>
#include <QInputDialog>
#include <QMenu>
#include <QIcon>
#include <QLabel>
#include <QWidget>
#include <QMessageBox>
#include <QTextEdit>

// --- Circular ProgressBar с градиентом и тенью ---
CircularProgressBar::CircularProgressBar(QWidget *parent) : QWidget(parent) {
    setMinimumSize(120, 120);

    // Тень
    auto *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(16);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0,0,0,40));
    setGraphicsEffect(shadow);
}

void CircularProgressBar::setValue(int value) {
    m_value = value; // сохраняем новое значение прогресса
    update(); // перерисовываем виджет с новым значением
}

void CircularProgressBar::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing); // Включение сглаживания
    QRectF rect(16, 16, width()-32, height()-32); // Определение области рисовки

    // Градиент для прогресса
    QConicalGradient gradient(rect.center(), 90);
    gradient.setColorAt(0, QColor("#4f8cff"));
    gradient.setColorAt(0.5, QColor("#00e0c6"));
    gradient.setColorAt(1, QColor("#4f8cff"));

    // Фоновый круг
    p.setPen(QPen(QColor(230,230,240,180), 14));
    p.drawEllipse(rect);

    // Прогресс
    p.setPen(QPen(QBrush(gradient), 14, Qt::SolidLine, Qt::RoundCap));
    int span = static_cast<int>(360.0 * m_value / 100.0);
    p.drawArc(rect, 90*16, -span*16);

    // Текст
    p.setPen(QPen(QColor("#222"), 2));
    QFont font = p.font();
    font.setPointSize(22);
    font.setBold(true);
    p.setFont(font);
    p.drawText(rect, Qt::AlignCenter, QString("%1%").arg(m_value));
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

{
    ui->setupUi(this);

    // Применение темы при старте
    setTheme(":/theme_light.qss"); // или "theme_dark.qss"

    setWindowTitle("RiskGuard");
    resize(1280, 800);

    // --- Главный экран ---
    mainScreenWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainScreenWidget);
    mainLayout->setSpacing(18);
    mainLayout->setContentsMargins(0,0,0,0);

    // --- Верхняя панель ---
    QWidget *topBar = new QWidget;
    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(10, 5, 10, 5);

    createHeader();
    mainLayout->addWidget(headerWidget);

    createDashboard();
    mainLayout->addWidget(dashboardWidget, 1);

    createFooter();
    mainLayout->addWidget(footerToolbar);

    // --- Аналитика ---
    macroWidget = new MacroAnalysisWidget(this);
    connect(macroWidget, &MacroAnalysisWidget::goHome, this, &MainWindow::showMainScreen);
    connect(macroWidget, &MacroAnalysisWidget::updateRisk, this, &MainWindow::updateRiskInfo);


    // --- Событийный анализ ---
    eventRiskWidget = new EventRiskWidget(this);
    connect(eventRiskWidget, &EventRiskWidget::goHome, this, &MainWindow::showMainScreen);
    connect(eventRiskWidget, &EventRiskWidget::updateRisk, this, &MainWindow::updateRiskInfo);


    // --- Стохастическое моделирование ---
    stochasticWidget = new StochasticRiskWidget(this);
    connect(stochasticWidget, &StochasticRiskWidget::goHome, this, &MainWindow::showMainScreen);
    connect(stochasticWidget, &StochasticRiskWidget::updateRisk, this, &MainWindow::updateRiskInfo);

    alertsWidget = new AlertsWidget(this);
    reportsWidget = new ReportsWidget(this);

    // --- Стек виджетов ---
    stackedWidget = new QStackedWidget(this);
    stackedWidget->addWidget(mainScreenWidget); // 0
    stackedWidget->addWidget(macroWidget);      // 1
    stackedWidget->addWidget(eventRiskWidget);  // 2
    stackedWidget->addWidget(stochasticWidget); // 3
    stackedWidget->addWidget(alertsWidget);     // 4
    stackedWidget->addWidget(reportsWidget);    // 5

    connect(alertsWidget, &AlertsWidget::goHomeRequested, this, &MainWindow::showMainScreen);
    connect(reportsWidget, &ReportsWidget::goHomeRequested, this, &MainWindow::showMainScreen);

    setCentralWidget(stackedWidget);
    stackedWidget->setCurrentWidget(mainScreenWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::createHeader() {
    headerWidget = new QWidget(this); // Создаем виджет заголовка
    headerWidget->setObjectName("HeaderWidget"); // Важно!
    headerWidget->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #4f8cff, stop:1 #00e0c6); border-radius: 0 0 24px 24px;");
    headerWidget->setFixedHeight(76);

    QHBoxLayout *layout = new QHBoxLayout(headerWidget); // Горизонтальный layout
    layout->setContentsMargins(32, 0, 32, 0);

    // Логотип и название
    QLabel *logo = new QLabel(headerWidget);
    logo->setPixmap(QPixmap(":/icons/logo_modern.svg").scaled(44, 44, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    QLabel *appName = new QLabel("RiskGuard", headerWidget);
    appName->setStyleSheet("font-size: 26px; font-weight: 700; color: white; margin-left: 12px; letter-spacing: 1px;");
    QHBoxLayout *logoLayout = new QHBoxLayout();
    logoLayout->addWidget(logo);
    logoLayout->addWidget(appName);

    QWidget *logoWidget = new QWidget(headerWidget); // Контейнер для кнопки уведомлений
    logoWidget->setLayout(logoLayout);

    // Поиск
    searchLine = new QLineEdit(headerWidget);
    searchLine->setPlaceholderText("🔎 Поиск рисков, регионов...");
    searchLine->setFixedWidth(340);
    // Уведомления
    notificationsButton = new QToolButton(headerWidget);
    notificationsButton->setIcon(QIcon(":/icons/bell_modern.svg"));
    notificationsButton->setIconSize(QSize(32, 32));
    notificationsButton->setStyleSheet("QToolButton { border: none; background: transparent; }");
    notifIndicator = new QLabel(headerWidget);
    notifIndicator->setFixedSize(13, 13);
    notifIndicator->setStyleSheet("background: #ff4d67; border-radius: 7px; border: 2px solid #fff;");
    notifIndicator->move(8, 8);
    notifIndicator->raise();

    QWidget *notifWidget = new QWidget(headerWidget);
    QHBoxLayout *notifLayout = new QHBoxLayout(notifWidget);
    notifLayout->setContentsMargins(0, 0, 0, 0);
    notifLayout->addWidget(notificationsButton); // кнопка уведомлений
    notifLayout->addWidget(notifIndicator); // индикатор

    layout->addWidget(logoWidget, 0, Qt::AlignLeft); // Логотип
    layout->addSpacing(36);
    layout->addWidget(searchLine, 0); // Поле поиска
    layout->addStretch(); // Заполнение пустого пространства
    layout->addWidget(notifWidget); // Уведомления
    layout->addSpacing(18);

    // --- Кнопка пользователя с иконкой ---
    QToolButton *userButton = new QToolButton(headerWidget);
    userButton->setFixedSize(40, 40);
    userButton->setIcon(QIcon(":/icons/user_modern.svg")); // Подставьте свою иконку
    userButton->setIconSize(QSize(32, 32));
    userButton->setStyleSheet(R"(
        QToolButton {
            border: none;
            background: #ffffff;
            border-radius: 20px;
            padding: 4px;
            transitition: all 0.3s ease;
        }
        QToolButton:hover {
            background: #eaf2ff;
        }
    )");

    // --- Меню пользователя ---
    QMenu *userMenu = new QMenu(userButton);
    QAction *profileAction = userMenu->addAction("Профиль");
    QAction *settingsAction = userMenu->addAction("Настройки");
    userMenu->addSeparator();
    QAction *logoutAction = userMenu->addAction("Выход");

    connect(profileAction, &QAction::triggered, this, &MainWindow::openUserProfile);
    connect(settingsAction, &QAction::triggered, this, &MainWindow::openUserSettings);
    connect(logoutAction, &QAction::triggered, this, &MainWindow::handleLogout);
    connect(searchLine, &QLineEdit::returnPressed, this, &MainWindow::handleSearch);

    userButton->setMenu(userMenu); // меню пользователя
    userButton->setPopupMode(QToolButton::InstantPopup); // меню открывается при клике

    layout->addWidget(userButton); // Добавляем кнопку в layout
}

void MainWindow::openUserProfile() {
    if (!profileDialog)
        profileDialog = new UserProfileDialog(this); // Открытие диалога профиля
    profileDialog->show();
}

void MainWindow::openUserSettings() {
    if (!settingsDialog) {
        settingsDialog = new SettingsDialog(this); // Открытие диалога настроек
        connect(settingsDialog, &SettingsDialog::themeChanged, this, &MainWindow::setTheme);
    }
    settingsDialog->show();
}

void MainWindow::handleLogout()
{
    this->hide();
    LoginDialog loginDialog;
    if (loginDialog.exec() == QDialog::Accepted) {
        this->show(); // Показываем снова главное окно после успешного входа
    } else {
        qApp->quit(); // Выход, если нажата отмена
    }
}

void MainWindow::createDashboard() {
    dashboardWidget = new QWidget(this);
    dashboardWidget->setStyleSheet("background: transparent;");

    QVBoxLayout *mainLayout = new QVBoxLayout(dashboardWidget); // Основной вертикальный layout
    mainLayout->setSpacing(18);

    // --- Дашборд с эффектом стекла ---
    QFrame *dashFrame = new QFrame(dashboardWidget);
    dashFrame->setStyleSheet("background: rgba(255,255,255,0.7); border-radius: 28px; border: 1.5px solid #e0e6ef; box-shadow: 0 8px 32px rgba(80,120,255,0.10);");
    QHBoxLayout *dashLayout = new QHBoxLayout(dashFrame);

    // Индикатор риска
    riskCircle = new CircularProgressBar(dashFrame);
    riskCircle->setValue(65);

    QVBoxLayout *riskInfo = new QVBoxLayout();
    riskLabel = new QLabel("Текущий уровень риска: <b>0%</b> <span style='color:#2ecc71;'>(зелёная зона)</span>", dashFrame);
    riskLabel->setStyleSheet("font-size: 18px; color: #22304a;");
    changeLabel = new QLabel("↑ <b>%0", dashFrame);
    changeLabel->setStyleSheet("font-size: 14px; color: #00b894;");
    analysisLabel = new QLabel("Нет доступной аналитики <b>X</b>", dashFrame);
    analysisLabel->setStyleSheet("font-size: 14px; color: #888; font-style: italic;");
    analysisLabel->setWordWrap(true);

    riskInfo->addWidget(riskLabel);
    riskInfo->addWidget(changeLabel);
    riskInfo->addWidget(analysisLabel);

    dashLayout->addWidget(riskCircle, 0, Qt::AlignVCenter);
    dashLayout->addSpacing(28);
    dashLayout->addLayout(riskInfo);

    // --- Фильтры ---
    QHBoxLayout *filterLayout = new QHBoxLayout();
    QLabel *filterLabel = new QLabel("Фильтры:", dashboardWidget);
    filterLabel->setStyleSheet("font-weight: 600; color: #4f8cff;");
    regionCombo = new QComboBox(dashboardWidget);
    regionCombo->addItems({"Все регионы", "Северная Америка", "Европа", "Азия", "Ближний Восток"});
    periodCombo = new QComboBox(dashboardWidget);
    periodCombo->addItems({"Период", "День", "Неделя", "Месяц"});
    typeCombo = new QComboBox(dashboardWidget);
    typeCombo->addItems({"Все типы", "Финансовый", "Политический", "Экологический"});

    // Стилизация всех комбобоксов
    QString comboStyle = R"(
    QComboBox {
        background-color: white;
        color: black;
        padding: 4px 8px;
        border-radius: 6px;
        border: 1px solid #ccc;
    }
    QComboBox QAbstractItemView {
        background-color: white;
        color: black;
        selection-background-color: #e0e0e0;
        selection-color: black;
    }
    )";

    regionCombo->setStyleSheet(comboStyle); // Стили для выпадающих списков
    periodCombo->setStyleSheet(comboStyle);
    typeCombo->setStyleSheet(comboStyle);

    filterLayout->addWidget(filterLabel);
    filterLayout->addWidget(regionCombo);
    filterLayout->addWidget(periodCombo);
    filterLayout->addWidget(typeCombo);
    filterLayout->addStretch(); // Проталкиваем все вверх

    // --- Ключевые виджеты (заглушки) ---
    QHBoxLayout *widgetsLayout = new QHBoxLayout();

    // --- Карта рисков (тепловая карта) ---
    riskMapWidget = new QWidget(dashboardWidget);
    riskMapWidget->setMinimumSize(300, 200);  // Увеличиваем размер
    QVBoxLayout *riskMapLayout = new QVBoxLayout(riskMapWidget);

    riskMapImage = new QLabel;
    riskMapImage->setAlignment(Qt::AlignCenter);
    riskMapLayout->addWidget(riskMapImage);

    QLabel *mapLabel = new QLabel("Карта рисков (тепловая карта)");
    mapLabel->setAlignment(Qt::AlignCenter);
    mapLabel->setStyleSheet("font-weight: bold; font-size: 16px; color: #22304a;");
    riskMapLayout->addWidget(mapLabel);

    widgetsLayout->addWidget(riskMapWidget);

    // --- График: ВВП / Инфляция / Ставки ---
    economicChartPlot = new QCustomPlot(dashboardWidget);
    economicChartPlot->setMinimumSize(300, 200);  // Увеличиваем размер
    economicChartPlot->setProperty("glass", 1);

    // --- Примерные данные ---
    QVector<double> x(12), gdp(12), inflation(12), rates(12);
    for (int i = 0; i < 12; ++i) {
        x[i] = i + 1;
        gdp[i] = 2 + qSin(i / 2.0);
        inflation[i] = 5 + qCos(i / 3.0);
        rates[i] = 3 + qSin(i / 1.5);
    }

    economicChartPlot->addGraph();
    economicChartPlot->graph(0)->setName("ВВП");
    economicChartPlot->graph(0)->setData(x, gdp);
    economicChartPlot->graph(0)->setPen(QPen(Qt::blue, 2));

    economicChartPlot->addGraph();
    economicChartPlot->graph(1)->setName("Инфляция");
    economicChartPlot->graph(1)->setData(x, inflation);
    economicChartPlot->graph(1)->setPen(QPen(Qt::red, 2));

    economicChartPlot->addGraph();
    economicChartPlot->graph(2)->setName("Ставки");
    economicChartPlot->graph(2)->setData(x, rates);
    economicChartPlot->graph(2)->setPen(QPen(Qt::darkGreen, 2));

    economicChartPlot->legend->setVisible(true);
    economicChartPlot->rescaleAxes();
    economicChartPlot->replot();

    // --- Подпись под графиком ---
    QLabel *chartLabel = new QLabel("График: ВВП / Инфляция / Ставки", dashboardWidget);
    chartLabel->setAlignment(Qt::AlignCenter);
    chartLabel->setStyleSheet("font-weight: bold; font-size: 16px; color: #22304a;");

    // --- Контейнер с графиком и подписью ---
    QVBoxLayout *chartLayout = new QVBoxLayout();
    chartLayout->addWidget(economicChartPlot);
    chartLayout->addWidget(chartLabel);

    QWidget *chartContainer = new QWidget(dashboardWidget);
    chartContainer->setLayout(chartLayout);
    chartContainer->setMinimumSize(300, 200);  // Увеличиваем размер
    chartContainer->setProperty("glass", 1);

    // --- Добавление на главный layout ---
    widgetsLayout->addWidget(chartContainer);

    // --- Лента критических событий ---
    eventFeedBox = new QTextEdit(dashboardWidget);
    eventFeedBox->setReadOnly(true);
    eventFeedBox->setText(
        "🔴 Протесты в регионе X\n"
        "🌪 Ураган во Вьетнаме\n"
        "📉 Рецессия в стране Z\n"
        "🚨 Кибератака на инфраструктуру\n"
        "🔥 Пожар в промышленной зоне"
        );

    QLabel *eventLabel = new QLabel("Лента критических событий", dashboardWidget);
    eventLabel->setAlignment(Qt::AlignCenter);
    eventLabel->setStyleSheet("font-weight: 600; font-size: 15px; color: #22304a;");

    eventFeedBox->setMinimumSize(300, 180);  // Увеличиваем размер
    eventFeedBox->setProperty("glass", 1);
    eventFeedBox->setStyleSheet(R"(
    QTextEdit {
        background-color: rgba(255, 255, 255, 0.4);
        border: none;
        font-size: 14px;
        padding: 8px;
    }
)");

    // --- Топ-3 риска дня (список + подпись) ---
    topRiskList = new QListWidget(dashboardWidget);
    topRiskList->addItems({
        "1. Энергетический кризис",
        "2. Геополитическая напряжённость",
        "3. Инфляционное давление"
    });
    topRiskList->setMinimumSize(300, 180);  // Увеличиваем размер
    topRiskList->setStyleSheet(R"(
    QListWidget {
        background-color: rgba(255,255,255,0.4);
        border: none;
        font-size: 14px;
        padding: 8px;
    }
    QListWidget::item {
        padding: 6px;
    }
)");

    QLabel *topRiskLabel = new QLabel("Топ-3 риска дня", dashboardWidget);
    topRiskLabel->setAlignment(Qt::AlignCenter);
    topRiskLabel->setStyleSheet("font-weight: 600; font-size: 15px; color: #22304a;");

    // --- Контейнер для списка и подписи ---
    QVBoxLayout *topRiskLayout = new QVBoxLayout();
    topRiskLayout->addWidget(topRiskList);
    topRiskLayout->addWidget(topRiskLabel);

    QWidget *topRiskContainer = new QWidget(dashboardWidget);
    topRiskContainer->setLayout(topRiskLayout);
    topRiskContainer->setMinimumSize(300, 200);  // Увеличиваем размер
    topRiskContainer->setProperty("glass", 1);

    // --- Добавляем в layout ---
    widgetsLayout->addWidget(topRiskContainer);

    // --- Контейнер для ленты и подписи ---
    QVBoxLayout *eventLayout = new QVBoxLayout();
    eventLayout->addWidget(eventFeedBox);
    eventLayout->addWidget(eventLabel);

    QWidget *eventContainer = new QWidget(dashboardWidget);
    eventContainer->setLayout(eventLayout);
    eventContainer->setMinimumSize(300, 200);  // Увеличиваем размер
    eventContainer->setProperty("glass", 1);

    // --- Добавляем в layout ---
    widgetsLayout->addWidget(eventContainer);

    // Добавляем все виджеты в основной layout
    mainLayout->addWidget(dashFrame);
    mainLayout->addSpacing(12);
    mainLayout->addLayout(filterLayout);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(widgetsLayout);

    updateDashboardContent();

    connect(regionCombo, &QComboBox::currentTextChanged, this, &MainWindow::updateDashboardContent);
    connect(periodCombo, &QComboBox::currentTextChanged, this, &MainWindow::updateDashboardContent);
    connect(typeCombo, &QComboBox::currentTextChanged, this, &MainWindow::updateDashboardContent);
}

void MainWindow::updateDashboardContent() {
    QString region = regionCombo->currentText(); // Текущий регион
    QString period = periodCombo->currentText(); // Период
    QString type = typeCombo->currentText(); // Тип риска

    // --- Карта рисков (Отображение карты по региону) ---
    QString resourcePath;
    if (region == "Северная Америка") {
        resourcePath = ":/NorthAmerica.png";
    } else if (region == "Европа") {
        resourcePath = ":/Europe.png";
    } else if (region == "Азия") {
        resourcePath = ":/Asia.png";
    } else if (region == "Ближний Восток") {
        resourcePath = ":/TheMiddleEast.png";
    } else {
        resourcePath = ":/heatmap.png";
    }

    // --- Установка карты на QLabel ---
    QPixmap mapPixmap(resourcePath);
    if (!mapPixmap.isNull()) {
        riskMapImage->setPixmap(mapPixmap.scaled(300, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        qDebug() << "Ошибка загрузки карты:" << resourcePath;
    }

    // --- График ---
    QVector<double> x(12), gdp(12), inflation(12), rates(12);
    for (int i = 0; i < 12; ++i) {
        x[i] = i + 1;
        gdp[i] = 2 + qSin(i / 2.0);
        inflation[i] = 5 + qCos(i / 3.0);
        rates[i] = 3 + qSin(i / 1.5);

        if (type == "Финансовый") gdp[i] += 1.0;
        if (type == "Политический") inflation[i] += 0.5;
        if (region == "Азия") rates[i] += 0.3;
        if (period == "Неделя") gdp[i] *= 1.05;
        if (period == "Месяц") inflation[i] *= 1.08;
    }

    economicChartPlot->graph(0)->setData(x, gdp);
    economicChartPlot->graph(1)->setData(x, inflation);
    economicChartPlot->graph(2)->setData(x, rates);
    economicChartPlot->replot();

    // --- Лента событий ---
    eventFeedBox->clear();
    if (region == "Ближний Восток")
        eventFeedBox->setText("🛑 Конфликт в регионе\n🚨 Политическая нестабильность\n📉 Падение инвестиций");
    else if (type == "Экологический")
        eventFeedBox->setText("🌪 Ураган на побережье\n🔥 Пожар в лесной зоне\n🌊 Наводнение в регионе X");
    else
        eventFeedBox->setText("🔴 Протесты в регионе X\n🌪 Ураган во Вьетнаме\n📉 Рецессия в стране Z");

    // --- Топ-3 риска дня ---
    topRiskList->clear();
    if (type == "Политический") {
        topRiskList->addItems({
            "1. Геополитическая напряжённость",
            "2. Массовые протесты",
            "3. Санкции и торговые конфликты"
        });
    } else if (type == "Экологический") {
        topRiskList->addItems({
            "1. Природные катастрофы",
            "2. Климатические изменения",
            "3. Наводнения и ураганы"
        });
    } else {
        topRiskList->addItems({
            "1. Энергетический кризис",
            "2. Инфляционное давление",
            "3. Рецессия"
        });
    }

    // --- Индикатор риска ---
    int value = 0;
    if (region == "Северная Америка") value = 45;
    else if (region == "Азия") value = 78;
    else if (region == "Ближний Восток") value = 88;
    else if (region == "Европа") value = 65;

    riskCircle->setValue(value);

    QString zone, color;
    if (value >= 80) { zone = "(красная зона)"; color = "#e74c3c"; }
    else if (value >= 50) { zone = "(жёлтая зона)"; color = "#f7b731"; }
    else { zone = "(зелёная зона)"; color = "#2ecc71"; }

    int changeValue = 0; // базовое значение

    if (period == "День") changeValue = 2;
    else if (period == "Неделя") changeValue = 7;
    else if (period == "Месяц") changeValue = 15;

    if (region == "Азия") changeValue += 3;
    if (type == "Политический") changeValue += 2;

    riskLabel->setText(QString("Текущий уровень риска: <b>%1%</b> <span style='color:%2;'>%3</span>").arg(value).arg(color).arg(zone));
    QString changePeriod;
    if (period == "День")        changePeriod = "за день";
    else if (period == "Неделя") changePeriod = "за неделю";
    else if (period == "Месяц")  changePeriod = "за месяц";

    changeLabel->setText(QString("↑ <b>%1%</b> %2").arg(changeValue).arg(changePeriod));
    analysisLabel->setText(QString("Рост рисков в регионе <b>%1</b>").arg(region));
}

void MainWindow::createFooter() {
    footerToolbar = new QToolBar(this);
    footerToolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    footerToolbar->setMovable(false); // Запрет перемещения

    // Создание действий
    homeAction = new QAction(QIcon(":/icons/home_modern.svg"), "Главная", this);
    analyticsAction = new QAction(QIcon(":/icons/chart_modern.svg"), "Аналитика", this);
    alertsAction = new QAction(QIcon(":/icons/bell_modern.svg"), "Оповещения", this);
    reportsAction = new QAction(QIcon(":/icons/report_modern.svg"), "Отчёты", this);

    // Добавление в тулбар
    footerToolbar->addAction(homeAction);
    footerToolbar->addAction(analyticsAction);
    footerToolbar->addAction(alertsAction);
    footerToolbar->addAction(reportsAction);

    // --Аналитикаconnect(analyticsAction, &QAction::triggered, this, &MainWindow::showAnalytics);
    connect(analyticsAction, &QAction::triggered, this, &MainWindow::showAnalyzeDialog);

    // --- Меню настроек ---
    settingsMenu = new QMenu(this);
    lightThemeAction = new QAction(QIcon(":/icons/theme_light.svg"), "Светлая тема", this);
    darkThemeAction  = new QAction(QIcon(":/icons/theme_dark.svg"),  "Тёмная тема", this);
    settingsMenu->addAction(lightThemeAction);
    settingsMenu->addAction(darkThemeAction);

    connect(lightThemeAction, &QAction::triggered, this, [this](){
        setTheme(":/theme_light.qss");
    });
    connect(darkThemeAction, &QAction::triggered, this, [this](){
        setTheme(":/theme_dark.qss");
    });

    // --- Кнопка настроек с выпадающим меню ---
    QToolButton *settingsButton = new QToolButton(this);
    settingsButton->setIcon(QIcon(":/icons/settings_modern.svg"));
    settingsButton->setText("Настройки");
    settingsButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    settingsButton->setPopupMode(QToolButton::InstantPopup);
    settingsButton->setMenu(settingsMenu);

    connect(alertsAction, &QAction::triggered, this, [this]() {
        stackedWidget->setCurrentWidget(alertsWidget);
    });
    connect(reportsAction, &QAction::triggered, this, [this]() {
        stackedWidget->setCurrentWidget(reportsWidget);
    });

    footerToolbar->addWidget(settingsButton);
}

void MainWindow::showAlerts() {
    if (!alertsWidget) {
        alertsWidget = new AlertsWidget(this); // Создаем виджет, если еще не создан
        stackedWidget->addWidget(alertsWidget); // Добавляем в стек
    }
    stackedWidget->setCurrentWidget(alertsWidget); // Переключаемся на него
}

void MainWindow::showReports() {
    if (!reportsWidget) {
        reportsWidget = new ReportsWidget(this);
        stackedWidget->addWidget(reportsWidget);
    }
    stackedWidget->setCurrentWidget(reportsWidget);
}

void MainWindow::showMainScreen() {
    stackedWidget->setCurrentWidget(mainScreenWidget); // Переключение на основной экран
}

void MainWindow::setTheme(const QString &themeFile)
{
    QFile file(themeFile);
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        qApp->setStyleSheet(styleSheet); // Применяем тему
    }
    else {
        QMessageBox::warning(this, "Ошибка", "Не удалось загрузить тему");
    }
}

void MainWindow::showAnalytics() {
    stackedWidget->setCurrentWidget(macroWidget);
}

// Диалог выбора типа анализа
void MainWindow::showAnalyzeDialog() {
    QStringList options;
    options << "Макроэкономический анализ"
            << "Событийный анализ (Event Risk)"
            << "Стохастическое моделирование рисков";
    bool ok = false;
    QString choice = QInputDialog::getItem(this, "Выбор анализа", "Выберите тип анализа:", options, 0, false, &ok);
    if (!ok) return;

    if (choice == "Макроэкономический анализ") {
        showMacroAnalysis();
    } else if (choice == "Событийный анализ (Event Risk)") {
        showEventRiskAnalysis();
    } else if (choice == "Стохастическое моделирование рисков") {
        showStochasticRiskAnalysis();
    }
}

void MainWindow::showMacroAnalysis() {
    stackedWidget->setCurrentWidget(macroWidget); // Макроанализ
}

void MainWindow::showEventRiskAnalysis() {
    stackedWidget->setCurrentWidget(eventRiskWidget); // Событийный анализ
}
void MainWindow::showStochasticRiskAnalysis() {
    stackedWidget->setCurrentWidget(stochasticWidget); // Стохастический анализ
}

void MainWindow::updateRiskInfo(int riskLevel, int riskChange, const QString &description) {
    riskCircle->setValue(riskLevel); // обновляем прогрессбар

    QString zone;
    QString color;
    if (riskLevel >= 80) {
        zone = "(красная зона)";
        color = "#e74c3c";
    } else if (riskLevel >= 50) {
        zone = "(жёлтая зона)";
        color = "#f7b731";
    } else {
        zone = "(зелёная зона)";
        color = "#2ecc71";
    }

    riskLabel->setText(QString("Текущий уровень риска: <b>%1%</b> <span style='color:%2;'>%3</span>").arg(riskLevel).arg(color).arg(zone));
    changeLabel->setText(QString("%1 <b>%2%</b> за день").arg(riskChange >= 0 ? "↑" : "↓").arg(abs(riskChange)));
    changeLabel->setStyleSheet(QString("font-size: 14px; color: %1;").arg(riskChange >= 0 ? "#00b894" : "#e74c3c"));
    analysisLabel->setText(description);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)

    event->accept();
}

// Выполнение поиска
void MainWindow::handleSearch() {
    QString query = searchLine->text().trimmed().toLower();

    if (query.isEmpty() || query.length() < 2)
        return;

    QStringList results;

    // 1. Поиск по регионам
    if (query.contains("европа")) results << "Регион: Европа (риск 55%)";
    if (query.contains("сша") || query.contains("америка")) results << "Регион: Северная Америка (риск 45%)";
    if (query.contains("азия")) results << "Регион: Азия (риск 78%)";
    if (query.contains("ближний восток")) results << "Регион: Ближний Восток (риск 88%)";

    // 2. Поиск по типам риска
    if (query.contains("инфляция") || query.contains("ввп") || query.contains("ставки"))
        results << "Тип риска: Финансовый\n- Инфляция +0.6%\n- ВВП -1.2%";
    if (query.contains("протест") || query.contains("санкции") || query.contains("политика"))
        results << "Тип риска: Политический\n- Массовые протесты\n- Политическая нестабильность";
    if (query.contains("ураган") || query.contains("пожар") || query.contains("природа"))
        results << "Тип риска: Экологический\n- Ураган в регионе Y\n- Лесные пожары";

    // 3. Поиск по событиям
    if (query.contains("угроза") || query.contains("событие") || query.contains("авария"))
        results << "События:\n🚨 Кибератака на инфраструктуру\n🔥 Пожар на заводе\n📉 Рецессия в стране Z";

    if (results.isEmpty())
        results << "Ничего не найдено по запросу: \"" + query + "\"";

    SearchResultsDialog *dialog = new SearchResultsDialog(this);
    dialog->setResults(results);
    dialog->exec();

    QString currentRegion = regionCombo->currentText();
    QString currentPeriod = periodCombo->currentText();
    QString currentType = typeCombo->currentText();
}

// stochasticriskwidget.cpp
#include "stochasticriskwidget.h"
#include "qcustomplot.h"

#include <QHBoxLayout>
#include <QMessageBox>
#include <QMenu>
#include <QToolButton>
#include <QVBoxLayout>
#include <QRandomGenerator>
#include <QLabel>
#include <QTableView>
#include <QPushButton>
#include <QComboBox>
#include <QToolBar>
#include <QHeaderView>
#include <algorithm>
#include <QPrinter>
#include <QFileDialog>
#include <QPainter>
#include <QTextStream>

StochasticRiskWidget::StochasticRiskWidget(QWidget *parent) : QWidget(parent) {
    setupUI();
}

void StochasticRiskWidget::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // --- Верхняя кнопка "На главную" ---
    QHBoxLayout *headerLayout = new QHBoxLayout;

    QPushButton *homeButton = new QPushButton("🏠 На главную");
    homeButton->setStyleSheet(R"(
        QPushButton {
            padding: 6px 14px;
            font-weight: bold;
            background-color: #4f8cff;
            color: white;
            border: none;
            border-radius: 6px;
        }
        QPushButton:hover {
            background-color: #3c6fdc;
        }
    )");
    homeButton->setCursor(Qt::PointingHandCursor);
    connect(homeButton, &QPushButton::clicked, this, [this]() {
        emit goHome();
    });

    headerLayout->addWidget(homeButton, 0, Qt::AlignLeft);
    headerLayout->addStretch();
    mainLayout->addLayout(headerLayout);  // добавляем в начало

    // --- График ---
    chart = new QCustomPlot(this);
    chart->setMinimumHeight(300);
    chart->legend->setVisible(true);
    chart->xAxis->setLabel("Месяц");
    chart->yAxis->setLabel("Инфляция (%)");
    mainLayout->addWidget(chart);

    // --- Страна ---
    countryCombo = new QComboBox(this);
    countryCombo->addItems({"США", "Китай", "Украина", "Россия", "Германия"});
    mainLayout->addWidget(new QLabel("Выберите страну:"));
    mainLayout->addWidget(countryCombo);

    // --- Таблица параметров ---
    inputTable = new QTableWidget(3, 2, this);
    inputTable->setHorizontalHeaderLabels({"Параметр", "Значение"});
    inputTable->horizontalHeader()->setStretchLastSection(true);
    inputTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    inputTable->verticalHeader()->setVisible(false);
    inputTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    inputTable->setMinimumHeight(120);
    inputTable->setStyleSheet(R"(
        QTableWidget { font-size: 14px; color: #333; }
        QTableWidget::item { padding: 8px; }
        QTableWidget::horizontalHeader { background-color: #f0f0f0; }
        QTableWidget::item:selected { background-color: #b0e0e6; }
    )");

    mainLayout->addWidget(new QLabel("Параметры моделирования:"));
    mainLayout->addWidget(inputTable);

    // --- Модель ---
    modelCombo = new QComboBox;
    modelCombo->addItems({"Monte Carlo", "Value at Risk (VaR)", "Стресс-тест"});
    connect(modelCombo, &QComboBox::currentTextChanged, this, &StochasticRiskWidget::onModelChanged);
    mainLayout->addWidget(modelCombo);  // только ComboBox без надписи

    // --- Параметры модели (заглушка) ---
    paramsWidget = new QWidget;
    paramsWidget->setMinimumHeight(60);
    mainLayout->addWidget(paramsWidget);

    // --- Запуск ---
    runButton = new QPushButton("Запустить моделирование");
    connect(runButton, &QPushButton::clicked, this, &StochasticRiskWidget::runSimulation);
    mainLayout->addWidget(runButton);

    // --- Таблица квантилей ---
    quantileTable = new QTableView;
    mainLayout->addWidget(quantileTable);

    // --- Экспорт ---
    exportPdfBtn = new QPushButton("Экспорт в PDF");
    exportExcelBtn = new QPushButton("Экспорт в Excel");
    connect(exportPdfBtn, &QPushButton::clicked, this, &StochasticRiskWidget::exportToPDF);
    connect(exportExcelBtn, &QPushButton::clicked, this, &StochasticRiskWidget::exportToExcel);
    QHBoxLayout *exportLayout = new QHBoxLayout;
    exportLayout->addWidget(exportPdfBtn);
    exportLayout->addWidget(exportExcelBtn);
    mainLayout->addLayout(exportLayout);

    setLayout(mainLayout);
}

void StochasticRiskWidget::onModelChanged(const QString &model) {
    Q_UNUSED(model);
    updateParameterTable();
}

void StochasticRiskWidget::runSimulation() {
    chart->clearGraphs();
    updateParameterTable();

    QString selected = modelCombo->currentText().trimmed();
    if (selected == "Monte Carlo") simulateMonteCarlo();
    else if (selected == "Value at Risk (VaR)") simulateVaR();
    else if (selected == "Стресс-тест") simulateStressTest();

    chart->rescaleAxes();
    chart->replot();

    // Обновить уровень риска
    int value = 58;
    int change = 1;
    QString description = "Моделирование показало умеренный рост рисков";
    emit updateRisk(value, change, description);
}

void StochasticRiskWidget::updateParameterTable() {
    QString country = countryCombo->currentText();
    double mu = 0.0, sigma = 0.0;

    if (country == "США") { mu = 0.015; sigma = 0.03; }
    else if (country == "Китай") { mu = 0.012; sigma = 0.05; }
    else if (country == "Украина") { mu = 0.03; sigma = 0.12; }
    else if (country == "Россия") { mu = 0.025; sigma = 0.10; }
    else if (country == "Германия") { mu = 0.01; sigma = 0.02; }

    inputTable->setItem(0, 0, new QTableWidgetItem("Ожидаемый рост"));
    inputTable->setItem(0, 1, new QTableWidgetItem(QString::number(mu * 100, 'f', 2) + " %"));
    inputTable->setItem(1, 0, new QTableWidgetItem("Волатильность"));
    inputTable->setItem(1, 1, new QTableWidgetItem(QString::number(sigma * 100, 'f', 2) + " %"));
    inputTable->setItem(2, 0, new QTableWidgetItem("Сценарий риска"));
    inputTable->setItem(2, 1, new QTableWidgetItem(modelCombo->currentText()));

    for (int i = 0; i < inputTable->rowCount(); ++i) {
        QTableWidgetItem *item = inputTable->item(i, 0);
        if (item) {
            QFont font = item->font();
            font.setBold(true);
            item->setFont(font);
        }
    }
}

void StochasticRiskWidget::simulateMonteCarlo() {
    int numSteps = 12;
    int numSimulations = 50;
    double value = 2.0;

    QString country = countryCombo->currentText();
    double mu = 0.02, sigma = 0.1;
    if (country == "США") { mu = 0.015; sigma = 0.03; }
    else if (country == "Китай") { mu = 0.012; sigma = 0.05; }
    else if (country == "Украина") { mu = 0.03; sigma = 0.12; }
    else if (country == "Россия") { mu = 0.025; sigma = 0.10; }
    else if (country == "Германия") { mu = 0.01; sigma = 0.02; }

    for (int sim = 0; sim < numSimulations; ++sim) {
        QVector<double> x, y;
        double v = value;
        for (int t = 0; t < numSteps; ++t) {
            double shock = QRandomGenerator::global()->generateDouble()* 2.0 - 1.0;
            v *= (1.0 + mu + sigma * shock);
            x.append(t);
            y.append(v);
        }
        chart->addGraph();
        chart->graph()->setData(x, y);
        chart->graph()->setPen(sim == 0 ? QPen(Qt::blue, 2) : QPen(QColor(100, 100, 255, 40)));
        if (sim == 0) {
            chart->graph()->setName("Monte Carlo (основной");
        } else {
            chart->graph()->setName("");
        }
    }
}

void StochasticRiskWidget::simulateVaR() {
    QVector<double> values;
    double initial = 100.0;
    double mu = 0.01, sigma = 0.04;
    for (int i = 0; i < 1000; ++i) {
        double shock = QRandomGenerator::global()->generateDouble()* 2.0 - 1.0;
        double result = initial * (1 + mu + sigma * shock);
        values.append(result);
    }
    std::sort(values.begin(), values.end());
    double var95 = values[int(values.size() * 0.05)];

    chart->addGraph();
    chart->graph()->setData({0, 1}, {initial, var95});
    chart->graph()->setPen(QPen(Qt::red, 2));
    chart->graph()->setName("VaR (95%)");
}

void StochasticRiskWidget::simulateStressTest() {
    QVector<double> x, y;
    double base = 100.0;
    for (int t = 0; t <= 12; ++t) {
        x.append(t);
        y.append(base * (1 - 0.05 * t));
    }
    chart->addGraph();
    chart->graph()->setData(x, y);
    chart->graph()->setPen(QPen(Qt::darkGreen, 2));
    chart->graph()->setName("Стресс-тест");
}

void StochasticRiskWidget::exportToPDF() {
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (btn == exportExcelBtn) {
    QString file = QFileDialog::getSaveFileName(this, "Экспорт в PDF", "", "*.pdf");
    // Реализовать экспорт
    QMessageBox::information(this, "Экспорт", "Экспорт в PDF завершён (заглушка)");
    }
}

void StochasticRiskWidget::exportToExcel() {
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (btn == exportExcelBtn) {
    QString file = QFileDialog::getSaveFileName(this, "Экспорт в Excel", "", "*.xlsx");
    // Реализовать экспорт
    QMessageBox::information(this, "Экспорт", "Экспорт в Excel завершён (заглушка)");
    }
}

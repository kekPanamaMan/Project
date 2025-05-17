#include "macroanalysiswidget.h"

#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QRandomGenerator>


MacroAnalysisWidget::MacroAnalysisWidget(QWidget *parent) : QWidget(parent) { // Конструктор виджета макроэкономического анализа
    setupUI();
    setupConnections();
    fetchIndicators();
    fetchData();
}

void MacroAnalysisWidget::setupUI() {
    // --- Фильтры ---
    countryCombo = new QComboBox;
    countryCombo->setEditable(true);
    countryCombo->addItems({"США", "Германия", "Китай", "Россия", "Бразилия"});
    countryCombo->setCurrentText("США");

    periodCombo = new QComboBox;
    periodCombo->addItems({"Месяц", "Квартал", "Год", "Произвольно"});

    dateFromEdit = new QDateEdit(QDate::fromString("2019-01-01", "yyyy-MM-dd"));
    dateToEdit = new QDateEdit(QDate::currentDate());

    // Три выпадающих списка для выбора макроэкономических индикаторов
    indicatorCombo1 = new QComboBox;
    indicatorCombo2 = new QComboBox;
    indicatorCombo3 = new QComboBox;
    indicatorCombo1->setEditable(true);
    indicatorCombo2->setEditable(true);
    indicatorCombo3->setEditable(true);

    // --- Прогноз ---
    modelCombo = new QComboBox;
    modelCombo->addItems({"ARIMA", "Линейная регрессия"});
    paramSlider = new QSlider(Qt::Horizontal);
    paramSlider->setRange(1, 12);
    paramSlider->setValue(6);

    // --- Кнопки экспорта ---
    exportExcelBtn = new QPushButton("Экспорт в Excel");
    exportPdfBtn = new QPushButton("Экспорт в PDF");
    exportPngBtn = new QPushButton("Экспорт в PNG");

    // --- График ---
    chartView = new QCustomPlot(this);
    chartView->setMinimumHeight(320);

    // --- Таблица ---
    tableModel = new QStandardItemModel(this);
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(tableModel);

    dataTable = new QTableView;
    dataTable->setModel(proxyModel);
    dataTable->setSortingEnabled(true);
    dataTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // --- Layout ---
    QHBoxLayout *filtersLayout = new QHBoxLayout;
    filtersLayout->addWidget(new QLabel("Страна/регион:"));
    filtersLayout->addWidget(countryCombo);
    filtersLayout->addWidget(new QLabel("Период:"));
    filtersLayout->addWidget(periodCombo);
    filtersLayout->addWidget(new QLabel("С:"));
    filtersLayout->addWidget(dateFromEdit);
    filtersLayout->addWidget(new QLabel("По:"));
    filtersLayout->addWidget(dateToEdit);

    QHBoxLayout *indicatorsLayout = new QHBoxLayout;
    indicatorsLayout->addWidget(new QLabel("Показатели:"));
    indicatorsLayout->addWidget(indicatorCombo1);
    indicatorsLayout->addWidget(indicatorCombo2);
    indicatorsLayout->addWidget(indicatorCombo3);

    QHBoxLayout *forecastLayout = new QHBoxLayout;
    forecastLayout->addWidget(new QLabel("Модель:"));
    forecastLayout->addWidget(modelCombo);
    forecastLayout->addWidget(new QLabel("Параметр:"));
    forecastLayout->addWidget(paramSlider);

    QHBoxLayout *exportLayout = new QHBoxLayout;
    exportLayout->addWidget(exportExcelBtn);
    exportLayout->addWidget(exportPdfBtn);
    exportLayout->addWidget(exportPngBtn);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // --- Кнопка "На главную" ---
    QPushButton *homeButton = new QPushButton("🏠 На главную");
    homeButton->setStyleSheet("padding: 8px 16px; background-color: #4f8cff; color: white; border-radius: 8px;");
    connect(homeButton, &QPushButton::clicked, this, &MacroAnalysisWidget::goHome);
    mainLayout->addWidget(homeButton, 0, Qt::AlignLeft);

    mainLayout->addLayout(filtersLayout);
    mainLayout->addLayout(indicatorsLayout);
    mainLayout->addWidget(chartView);
    mainLayout->addLayout(forecastLayout);
    mainLayout->addWidget(dataTable);
    mainLayout->addLayout(exportLayout);

    setLayout(mainLayout);
}

void MacroAnalysisWidget::setupConnections() {
    // Обновляем данные при изменении фильтров
    connect(countryCombo, &QComboBox::currentTextChanged, this, &MacroAnalysisWidget::fetchData);
    connect(periodCombo, &QComboBox::currentTextChanged, this, &MacroAnalysisWidget::fetchData);
    connect(dateFromEdit, &QDateEdit::dateChanged, this, &MacroAnalysisWidget::fetchData);
    connect(dateToEdit, &QDateEdit::dateChanged, this, &MacroAnalysisWidget::fetchData);

    // Обновляем график при выборе индикаторов
    connect(indicatorCombo1, &QComboBox::currentTextChanged, this, &MacroAnalysisWidget::updateChart);
    connect(indicatorCombo2, &QComboBox::currentTextChanged, this, &MacroAnalysisWidget::updateChart);
    connect(indicatorCombo3, &QComboBox::currentTextChanged, this, &MacroAnalysisWidget::updateChart);

    // Сигналы для модели прогнозирования
    connect(modelCombo, &QComboBox::currentTextChanged, this, &MacroAnalysisWidget::showForecast);
    connect(paramSlider, &QSlider::valueChanged, this, &MacroAnalysisWidget::showForecast);

    // Кнопки экспорта графиков
    connect(exportExcelBtn, &QPushButton::clicked, this, &MacroAnalysisWidget::exportData);
    connect(exportPdfBtn, &QPushButton::clicked, this, &MacroAnalysisWidget::exportData);
    connect(exportPngBtn, &QPushButton::clicked, this, &MacroAnalysisWidget::exportData);
}

void MacroAnalysisWidget::fetchIndicators() {
    // Примерные индикаторы
    QStringList indicators = {"Инфляция", "Ключевая ставка", "ВВП", "Безработица", "Долг"};
    indicatorCombo1->addItems(indicators);
    indicatorCombo2->addItems(indicators);
    indicatorCombo3->addItems(indicators);
    indicatorCombo1->setCurrentText("Инфляция");
    indicatorCombo2->setCurrentText("Ключевая ставка");
    indicatorCombo3->setCurrentText("");
}

void MacroAnalysisWidget::fetchData() {
    // Здесь должен быть запрос к API (МВФ, ВБ и т.д.)
    // Для примера — генерация тестовых данных
    tableModel->clear();
    tableModel->setHorizontalHeaderLabels({"Дата", "Показатель", "Значение", "Источник", "Событие"});
    QDate start = dateFromEdit->date();
    QDate end = dateToEdit->date();
    int days = start.daysTo(end);

    for (int i = 0; i <= days; i += 30) {
        QDate d = start.addDays(i);
        QList<QStandardItem*> row;
        row << new QStandardItem(d.toString("yyyy-MM"));
        row << new QStandardItem("Инфляция");
        row << new QStandardItem(QString::number(2.0 + QRandomGenerator::global()->bounded(3) + i*0.01, 'f', 2));
        row << new QStandardItem("МВФ");
        row << new QStandardItem(i==90 ? "Изменение ставки ЦБ" : "");
        tableModel->appendRow(row);
    }
    updateChart();
}

void MacroAnalysisWidget::updateChart() {
    chartView->clearGraphs(); // Стереть старые графики

    chartView->legend->setVisible(true); // Показать легенду

    QStringList indicators = {indicatorCombo1->currentText(), indicatorCombo2->currentText(), indicatorCombo3->currentText()};
    QColor colors[3] = {Qt::blue, Qt::red, Qt::darkGreen};

    for (int idx = 0; idx < 3; ++idx) {
        if (indicators[idx].isEmpty()) continue;
        QVector<double> x, y;
        // Проход по данным в таблице и выборе значений по выбранному индикатору
        for (int row = 0; row < tableModel->rowCount(); ++row) {
            QString name = tableModel->item(row, 1)->text();
            if (name == indicators[idx]) {
                QDate date = QDate::fromString(tableModel->item(row, 0)->text(), "yyyy-MM");
                x << date.toJulianDay();
                y << tableModel->item(row, 2)->text().toDouble();
            }
        }
        chartView->addGraph(); // Добавляем новый график
        chartView->graph(idx)->setName(indicators[idx]);
        chartView->graph(idx)->setPen(QPen(colors[idx], 2));
        chartView->graph(idx)->setData(x, y);
    }
    chartView->xAxis->setLabel("Дата");
    chartView->yAxis->setLabel("Значение");
    chartView->rescaleAxes(); // Оси
    chartView->replot(); // Отображение

    // Обновление главного уровня риска (сигнал в MainWindow)
    int riskValue = 65;
    int riskChange = 2;
    QString description = "Рост рисков из-за нестабильности в регионе X";

    emit updateRisk(riskValue, riskChange, description);
}

void MacroAnalysisWidget::showForecast() {
    if (indicatorCombo1->currentText().isEmpty()) return;

    // Удаляем старые прогнозные графики (оставляем первые 3)
    while (chartView->graphCount() > 3) {
        chartView->removeGraph(chartView->graphCount() - 1);
    }

    QVector<double> realX, realY;

    // Извлечение данных с первого графика
    const QCPGraphDataContainer *dataContainer = chartView->graph(0)->data().data();
    for (auto it = dataContainer->constBegin(); it != dataContainer->constEnd(); ++it) {
        realX.append(it->key);
        realY.append(it->value);
    }

    if (realX.isEmpty() || realY.isEmpty()) return;

    // Формируем прогноз
    double lastX = realX.last();
    double lastY = realY.last();

    QVector<double> x, y;
    int steps = paramSlider->value(); // Количество шагов прогноза
    for (int i = 1; i <= steps; ++i) {
        x.append(lastX + i * 30);  // например, шаг = 30 дней
        double delta = QRandomGenerator::global()->bounded(3.0) - 1.5;
        y.append(lastY + delta);
    }

    chartView->addGraph();
    chartView->graph()->setName("Прогноз");
    chartView->graph()->setPen(QPen(Qt::gray, 2, Qt::DashLine));
    chartView->graph()->setData(x, y);
    chartView->replot();
}
void MacroAnalysisWidget::exportData() {
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (btn == exportExcelBtn) {
        QString file = QFileDialog::getSaveFileName(this, "Экспорт в Excel", "", "*.xlsx");
        // Реализовать экспорт
        QMessageBox::information(this, "Экспорт", "Экспорт в Excel завершён (заглушка)");
    } else if (btn == exportPdfBtn) {
        QString file = QFileDialog::getSaveFileName(this, "Экспорт в PDF", "", "*.pdf");
        // Реализовать экспорт
        QMessageBox::information(this, "Экспорт", "Экспорт в PDF завершён (заглушка)");
    } else if (btn == exportPngBtn) {
        QString file = QFileDialog::getSaveFileName(this, "Экспорт в PNG", "", "*.png");
        // Реализовать экспорт
        QMessageBox::information(this, "Экспорт", "Экспорт в PNG завершён (заглушка)");
    }
}

void MacroAnalysisWidget::loadData() {
    // Реализация или просто пусто
}

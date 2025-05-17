#ifndef MACROANALYSISWIDGET_H
#define MACROANALYSISWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QDateEdit>
#include <QPushButton>
#include <QTableView>
#include <QSlider>
#include <QLabel>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include "qcustomplot.h"



class MacroAnalysisWidget : public QWidget {
    Q_OBJECT
public:
    explicit MacroAnalysisWidget(QWidget *parent = nullptr);

private slots:
    void loadData();
    void updateChart();
    void exportData();

private:
    // Фильтры
    QComboBox *countryCombo;
    QComboBox *periodCombo;
    QDateEdit *dateFromEdit;
    QDateEdit *dateToEdit;
    QLineEdit *indicatorSearch;
    QComboBox *indicatorCombo1;
    QComboBox *indicatorCombo2;
    QComboBox *indicatorCombo3;

    // Прогноз
    QComboBox *modelCombo;
    QSlider *paramSlider;

    // Визуализация
    QCustomPlot *chartView;
    QTableView *dataTable;
    QStandardItemModel *tableModel;
    QSortFilterProxyModel *proxyModel;

    // Экспорт
    QPushButton *exportExcelBtn;
    QPushButton *exportPdfBtn;
    QPushButton *exportPngBtn;

    // Внутренние методы
    void setupUI();
    void setupConnections();
    void fetchIndicators();
    void fetchData();
    void showForecast();

signals:
    void goHome();
    void updateRisk(int value, int change, const QString &description);
};

#endif // MACROANALYSISWIDGET_H

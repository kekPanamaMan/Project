// stochasticriskwidget.h
#ifndef STOCHASTICRISKWIDGET_H
#define STOCHASTICRISKWIDGET_H

#include "qcustomplot.h"

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QTableView>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QToolBar>
#include <QAction>
#include <QLabel>
#include <QStackedWidget>

class StochasticRiskWidget : public QWidget {
    Q_OBJECT
public:
    explicit StochasticRiskWidget(QWidget *parent = nullptr);

signals:
    void goHome();
    void updateRisk(int value, int change, const QString &description);

private slots:
    void onModelChanged(const QString &model);
    void exportToPDF();
    void exportToExcel();
    void runSimulation();
    void updateParameterTable();

private:
    void setupUI();
    void updateUI();
    void simulateMonteCarlo();
    void simulateVaR();
    void simulateStressTest();

    QComboBox *modelCombo;
    QWidget *paramsWidget;
    QPushButton *runButton;
    QLabel *plotLabel; // Здесь можно интегрировать matplotlib/plotly через QWebEngineView
    QTableView *quantileTable;
    QTableWidget *inputTable;
    QPushButton *exportPdfBtn;
    QPushButton *exportExcelBtn;
    QToolBar *footerToolbar;
    QCustomPlot *chart;
    QTableView *quantileTablel;
    QComboBox *countryCombo;
};

#endif // STOCHASTICRISKWIDGET_H

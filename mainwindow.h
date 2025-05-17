#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QToolBar>
#include <QAction>
#include <QToolButton>
#include <QFrame>
#include <QPainter>
#include <QIcon>
#include <QStackedWidget>
#include <QWidget>
#include <QKeyEvent>

#include "macroanalysiswidget.h"
#include "eventriskwidget.h"
#include "eventanalysisdialog.h"
#include "stochasticriskwidget.h"
#include "alertswidget.h"
#include "reportswidget.h"
#include "qcustomplot.h"

class CircularProgressBar : public QWidget {
    Q_OBJECT
public:
    explicit CircularProgressBar(QWidget *parent = nullptr);
    void setValue(int value);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    int m_value = 0;
};

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class UserProfileDialog;
class SettingsDialog;
class LoginDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void setTheme(const QString &themeFile);
    void showAnalytics();
    void showMainScreen();
    void showAnalyzeDialog();
    void showMacroAnalysis();
    void showEventRiskAnalysis();
    void showStochasticRiskAnalysis();
    void handleLogout();
    void openUserProfile();
    void openUserSettings();
    void updateRiskInfo(int riskLevel, int riskChange, const QString &description);
    void handleSearch();
    void showAlerts();
    void showReports();

private:
    void createHeader();
    void createDashboard();
    void createFooter();
    void keyPressEvent(QKeyEvent *event) override;
    void updateDashboardContent();


    QWidget *headerWidget;
    QLineEdit *searchLine;
    QToolButton *notificationsButton;
    QLabel *notifIndicator;
    QToolButton *profileButton;
    LoginDialog *loginDialog = nullptr;
    UserProfileDialog *profileDialog = nullptr;
    SettingsDialog *settingsDialog = nullptr;

    QWidget *dashboardWidget;
    CircularProgressBar *riskCircle;
    QLabel *riskLabel;
    QLabel *changeLabel;
    QLabel *analysisLabel;
    QLabel *riskMapImage;

    QComboBox *regionCombo;
    QComboBox *periodCombo;
    QComboBox *typeCombo;

    QWidget *riskMapWidget;
    QWidget *economicChartWidget;
    QWidget *eventFeedWidget;
    QWidget *topRisksWidget;

    QToolBar *footerToolbar;
    QAction *homeAction;
    QAction *analyticsAction;
    QAction *alertsAction;
    QAction *reportsAction;
    QAction *settingsAction;

    QMenu *settingsMenu;
    QAction *lightThemeAction;
    QAction *darkThemeAction;

    QStackedWidget *stackedWidget;
    AlertsWidget *alertsWidget;
    ReportsWidget *reportsWidget;

    QCustomPlot *economicChartPlot;
    QTextEdit *eventFeedBox;
    QListWidget *topRiskList;

    // экран
    QWidget *mainScreenWidget; // ваш главный экран
    MacroAnalysisWidget *macroWidget;
    // 2 функция
    EventRiskWidget *eventRiskWidget;
    // 3 функция
    StochasticRiskWidget *stochasticWidget;

private:
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H

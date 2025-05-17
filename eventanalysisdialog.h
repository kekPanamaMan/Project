#ifndef EVENTANALYSISDIALOG_H
#define EVENTANALYSISDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QToolBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGraphicsDropShadowEffect>

class EventAnalysisDialog : public QDialog {
    Q_OBJECT
public:
    explicit EventAnalysisDialog(QWidget *parent = nullptr);

    enum AnalysisType {
        None,
        Macro,
        EventRisk
    };

    AnalysisType selectedType() const { return m_selected; }

private:
    AnalysisType m_selected = None;
    QPushButton *macroButton;
    QPushButton *eventRiskButton;

private slots:
    void chooseMacro();
    void chooseEventRisk();
    void goHome();
};

#endif // EVENTANALYSISDIALOG_H

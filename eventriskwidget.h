#ifndef EVENTRISKWIDGET_H
#define EVENTRISKWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QListWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QMap>
#include <QSplitter>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QTimer>
#include <QDialog>
#include <QToolBar>
#include "worldmapwidget.h"

struct GeoCoord {
    double latitude;
    double longitude;
};

class EventRiskWidget : public QWidget {
    Q_OBJECT
public:
    explicit EventRiskWidget(QWidget *parent = nullptr);

private slots:
    void updateFilters();
    void onMapEventClicked(int eventId);
    void addToReport();
    void exportToPDF();
    void exportToExcel();
    void fetchEvents();

signals:
    void goHome(); // Сигнал для возврата на главный экран
    void updateRisk(int value, int change, const QString &description);

private:
    // Фильтры
    QComboBox *typeCombo;
    QComboBox *threatLevelCombo;
    QComboBox *countryCombo;

    // Карта и лента событий
    QListWidget *eventList;
    QPushButton *exportPdfBtn;
    QPushButton *exportExcelBtn;

    // Данные
    struct Event {
        int id;
        QString type;
        QString threatLevel;
        QString country;
        QString description;
        QStringList tags;
        double impactPercent;
        GeoCoord location;
        QString markerColor;
    };


    // Карта
    WorldMapWidget *mapWidget;
    QList<Event> events;
    QList<int> selectedEventIds;

    void setupUI();
    void updateMap();
    void updateEventList(const QList<Event> &filtered);
    void showEventPopup(const Event &event);
    void setStatusText(const QString& text);
};

#endif // EVENTRISKWIDGET_H

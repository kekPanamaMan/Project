#ifndef WORLDMAPWIDGET_H
#define WORLDMAPWIDGET_H

#include <QWidget>
#include <QMap>
#include <QList>
#include <QRect>
#include <QString>

struct CountryEvent {
    QString description;
    QString riskLevel;
    double latitude; // добавить
    double longitude; // добавить
};

class WorldMapWidget : public QWidget {
    Q_OBJECT
public:
    explicit WorldMapWidget(QWidget *parent = nullptr);

    QList<CountryEvent> eventsForCountry(const QString& country) const;
    void setStatusText(const QString& text);

signals:
    void countryEventsChanged(const QString& country, const QList<CountryEvent>& events);
    void anyEventAdded(const QString& country, const CountryEvent& event); // Для синхронизации с лентой событий

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QMap<QString, QRect> countryRects;
    QMap<QString, QList<CountryEvent>> countryEvents;
    QString statusText;
    QPixmap mapPixmap;
    QPointF geoToPixel(double lat, double lon); // <- объявление метода

    QString countryAt(const QPoint& pos) const;
    void addEventToCountry(const QString& country);
};

#endif // WORLDMAPWIDGET_H

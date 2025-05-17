#include "worldmapwidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QInputDialog>
#include <QMessageBox>
#include <QFile>

WorldMapWidget::WorldMapWidget(QWidget *parent) : QWidget(parent) {
    setMinimumSize(800, 500);
    mapPixmap = QPixmap(":/world_map.png"); // добавь файл в ресурсы .qrc
}

void WorldMapWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // 1. Отрисовка карты
    if (!mapPixmap.isNull()) {
        p.drawPixmap(rect(), mapPixmap);
    }

    // 2. Отображение событий как точек на карте
    for (const auto& country : countryEvents.keys()) {
        for (const auto& ev : countryEvents[country]) {
            QPointF pos = geoToPixel(ev.latitude, ev.longitude);

            QColor riskColor = Qt::green;
            if (ev.riskLevel == "Средний") riskColor = QColor(255, 200, 0);
            if (ev.riskLevel == "Высокий") riskColor = Qt::red;

            p.setBrush(riskColor);
            p.setPen(Qt::black);
            p.drawEllipse(pos, 6, 6);
        }
    }

    // 3. Статус
    if (!statusText.isEmpty()) {
        QFont f = p.font();
        f.setPointSize(10);
        p.setFont(f);
        p.setPen(QColor("#1976d2"));
        p.drawText(rect().adjusted(0, 0, 0, -10), Qt::AlignBottom | Qt::AlignHCenter, statusText);
    }
}

QPointF WorldMapWidget::geoToPixel(double lat, double lon) {
    // Пример проекции Меркатора на прямоугольник (ширина карты == виджету)
    double x = (lon + 180.0) / 360.0 * width();
    double y = (90.0 - lat) / 180.0 * height(); // упрощённо
    return QPointF(x, y);
}

void WorldMapWidget::mousePressEvent(QMouseEvent *event) {
    QPoint pos = event->pos();
    double lon = (double(pos.x()) / width()) * 360.0 - 180.0;
    double lat = 90.0 - (double(pos.y()) / height()) * 180.0;

    bool ok = false;
    QString desc = QInputDialog::getText(this, "Новое событие", "Описание:", QLineEdit::Normal, "", &ok);
    if (!ok || desc.trimmed().isEmpty()) return;

    QStringList risks = {"Низкий", "Средний", "Высокий"};
    QString risk = QInputDialog::getItem(this, "Уровень риска", "Выберите уровень:", risks, 1, false, &ok);
    if (!ok) return;

    CountryEvent ev{desc, risk, lat, lon};
    countryEvents["Custom"].append(ev);

    emit anyEventAdded("Custom", ev);
    update();
}

void WorldMapWidget::setStatusText(const QString& text) {
    statusText = text;
    update();
}

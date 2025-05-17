#ifndef USERICON_H
#define USERICON_H

#include <QWidget>

class UserIcon : public QWidget
{
    Q_OBJECT
public:
    explicit UserIcon(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // USERICON_H

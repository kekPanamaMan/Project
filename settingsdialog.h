#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

class QComboBox;

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = nullptr);

signals:
    void themeChanged(const QString &themeFile);

private:
    QComboBox *themeComboBox;
};

#endif // SETTINGSDIALOG_H


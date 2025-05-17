#ifndef SEARCHRESULTSDIALOG_H
#define SEARCHRESULTSDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>

class SearchResultsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SearchResultsDialog(QWidget *parent = nullptr);

    void setResults(const QStringList &results);
    void handleSearch(const QString &query);

private:
    QLabel *resultsLabel;
};

#endif // SEARCHRESULTSDIALOG_H

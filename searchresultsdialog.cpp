#include "searchresultsdialog.h"

SearchResultsDialog::SearchResultsDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Результаты поиска");
    resize(400, 300);

    resultsLabel = new QLabel(this);
    resultsLabel->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(resultsLabel);

    setLayout(layout);
}

void SearchResultsDialog::setResults(const QStringList &results) {
    if (results.isEmpty()) {
        resultsLabel->setText("Ничего не найдено");
    } else {
        resultsLabel->setText(results.join("\n"));
    }
}

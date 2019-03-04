#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTableView>
#include <QStandardItemModel>
#include <QCompleter>
#include "tr098xmlparser.h"

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

    void setResultTable(QList<QStringList> &list);

private slots:
    void onOpenButtonClicked();
    void onConvertButtonClicked();

private:
    QPushButton *openBtn;
    QPushButton *convertBtn;
    QLabel *xmlFileLable;
    QLabel *queryLabel;
    QLineEdit *queryLineEdit;
    QCompleter *completer;
    QTableView *resultTableView;
    QStandardItemModel *resultViewModel;

    QString xmlFilePath;
    Tr098XmlParser *xmlParser;
};

#endif // WIDGET_H

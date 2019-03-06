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
    void onExportButtonClicked();

private:
    void readSettings();
    void writeSettings();

    QPushButton *openBtn;
    QPushButton *convertBtn;
    QPushButton *exportBtn;
    QLabel *xmlFileLable;
    QLabel *queryLabel;
    QLabel *queryNumLabel;
    QLabel *modelNameLabel;
    QLineEdit *queryLineEdit;
    QCompleter *completer;
    QTableView *resultTableView;
    QStandardItemModel *resultViewModel;

    QString xmlFilePath;
    Tr098XmlParser *xmlParser;
    const QStringList mHeaderLabels;
};

#endif // WIDGET_H

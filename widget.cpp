#include "widget.h"
#include <QLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QStringListModel>
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle(tr("XML Full Path Convert Tool"));
    openBtn = new QPushButton(tr("Open"));
    openBtn->setFixedWidth(100);
    convertBtn = new QPushButton(tr("Convert"));
    convertBtn->setFixedWidth(100);
    xmlFileLable = new QLabel;
    queryLabel = new QLabel(tr("Query:"));
    queryLineEdit = new QLineEdit;
    queryLabel->setBuddy(queryLineEdit);
    resultTableView = new QTableView;
//    resultTableView->verticalHeader()->hide();
    resultTableView->horizontalHeader()->setStretchLastSection(true);
    resultViewModel = new QStandardItemModel;
    setResultTable(QList<QStringList>());
    resultTableView->setModel(resultViewModel);
    completer = new QCompleter;
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    queryLineEdit->setCompleter(completer);

    QHBoxLayout *hLayout1 = new QHBoxLayout;
    QHBoxLayout *hLayout2 = new QHBoxLayout;
    QVBoxLayout *vLayout = new QVBoxLayout;
    hLayout1->addWidget(openBtn);
    hLayout1->addWidget(convertBtn);
    hLayout1->addSpacing(20);
    hLayout1->addWidget(xmlFileLable);
    hLayout2->addWidget(queryLabel);
    hLayout2->addWidget(queryLineEdit);
    hLayout2->addStretch();
    vLayout->addLayout(hLayout1);
    vLayout->addLayout(hLayout2);
    vLayout->addWidget(resultTableView);
    setLayout(vLayout);

    connect(openBtn, &QPushButton::clicked, this, &Widget::onOpenButtonClicked);
    connect(convertBtn, &QPushButton::clicked, this, &Widget::onConvertButtonClicked);
    connect(queryLineEdit, &QLineEdit::editingFinished, this, &Widget::onCompleterActivated);

    xmlParser = new Tr098XmlParser;
    setMinimumWidth(resultTableView->width());
}

Widget::~Widget()
{
    delete xmlParser;
}

void Widget::setResultTable(QList<QStringList> list)
{
    int i = 0;
    QStringList headerLabels;

    resultViewModel->clear();
    headerLabels << tr("Parameter") << tr("R/W") << tr("Inform") << tr("Description");
    resultViewModel->setHorizontalHeaderLabels(headerLabels);
    headerLabels.clear();
    foreach (QStringList row, list) {
        headerLabels << QString("%1").arg(i++ + 1);
        QList<QStandardItem *> items;
        foreach (QString text, row)
            items.append(new QStandardItem(text));
        resultViewModel->appendRow(items);
    }
    resultViewModel->setVerticalHeaderLabels(headerLabels);
}

void Widget::onOpenButtonClicked()
{
    xmlFilePath = QFileDialog::getOpenFileName(this, tr("Open XML File"), QString("."), tr("XML files (*.xml)"));
    xmlFileLable->setText(xmlFilePath);
}

void Widget::onConvertButtonClicked()
{
    if(!xmlParser->openXmlDoc(xmlFilePath))
    {
        QMessageBox::warning(this, tr("Warning"), xmlParser->errorString());
        return;
    }
    if(!xmlParser->parseXmlDoc())
    {
        QMessageBox::warning(this, tr("Warning"), xmlParser->errorString());
    }
    else
    {
        setResultTable(xmlParser->xmlAttrList());
        QStringListModel *stringListModel = new QStringListModel;
        stringListModel->setStringList(xmlParser->xmlPathList());
        completer->setModel(stringListModel);
    }
    xmlParser->closeXmlDoc();
}

void Widget::onCompleterActivated()
{
    QStringListModel *stringListModel = qobject_cast<QStringListModel *>(completer->completionModel());
    qDebug() << stringListModel->stringList();
//    QStringList completerStringList = completer->
}

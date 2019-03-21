#include "widget.h"
#include <QLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QStringListModel>
#include <QApplication>
#include <QClipboard>
#include <QTimer>
#include <QDebug>


Widget::Widget(QWidget *parent)
    : QWidget(parent),
      mHeaderLabels(QStringList() << tr("Parameter") << tr("R/W") << tr("Inform") << tr("Description"))
{
    setWindowTitle(tr("XML Full Path Convert Tool"));
    openBtn = new QPushButton(tr("Open"));
    openBtn->setFixedWidth(100);
    convertBtn = new QPushButton(tr("Convert"));
    convertBtn->setFixedWidth(100);
    exportBtn = new QPushButton(tr("Export"));
    exportBtn->setFixedWidth(100);
    xmlFileLable = new QLabel;
    queryLabel = new QLabel(tr("Query:"));
    queryLineEdit = new QLineEdit;
    queryLabel->setBuddy(queryLineEdit);
    queryNumLabel = new QLabel;
    modelNameLabel = new QLabel;
    resultTableView = new QTableView;
    resultTableView->horizontalHeader()->setStretchLastSection(true);
    resultViewModel = new QStandardItemModel;
    resultTableView->setModel(resultViewModel);
    resultTableView->setSelectionMode(QTableView::ContiguousSelection);
    completer = new QCompleter;
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    completer->setModel(resultViewModel);
    queryLineEdit->setCompleter(completer);

    copyAction = new QAction(tr("&Copy"), this);
    copyAction->setShortcut(QKeySequence::Copy);
    copyAction->setStatusTip(tr("Copy the current selection's contents to the clipboard"));
    connect(copyAction, &QAction::triggered, this, &Widget::copy);
    resultTableView->addAction(copyAction);
    resultTableView->setContextMenuPolicy(Qt::ActionsContextMenu);

    QHBoxLayout *hLayout1 = new QHBoxLayout;
    QHBoxLayout *hLayout2 = new QHBoxLayout;
    QVBoxLayout *vLayout = new QVBoxLayout;
    hLayout1->addWidget(openBtn);
    hLayout1->addWidget(convertBtn);
    hLayout1->addSpacing(20);
    hLayout1->addWidget(xmlFileLable);
    hLayout1->addStretch();
    hLayout1->addWidget(exportBtn);
    hLayout2->addWidget(queryLabel);
    hLayout2->addWidget(queryLineEdit);
    hLayout2->addWidget(queryNumLabel);
    hLayout2->addStretch();
    hLayout2->addWidget(modelNameLabel);
    vLayout->addLayout(hLayout1);
    vLayout->addLayout(hLayout2);
    vLayout->addWidget(resultTableView);
    setLayout(vLayout);

    connect(openBtn, &QPushButton::clicked, this, &Widget::onOpenButtonClicked);
    connect(convertBtn, &QPushButton::clicked, this, &Widget::startConvertXml);
    connect(exportBtn, &QPushButton::clicked, this, &Widget::onExportButtonClicked);
    connect(queryLineEdit, &QLineEdit::returnPressed,
            [=](){
        if(queryLineEdit->text().length() == 0)
            resultTableView->setModel(resultViewModel);
        else
            resultTableView->setModel(completer->completionModel());
        queryNumLabel->setText(tr("Matched: %1").arg(resultTableView->model()->rowCount()));
    });
    connect(completer, QOverload<const QString &>::of(&QCompleter::activated),
            [=](const QString &text){
        completer->setCompletionPrefix(text);
        resultTableView->setModel(completer->completionModel());
        queryNumLabel->setText(tr("Matched: %1").arg(resultTableView->model()->rowCount()));
    });

    xmlParser = new Tr098XmlParser;
    setResultTable(xmlParser->xmlAttrList());
    setMinimumWidth(resultTableView->width());

    QTimer::singleShot(0, this, &Widget::loadDelay);
}

Widget::~Widget()
{
    delete xmlParser;
}

void Widget::setResultTable(QList<QStringList> &list)
{
    int i = 0, col = 1;
    QStringList headerLabels;

    resultViewModel->clear();
    resultViewModel->setHorizontalHeaderLabels(mHeaderLabels);
    foreach (QStringList row, list) {
        headerLabels << QString("%1").arg(i++ + 1);
        QList<QStandardItem *> items;
        col = 1;
        foreach (QString text, row) {
            QStandardItem *item = new QStandardItem(text);
            if(col == 2 || col == 3)
                item->setTextAlignment(Qt::AlignCenter);
            items.append(item);
            ++col;
        }
        resultViewModel->appendRow(items);
    }
    resultViewModel->setVerticalHeaderLabels(headerLabels);
    resultTableView->resizeColumnsToContents();
}

void Widget::copy()
{
    QString str;
    QModelIndexList modelIndexList = resultTableView->selectionModel()->selectedIndexes();
    int lastCol = modelIndexList.last().column();
    foreach (QModelIndex index, modelIndexList) {
        str += index.data(Qt::EditRole).toString();
        if(index.column() == lastCol)
            str += "\n";
        else
            str += "\t";
    }
    QApplication::clipboard()->setText(str);
}

void Widget::onOpenButtonClicked()
{
    xmlFilePath = QFileDialog::getOpenFileName(this, tr("Open XML File"), xmlFilePath, tr("XML files (*.xml)"));
    xmlFileLable->setText(xmlFilePath);
}

void Widget::startConvertXml()
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
        xmlParser->preprocessData();
        setResultTable(xmlParser->xmlAttrList());
        modelNameLabel->setText(tr("Data Model: %1").arg(xmlParser->modelName()));
        queryNumLabel->setText(tr("Matched: %1").arg(resultTableView->model()->rowCount()));
        writeSettings();
    }
    xmlParser->closeXmlDoc();
}

void Widget::onExportButtonClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), QString("untitiled.xlsx"), tr("Microsoft Excel Workbook (*.xlsx)"));
    if(fileName.isNull())
        return;
    exportExcel(fileName);
}

void Widget::loadDelay()
{
    QStringList args = QApplication::arguments();
    if(args.size() >= 2)
    {
        xmlFilePath = args[1];
        if(xmlFilePath.endsWith(".xml")) {
            startConvertXml();
        } else {
            QMessageBox::warning(this, tr("Warning"), tr("Please open xml file"));
        }
    }
    else
        readSettings();
    xmlFileLable->setText(xmlFilePath);
}

void Widget::readSettings()
{
    QSettings settings("Settings.ini", QSettings::IniFormat);
    xmlFilePath = settings.value("Path").toString();
}

void Widget::writeSettings()
{
    QSettings settings("Settings.ini", QSettings::IniFormat);
    settings.setValue("Path", xmlFilePath);
}

void Widget::exportExcel(const QString &fileName)
{
    Document xlsx;
    QFontMetrics fontMetrics(resultTableView->font());
    QAbstractItemModel *model = resultTableView->model();

    xlsx.addSheet(QObject::tr("TR069 Data Model"));
    xlsx.setDocumentProperty("title", "TR069 Data Model");
    xlsx.setDocumentProperty("subject", "");
    xlsx.setDocumentProperty("creator", "zxl");
    xlsx.setDocumentProperty("company", "");
    xlsx.setDocumentProperty("category", "XML file");
    xlsx.setDocumentProperty("keywords", "TR069");
    xlsx.setDocumentProperty("description", "Created by XmlPathOutput Tool");

    xlsx.setRowHeight(1, 20);
    for(int col = 1; col <= model->columnCount(); col++)
    {
        xlsx.setColumnWidth(col, fontMetrics.width(mHeaderLabels[col - 1]));
        xlsx.write(1, col, mHeaderLabels[col - 1], cellFormat(16, Qt::yellow, Format::AlignHCenter));
    }
    for(int row = 2; row <= model->rowCount() + 1; row++)
    {
        xlsx.setRowHeight(row, 16);
        for(int col = 1; col <= model->columnCount(); col++)
        {
            if(col == 2 || col == 3)
                xlsx.write(row, col, model->data(model->index(row - 2, col - 1)), cellFormat(12, Qt::gray, Format::AlignHCenter));
            else
                xlsx.write(row, col, model->data(model->index(row - 2, col - 1)), cellFormat(12, Qt::gray, Format::AlignLeft));
        }
    }
    xlsx.saveAs(fileName);
}

Format Widget::cellFormat(int fontSize, const QColor &color, Format::HorizontalAlignment align)
{
    Format format;
    format.setFontSize(fontSize);
    format.setPatternBackgroundColor(color);
    format.setHorizontalAlignment(align);
    format.setVerticalAlignment(Format::AlignVCenter);
    format.setBorderStyle(Format::BorderThin);
    return format;
}

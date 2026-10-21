#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    labNodeText = new QLabel("节点标题", this);
    labNodeText->setMinimumWidth(200);
    ui->statusBar->addWidget(labNodeText);

    spinRatio = new QSpinBox(this);
    spinRatio->setRange(0, 2000);
    spinRatio->setValue(100);
    spinRatio->setSuffix(" %");
    spinRatio->setReadOnly(true);
    spinRatio->setButtonSymbols(QAbstractSpinBox::NoButtons);
    ui->statusBar->addWidget(spinRatio);

    labFileName = new QLabel("文件名", this);
    ui->statusBar->addWidget(labFileName);

    buildTreeHeader();
    iniTree();
    setCentralWidget(ui->scrollArea);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::buildTreeHeader()
{
    ui->treeFiles->clear(); // 清除所有节点，但不改变表头
    QTreeWidgetItem *header = new QTreeWidgetItem();
    header->setText(MainWindow::colItem,        "目录和文件");
    header->setText(MainWindow::colItemType,    "节点类型");
    header->setText(MainWindow::colDate,        "最后修改日期");
    header->setTextAlignment(colItem, Qt::AlignHCenter | Qt::AlignVCenter);
    header->setTextAlignment(colItemType, Qt::AlignHCenter | Qt::AlignVCenter);
    ui->treeFiles->setHeaderItem(header);

}

void MainWindow::iniTree()
{
    QIcon icon(":/images/icons/15.ico");
    QTreeWidgetItem *item = new QTreeWidgetItem(MainWindow::itTopItem);
    item->setIcon(MainWindow::colItem, icon);
    item->setText(MainWindow::colItem, "图片");
    item->setText(MainWindow::colItemType, "Top Item");
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable
                   | Qt::ItemIsEnabled | Qt::ItemIsAutoTristate);
    item->setCheckState(MainWindow::colItem, Qt::Checked);
    ui->treeFiles->addTopLevelItem(item);
}

void MainWindow::addFolderItem(QTreeWidgetItem *parItem, QString dirName)
{
    // 添加一个分组节点
    QIcon icon(":/images/icons/open3.bmp");
    QString NodeText = getFinalFolderName(dirName);
    QTreeWidgetItem *item = new QTreeWidgetItem(MainWindow::itGroupItem);
    item->setIcon(colItem, icon);
    item->setText(colItem, NodeText);
    item->setText(colItemType, "Group Item");
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable
                   | Qt::ItemIsEnabled | Qt::ItemIsAutoTristate);
    item->setCheckState(colItem, Qt::Checked);
    item->setData(colItem, Qt::UserRole, QVariant(dirName));
    parItem->addChild(item);

}

QString MainWindow::getFinalFolderName(const QString &fullPathName)
{
    int cnt = fullPathName.length();
    int i = fullPathName.lastIndexOf("/");
    QString str = fullPathName.right(cnt - i - 1);
    return str;
}

void MainWindow::addImageItem(QTreeWidgetItem *parItem, QString aFilename)
{
    // 添加一个图片节点
    QIcon icon(":/images/icons/31.ico");
    QFileInfo fileInfo(aFilename);
    QString NodeText = fileInfo.fileName();
    QDateTime birthDate = fileInfo.birthTime();

    QTreeWidgetItem *item = new QTreeWidgetItem(MainWindow::itImageItem);
    item->setIcon(colItem, icon);
    item->setText(colItem, NodeText);
    item->setText(colItemType, "Image Item");
    item->setText(MainWindow::colDate, birthDate.toString("yyyy-MM-dd"));
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable
                   | Qt::ItemIsEnabled | Qt::ItemIsAutoTristate);
    item->setCheckState(colItem, Qt::Checked);
    item->setData(colItem, Qt::UserRole, QVariant(aFilename));
    parItem->addChild(item);
}

void MainWindow::displayImage(QTreeWidgetItem *item)
{
    QString filename = item->data(colItem, Qt::UserRole).toString();
    labFileName->setText(filename);
    labNodeText->setText(item->text(colItem));
    m_pixmap.load(filename);

    ui->actZoomFitH->setEnabled(true);
    ui->actZoomFitH->trigger();
    ui->actZoomFitW->setEnabled(true);
    ui->actZoomIn->setEnabled(true);
    ui->actZoomOut->setEnabled(true);
    ui->actZoomRealSize->setEnabled(true);
}

void MainWindow::changeItemCaption(QTreeWidgetItem *item)
{
    QString str = "*" + item->text(colItem);
    item->setText(colItem, str);
    if (item->childCount() > 0) {
        for (int i = 0; i < item->childCount(); ++i) {
            changeItemCaption(item->child(i));
        }
    }
}

void MainWindow::deleteItem(QTreeWidgetItem *parItem, QTreeWidgetItem *item)
{
    // 递归删除
    if (item->childCount() > 0) {
        int count = item->childCount();
        QTreeWidgetItem *tempParItem = item;
        for (int i = count - 1; i >= 0; --i) {
            deleteItem(tempParItem, tempParItem->child(i));
        }
    }
    parItem->removeChild(item);
    delete item;
}

void MainWindow::on_actAddFolder_triggered()
{
    // 添加目录
    QString dir = QFileDialog::getExistingDirectory();
    if (dir.isEmpty()) return;
    QTreeWidgetItem *parItem = ui->treeFiles->currentItem();
    if (parItem == nullptr) return;
    if (parItem->type() != MainWindow::itImageItem)
        addFolderItem(parItem, dir);
}


void MainWindow::on_actAddFiles_triggered()
{
    QStringList files = QFileDialog::getOpenFileNames(this, "选择文件", "", "Images(*.jpg *.jpeg *.png)");
    if (files.isEmpty()) return;
    QTreeWidgetItem *parItem, *item;
    item = ui->treeFiles->currentItem();
    if (item == nullptr)
        item = ui->treeFiles->topLevelItem(0);

    if (item->type() == MainWindow::itImageItem)
        // 如果当前节点时图片节点，取其父节点
        parItem = item->parent();
    else
        parItem = item;
    for (int i = 0; i < files.size(); ++i) {
        QString aFilename = files.at(i);
        addImageItem(parItem, aFilename);
    }
    parItem->setExpanded(true);
}


void MainWindow::on_treeFiles_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous);
    qDebug("currentItemChanged() is emitted");
    if (current == nullptr) return;
    if (current == previous) return; // 没有切换节点，只是列变化

    int var = current->type();
    switch(var) {
    case MainWindow::itTopItem:
        ui->actAddFolder->setEnabled(true);
        ui->actAddFiles->setEnabled(true);
        ui->actDeleteItem->setEnabled(false);
        break;
    case MainWindow::itGroupItem:
        ui->actAddFolder->setEnabled(true);
        ui->actAddFiles->setEnabled(true);
        ui->actDeleteItem->setEnabled(true);
        break;
    case MainWindow::itImageItem:
        ui->actAddFolder->setEnabled(false);
        ui->actAddFiles->setEnabled(true);
        ui->actDeleteItem->setEnabled(true);
        displayImage(current);
    }
}


void MainWindow::on_actDeleteItem_triggered()
{
    QTreeWidgetItem *item = ui->treeFiles->currentItem();
    if (item == nullptr) return;
    QTreeWidgetItem *parItem = item->parent();
    deleteItem(parItem, item);
}


void MainWindow::on_actScanItems_triggered()
{
    for (int i = 0; i < ui->treeFiles->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = ui->treeFiles->topLevelItem(i); // 虽然此项目中顶层节点只有一个，但是这里写通用方法
        changeItemCaption(item);
    }
}


void MainWindow::on_actZoomFitH_triggered()
{
    int H = ui->scrollArea->height();
    int realH = m_pixmap.height();
    m_ratio = float(H) / realH;
    spinRatio->setValue(100 * m_ratio);
    QPixmap pix = m_pixmap.scaledToHeight(H-30);
    ui->labPic->setPixmap(pix);
}


void MainWindow::on_actZoomIn_triggered()
{
    m_ratio = m_ratio * 1.2;
    spinRatio->setValue(100 * m_ratio);
    int w = m_ratio * m_pixmap.width();
    int h = m_ratio * m_pixmap.height();
    QPixmap pix = m_pixmap.scaled(w, h);
    ui->labPic->setPixmap(pix);
}


void MainWindow::on_actZoomOut_triggered()
{
    m_ratio = m_ratio * 0.8;
    spinRatio->setValue(100 * m_ratio);
    int w = m_ratio * m_pixmap.width();
    int h = m_ratio * m_pixmap.height();
    QPixmap pix = m_pixmap.scaled(w, h);
    ui->labPic->setPixmap(pix);
}


void MainWindow::on_actZoomRealSize_triggered()
{
    m_ratio = 1;
    spinRatio->setValue(100);
    ui->labPic->setPixmap(m_pixmap);
}


void MainWindow::on_actZoomFitW_triggered()
{
    int W = ui->scrollArea->width();
    int realW = m_pixmap.width();
    m_ratio = float(W) / realW;
    spinRatio->setValue(100 * m_ratio);
    QPixmap pix = m_pixmap.scaledToHeight(W-30);
    ui->labPic->setPixmap(pix);
}


void MainWindow::on_actDockVisible_triggered(bool checked)
{
    ui->dockWidget->setVisible(checked);
}


void MainWindow::on_actDockFloat_triggered(bool checked)
{
    ui->dockWidget->setFloating(checked);
}


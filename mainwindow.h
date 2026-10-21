#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include <QSpinBox>
#include <QTreeWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    enum TreeItemType {itTopItem = 1001, itGroupItem, itImageItem};
    enum TreeColNum {colItem = 0, colItemType, colDate};
    QLabel *labFileName;
    QLabel *labNodeText;
    QSpinBox *spinRatio;
    QPixmap m_pixmap;
    float m_ratio;

    void buildTreeHeader();
    void iniTree();
    void addFolderItem(QTreeWidgetItem *parItem, QString dirName);
    QString getFinalFolderName(const QString &fullPathName);
    void addImageItem(QTreeWidgetItem *parItem, QString aFilename);
    void displayImage(QTreeWidgetItem *item);
    void changeItemCaption(QTreeWidgetItem *item);
    void deleteItem(QTreeWidgetItem *parItem, QTreeWidgetItem *item);
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actAddFolder_triggered();

    void on_actAddFiles_triggered();

    void on_treeFiles_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_actDeleteItem_triggered();

    void on_actScanItems_triggered();

    void on_actZoomFitH_triggered();

    void on_actZoomIn_triggered();

    void on_actZoomOut_triggered();

    void on_actZoomRealSize_triggered();

    void on_actZoomFitW_triggered();

    void on_actDockVisible_triggered(bool checked);

    void on_actDockFloat_triggered(bool checked);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QTreeWidget>
#include <QTextBrowser>
#include <QPushButton>
#include <QJsonObject>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSearchTextChanged(const QString &text);
    void onTreeItemClicked(QTreeWidgetItem *item, int column);
    void onCopyButtonClicked();
    void updateLanguage(); // 仅保留这一个声明

private:
    Ui::MainWindow *ui;
    QLineEdit *searchEdit;
    QTreeWidget *sqlTree;
    QTextBrowser *detailBrowser;
    QPushButton *copyButton;
    QPushButton *languageButton; // 新的语言按钮
    QJsonObject sqlData;
    QString currentLang;

    void loadSqlData();
};

#endif // MAINWINDOW_H
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QStandardItemModel;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private slots:
    void on_actionSettings_triggered();

    void on_action_Scan_Computer_triggered();

private:
    void readSettings();
    void writeSettings();
    Ui::MainWindow *ui;
    QStandardItemModel *model;
};

#endif // MAINWINDOW_H

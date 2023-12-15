#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "DockManager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void OnCustMenuTriggered();
private:
    Ui::MainWindow *ui;
    ads::CDockManager* m_DockManager;
    ads::CDockWidget* DockWidget;
};

#endif // MAINWINDOW_H

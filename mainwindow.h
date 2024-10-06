#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void update_ui();
    void update_fila();
    void on_patientButton_clicked();
    void gerar_relatorio();

    void on_cpfLineEdit_editingFinished();

    void on_consultarPatient_clicked();

    void on_pushButton_clicked();

    void on_removePatient_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H

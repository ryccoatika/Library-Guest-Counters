    #ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString getLastIDGuest();

private slots:
    void keyPressEvent(QKeyEvent *event);

    void on_loginButton_clicked();

    void on_loginGuestButton_clicked();

    void on_noIDEdit_returnPressed();

    void on_lanjutLoginButton_clicked();

    void on_kembaliButton_clicked();

    void on_loginTamuButton_clicked();

    void on_namaTamuEdit_returnPressed();

    void on_instansiTamuEdit_returnPressed();

    void on_kembaliButton_2_clicked();

private:
    Ui::MainWindow *ui;
    QString id;
    QString waktu;
    QString tgl;
    QString hostname = "10.84.5.200";
    QString dbname = "dbperpustakaan";
    QString user = "root";
    QString pass = "unsoed2018";
};

#endif // MAINWINDOW_H

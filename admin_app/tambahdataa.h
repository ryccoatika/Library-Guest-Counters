#ifndef TAMBAHDATAA_H
#define TAMBAHDATAA_H

#include <QDialog>
#include <QDir>
#include <QMessageBox>

namespace Ui {
class tambahDataA;
}

class tambahDataA : public QDialog
{
    Q_OBJECT

public:
    explicit tambahDataA(QWidget *parent = 0);
    ~tambahDataA();

private slots:
    void on_tambahButton_clicked();

    void on_kembaliButton_clicked();

    void on_getFoto_clicked();

    void on_noIDEdit_textChanged(const QString &arg1);

private:
    Ui::tambahDataA *ui;
    QDir dir;
    QString file;
    QMessageBox *msgbox;
};

#endif // TAMBAHDATAA_H

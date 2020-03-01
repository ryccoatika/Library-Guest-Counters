#ifndef EDITDATAA_H
#define EDITDATAA_H

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>

namespace Ui {
class editdataa;
}

class editdataa : public QDialog
{
    Q_OBJECT

public:
    explicit editdataa(QWidget *parent = 0);
    void setId(QString id);
    ~editdataa();

private slots:
    void on_getFoto_clicked();

    void on_kembaliButton_clicked();

    void on_simpanButton_clicked();

private:
    Ui::editdataa *ui;
    QString file;
    QString orifile;
    QMessageBox *msgbox;
};

#endif // EDITDATAA_H

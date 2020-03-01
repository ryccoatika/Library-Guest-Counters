#include "tambahdataa.h"
#include "ui_tambahdataa.h"

#include <QFileDialog>
#include <QSqlQuery>

tambahDataA::tambahDataA(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tambahDataA)
{
    ui->setupUi(this);
}

tambahDataA::~tambahDataA()
{
    delete msgbox;
    delete ui;
}

void tambahDataA::on_tambahButton_clicked()
{
    if(ui->noIDEdit->text().trimmed().isEmpty() || ui->namaEdit->text().trimmed().isEmpty() || ui->alamatEdit->text().trimmed().isEmpty()){
        QMessageBox::information(this,"Tambah Anggota","No ID, Nama, Alamat, dan Status harus diisi");
        ui->noIDEdit->setFocus();
        return;
    }
    QString id = ui->noIDEdit->text();
    QString nama = ui->namaEdit->text();
    QString alamat = ui->alamatEdit->text();
    QString status = ui->tipeAnggotaPilih->currentText();
    QStringList filename = file.split("/");
    QPixmap pixmap;
    if(file.size() == 0)
        pixmap = QPixmap(":/user.jpg");
    else
        pixmap = QPixmap(file);

    pixmap = pixmap.scaled(100,100,Qt::KeepAspectRatio);
    msgbox = new QMessageBox(this);
    msgbox->setText(" No ID       : "+id+"\n Nama      : "+nama+"\n Alamat    : "+alamat+"\n Status      : "+status);
    msgbox->setWindowTitle("Tambah Anggota");
    msgbox->setIconPixmap(pixmap);
    msgbox->setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
    if(msgbox->exec() == QMessageBox::Save){
        QSqlQuery query;
        query.prepare("INSERT INTO daftaranggota (noID, nama, alamat, jenisAnggota,foto) VALUES (?,?,?,?,?)");
        query.addBindValue(id);
        query.addBindValue(nama);
        query.addBindValue(alamat);
        query.addBindValue(status);
        if(!file.trimmed().isNull())
            query.addBindValue(filename.last());
        else
            query.addBindValue("");
        if(query.exec()){
            if(!file.trimmed().isNull()){
                if(!QDir("imgAnggota").exists()){
                    dir = QDir(QDir::currentPath());
                    dir.mkdir("imgAnggota");
                }
                if(QFile::copy(file,"imgAnggota/"+filename.last())){
                    QMessageBox::information(this,"Tambah Anggota","Berhasil menambahkan anggota");
                    this->close();
                }
            }else{
                QMessageBox::information(this,"Tambah Anggota","Berhasil menambahkan anggota");
                this->close();
            }
        }
    }
}

void tambahDataA::on_kembaliButton_clicked()
{
    this->close();
}

void tambahDataA::on_getFoto_clicked()
{
    back:
    file = QFileDialog::getOpenFileName(this,"Pilih Foto","","(*.jpg);;(*.png);;(*.jpeg)");
    if(file.trimmed().size() == 0) return;
    QStringList filename = file.split("/");
    if(QFile("imgAnggota/"+filename.last()).exists()){
        QMessageBox::warning(this,"Peringatan","Foto sudah ada");
        goto back;
    }
    ui->getFoto->setText(filename.last());
}

void tambahDataA::on_noIDEdit_textChanged(const QString &arg1)
{
    ui->noIDEdit->setText(arg1.toUpper());
    QSqlQuery query;
    QString sql = "SELECT noID FROM daftaranggota";
    query.exec(sql);
    QStringList daftarID;
    while(query.next())
        daftarID.append(query.value(0).toString());

    for(int i = 0; i < daftarID.length(); i++){
        if(arg1 == daftarID.at(i))
            QMessageBox::information(this,"Tambah Anggota","ID sudah terdaftar");
    }
}

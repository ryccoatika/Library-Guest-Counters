#include "editdataa.h"
#include "ui_editdataa.h"

#include <QSqlQuery>
#include <QDir>

editdataa::editdataa(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::editdataa)
{
    ui->setupUi(this);
}

editdataa::~editdataa()
{
    delete msgbox;
    delete ui;
}

void editdataa::setId(QString id)
{
    QString ID = id;
    QSqlQuery query;
    query.prepare("SELECT * FROM daftaranggota WHERE noID = ?");
    query.addBindValue(ID);
    query.exec();
    if(query.next()){
        ui->noIDEdit->setText(query.value(0).toString());
        ui->namaEdit->setText(query.value(1).toString());
        ui->alamatEdit->setText(query.value(2).toString());
        ui->tipeAnggotaPilih->setCurrentText(query.value(3).toString());
        if(!query.value(4).isNull()){
            ui->getFoto->setText(query.value(4).toString());
            file = "imgAnggota/"+query.value(4).toString();
            orifile = file;
        }
    }
}

void editdataa::on_getFoto_clicked()
{
    back:
    file = QFileDialog::getOpenFileName(this,"Pilih Foto","","(*.jpg);;(*.png);;(*.jpeg)");
    if(file.trimmed().size() == 0) return;
    QStringList filename = file.split("/");
    if(orifile == "imgAnggota/"+filename.last()){
        QMessageBox::information(this,"Informasi","File yang dipilih sama dengan file sebelumnya");
    }else if(QFile("imgAnggota/"+filename.last()).exists()){
        QMessageBox::warning(this,"Peringatan","Foto sudah ada");
        goto back;
    }
    ui->getFoto->setText(filename.last());
}

void editdataa::on_kembaliButton_clicked()
{
    this->close();
}

void editdataa::on_simpanButton_clicked()
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
        query.prepare("UPDATE daftaranggota SET noID=?, nama=?, alamat=?, jenisAnggota=?, foto=? WHERE noID = ?");
        query.addBindValue(id);
        query.addBindValue(nama);
        query.addBindValue(alamat);
        query.addBindValue(status);
        query.addBindValue(filename.last());
        query.addBindValue(id);
        if(query.exec()){
            if(file == orifile || file.isEmpty()){
                QMessageBox::information(this,"Edit Anggota","Edit data anggota berhasil");
                this->close();
            }else{
                if(QFile::copy(file,"imgAnggota/"+filename.last())){
                    QFile::remove(orifile);
                    QMessageBox::information(this,"Edit Anggota","Edit data anggota berhasil");
                    this->close();
                }
            }
        }
    }
}

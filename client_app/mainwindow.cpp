#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QTime>
#include <QDesktopWidget>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(hostname);
    db.setDatabaseName(dbname);
    db.setUserName(user);
    db.setPassword(pass);
    if(! db.open()){
        QMessageBox::critical(this,"Kesalahan","Database error : "+db.lastError().text());
        return;
    }
    ui->setupUi(this);
    int x = (QApplication::desktop()->screen()->width() / 2) - (ui->stackedWidget->width() / 2);
    int y = (QApplication::desktop()->screen()->height() / 2) - (ui->stackedWidget->height() / 2);
    ui->stackedWidget->move(x,y);
    ui->stackedWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape){
        if(this->isFullScreen())
            this->showMaximized();
    }else if(event->key() == Qt::Key_F11){
        if(!this->isFullScreen())
            this->showFullScreen();
    }
}

void MainWindow::on_loginButton_clicked()
{
    if(ui->noIDEdit->text().trimmed().isEmpty()){
        QMessageBox::information(this,"Informasi","Isi dengan nama atau no ID");
        ui->noIDEdit->setFocus();
        return;
    }

    id = ui->noIDEdit->text();
    QSqlQuery query;
    query.prepare("SELECT * FROM daftaranggota WHERE noID = ? OR nama LIKE ?");
    query.addBindValue(id);
    query.addBindValue("%"+id+"%");
    query.exec();
    if(query.next()){
        ui->noIDEdit->clear();
        ui->stackedWidget->setCurrentWidget(ui->userPage);
        id = query.value(0).toString();
        QString nama = query.value(1).toString();
        QString alamat = query.value(2).toString();
        QString status = query.value(3).toString();
        waktu = QTime::currentTime().toString("hh:mm:ss");
        tgl = QDate::currentDate().toString("yyyy:MM:dd");

        ui->noIDLabel->setText(id);
        ui->namaLabel->setText(nama);
        ui->alamatLabel->setText(alamat);
        ui->statusLabel->setText(status);
    }else{
        QMessageBox::information(this,"Login",ui->noIDEdit->text()+" tidak terdaftar");
    }
}

void MainWindow::on_loginGuestButton_clicked()
{
    ui->namaTamuEdit->clear();
    ui->instansiTamuEdit->clear();
    ui->stackedWidget->setCurrentWidget(ui->loginGuest);
}

void MainWindow::on_noIDEdit_returnPressed()
{
    this->on_loginButton_clicked();
}

void MainWindow::on_lanjutLoginButton_clicked()
{
    QSqlQuery query;
    query.prepare("SELECT jamKunjung FROM daftarpengunjung WHERE daftarAnggota_noID = ? AND tglKunjung = ? ORDER BY jamKunjung DESC");
    query.addBindValue(id);
    query.addBindValue(QDate::currentDate().toString("yyyy-MM-dd"));
    query.exec();
    query.next();
    if(query.size() == 0){
        query.clear();
        query.prepare("INSERT INTO daftarpengunjung (daftarAnggota_noID, jamKunjung, tglKunjung) VALUES (?,?,?)");
        query.addBindValue(id);
        query.addBindValue(waktu);
        query.addBindValue(tgl);
        if(query.exec()){
            QMessageBox::information(this,"Berhasil","Login berhasil");
            ui->stackedWidget->setCurrentIndex(0);
        }
    }else{
        if(QMessageBox::information(this,"Informasi","Anda sudah login jam "+QTime(query.value(0).toTime()).toString("hh:mm")+".\nApakah anda yakin ingin login kembali?",QMessageBox::Yes | QMessageBox::Cancel) == QMessageBox::Cancel){
            ui->stackedWidget->setCurrentIndex(0);
            return;
        }
        query.clear();
        query.prepare("INSERT INTO daftarpengunjung (daftarAnggota_noID, jamKunjung, tglKunjung) VALUES (?,?,?)");
        query.addBindValue(id);
        query.addBindValue(waktu);
        query.addBindValue(tgl);
        if(query.exec()){
            QMessageBox::information(this,"Berhasil","Login berhasil");
            ui->stackedWidget->setCurrentIndex(0);
        }
    }
}

void MainWindow::on_kembaliButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_loginTamuButton_clicked()
{
    if(ui->namaTamuEdit->text().trimmed().isEmpty() || ui->instansiTamuEdit->text().trimmed().isEmpty()){
        QMessageBox::information(this,"Login Tamu","Mohon masukkan nama dan instansi anda !");
        return;
    }
    QString nama = ui->namaTamuEdit->text();
    QString instansi = ui->instansiTamuEdit->text();
    waktu = QTime::currentTime().toString("hh:mm:ss");
    tgl = QDate::currentDate().toString("yyyy:MM:dd");
    QSqlQuery query;
    query.prepare("SELECT daftarpengunjung.jamKunjung, daftartamu.noID FROM daftarpengunjung INNER JOIN daftartamu ON daftarpengunjung.daftarAnggota_noID = daftartamu.noID WHERE daftartamu.nama LIKE ? AND daftartamu.instansi LIKE ? AND daftarpengunjung.tglKunjung = ? ORDER BY daftarpengunjung.jamKunjung ASC");
    query.addBindValue("%"+nama+"%");
    query.addBindValue("%"+instansi+"%");
    query.addBindValue(QDate::currentDate().toString("yyyy-MM-dd"));
    query.exec();
    query.next();
    if(query.size() == 0){
        query.clear();
        query.prepare("SELECT noID FROM daftartamu WHERE nama LIKE ? AND instansi LIKE ?");
        query.addBindValue("%"+nama+"%");
        query.addBindValue("%"+instansi+"%");
        query.exec();
        query.next();
        if(query.size() == 0){
            QString id = this->getLastIDGuest();
            query.clear();
            query.prepare("INSERT INTO daftartamu (noID, nama, instansi) VALUES (?,?,?)");
            query.addBindValue(id);
            query.addBindValue(nama);
            query.addBindValue(instansi);
            query.exec();
            query.clear();
            query.prepare("INSERT INTO daftarpengunjung (daftarAnggota_noID, jamKunjung, tglKunjung) VALUES (?,?,?)");
            query.addBindValue(id);
            query.addBindValue(waktu);
            query.addBindValue(tgl);
        }else{
            QString id = query.value(0).toString();
            query.clear();
            query.prepare("INSERT INTO daftarpengunjung (daftarAnggota_noID, jamKunjung, tglKunjung) VALUES (?,?,?)");
            query.addBindValue(id);
            query.addBindValue(waktu);
            query.addBindValue(tgl);
        }

        if(query.exec()){
            QMessageBox::information(this,"Berhasil","Login berhasil");
            ui->stackedWidget->setCurrentIndex(0);
        }
    }else{
        if(QMessageBox::information(this,"Informasi","Anda sudah login jam "+QTime(query.value(0).toTime()).toString("hh:mm")+".\nApakah anda yakin ingin login kembali?",QMessageBox::Yes | QMessageBox::Cancel) == QMessageBox::Cancel){
            ui->stackedWidget->setCurrentIndex(0);
            return;
        }
        QString id = query.value(1).toString();
        query.clear();
        query.prepare("INSERT INTO daftarpengunjung (daftarAnggota_noID, jamKunjung, tglKunjung) VALUES (?,?,?)");
        query.addBindValue(id);
        query.addBindValue(waktu);
        query.addBindValue(tgl);
        if(query.exec()){
            QMessageBox::information(this,"Berhasil","Login berhasil");
            ui->stackedWidget->setCurrentIndex(0);
        }
    }
}

QString MainWindow::getLastIDGuest()
{
    QSqlQuery query;
    QString lastID;
    query.exec("SELECT noID FROM daftartamu ORDER BY noID ASC");
    for(int i = 1; i <= query.size(); i++){
        query.next();
        if(i < 10){
            lastID = "Tm00"+QVariant(i).toString();
            if(lastID != query.value("noID").toString())
                return lastID;
        }else if(i < 100){
            lastID = "Tm0"+QVariant(i).toString();
            if(lastID != query.value("noID").toString())
                return lastID;
        }else if(i < 1000){
            lastID = "Tm"+QVariant(i).toString();
            if(lastID != query.value("noID").toString())
                return lastID;
        }
    }

    return lastID;
}

void MainWindow::on_namaTamuEdit_returnPressed()
{
    this->on_loginTamuButton_clicked();
}

void MainWindow::on_instansiTamuEdit_returnPressed()
{
    this->on_loginTamuButton_clicked();
}

void MainWindow::on_kembaliButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

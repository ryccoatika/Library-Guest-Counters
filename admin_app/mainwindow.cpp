#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QTime>
#include <QDebug>
#include <fstream>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    db =  QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(hostname);
    db.setDatabaseName(dbname);
    db.setUserName(username);
    db.setPassword(password);
    if(!db.open()){
        QMessageBox::critical(this,"kesalahan","Database Error : "+ db.lastError().text());
        return;
    }
    ui->setupUi(this);
    this->loadDataA();
    this->loadDataP();
    this->loadDataT();
    this->on_perBulanButton_clicked();
    ui->daftarAnggotaToolButton->setChecked(true);
    ui->stackedWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete tableStatistik;
    delete model;
    delete msgbox;
    delete chartView;
    delete tambahData;
    delete editData;
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

// **********************************
// ********* toolbar button *********
// **********************************

void MainWindow::on_daftarAnggotaToolButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->daftarAnggota);
}

void MainWindow::on_daftarPengunjungToolButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->daftarPengunjung);
}

void MainWindow::on_dataStatistikToolButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->dataStatistik);
}

void MainWindow::on_daftarTamuToolButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->daftarTamu);
}

// **********************************
// ***** daftar anggota page ******
// **********************************

int MainWindow::getRowCount(QChar aorp) {
    QSqlQuery query;
    QString sql;
    if(aorp == 'a' || aorp == 'A')
        sql = "SELECT COUNT(*) FROM daftaranggota";
    else if(aorp == 'p' || aorp == 'P')
        sql = "SELECT COUNT(*) FROM daftarpengunjung";
    else
        sql = "SELECT COUNT(*) FROM daftartamu";
    query.exec(sql);
    query.next();
    return query.value(0).toInt();
}

void MainWindow::loadDataA() {
    ui->daftarAnggotaTabel->clear();
    ui->daftarAnggotaTabel->setRowCount(getRowCount('A'));
    ui->daftarAnggotaTabel->setColumnCount(4);
    ui->daftarAnggotaTabel->setColumnWidth(0,98);
    ui->daftarAnggotaTabel->setColumnWidth(1,270);
    ui->daftarAnggotaTabel->setColumnWidth(2,417);
    ui->daftarAnggotaTabel->setColumnWidth(3,120);
    QStringList columnHeaders;
    columnHeaders << "ID" << "Nama" << "Alamat" << "Status";
    ui->daftarAnggotaTabel->setHorizontalHeaderLabels(columnHeaders);

    QSqlQuery query;
    int row = 0;
    query.exec("SELECT * FROM daftaranggota");
    while(query.next()) {
        for(int i = 0; i < 4; i++) {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setText(query.value(i).toString());
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui->daftarAnggotaTabel->setItem(row,i,item);
        }
        row++;
    }
}

void MainWindow::on_daftarAnggotaTabel_itemDoubleClicked(QTableWidgetItem *item)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM daftaranggota WHERE noID = ?");
    query.addBindValue((QVariant) ui->daftarAnggotaTabel->item(item->row(),0)->text());
    query.exec();
    query.next();
    QString id = query.value(0).toString();
    QString nama = query.value(1).toString();
    QString alamat = query.value(2).toString();
    QString status = query.value(3).toString();
    QString foto = query.value(4).toString();
    QPixmap pixmap;
    if(foto.trimmed().isEmpty())
        pixmap = QPixmap(":/user.jpg");
    else
        pixmap = QPixmap("imgAnggota/"+foto);

    pixmap = pixmap.scaled(100,100,Qt::KeepAspectRatio);
     msgbox = new QMessageBox(this);
    msgbox->setText(" No ID       : "+id+"\n Nama      : "+nama+"\n Alamat    : "+alamat+"\n Status      : "+status);
    msgbox->setWindowTitle("Anggota Perpustakaan");
    msgbox->setIconPixmap(pixmap);
    msgbox->exec();
}

void MainWindow::on_searchButton_clicked()
{
    if(ui->searchByEdit->text().trimmed().isEmpty()){
        QSqlQuery query;
        int row = 0;
        query.prepare("SELECT * FROM daftarAnggota WHERE jenisAnggota = ? ");
        query.addBindValue(ui->searchBy->currentText());
        query.exec();
        ui->daftarAnggotaTabel->clearContents();
        ui->daftarAnggotaTabel->setRowCount(query.size());
        while(query.next()) {
            for(int i = 0; i < 4; i++) {
                QTableWidgetItem *item = new QTableWidgetItem();
                item->setText(query.value(i).toString());
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->daftarAnggotaTabel->setItem(row,i,item);
            }
            row++;
        }
    }else{
        QString s = "%"+ui->searchByEdit->text()+"%";
        QSqlQuery query;
        int row = 0;
        query.prepare("SELECT * FROM daftarAnggota WHERE jenisAnggota = ? AND nama LIKE ? ");
        query.addBindValue(ui->searchBy->currentText());
        query.addBindValue(s);
        query.exec();
        ui->daftarAnggotaTabel->clearContents();
        ui->daftarAnggotaTabel->setRowCount(query.size());
        while(query.next()) {
            for(int i = 0; i < 4; i++) {
                QTableWidgetItem *item = new QTableWidgetItem();
                item->setText(query.value(i).toString());
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->daftarAnggotaTabel->setItem(row,i,item);
            }
            row++;
        }
    }
}



void MainWindow::on_exportTableAButton_clicked()
{
    if(QMessageBox::information(this,"Export Tabel Anggota","Data yang tampil di Tabel daftar anggota \n akan diexport",QMessageBox::Ok | QMessageBox::Cancel) != QMessageBox::Ok)
        return;
    std::string savefile = QFileDialog::getSaveFileName(this,"Save Tabel Anggota","Daftar Anggota Perpustakaan","CSV File (*.csv)").toStdString();
    std::ofstream csvfile;
    csvfile.open(savefile,std::fstream::out);
    if(csvfile.is_open()){
        for(int row = 0; row < ui->daftarAnggotaTabel->rowCount(); row++){
            for(int col = 0; col < ui->daftarAnggotaTabel->columnCount(); col++){
                csvfile << "\""+ui->daftarAnggotaTabel->item(row,col)->text().toStdString()+"\",";
            }
            csvfile << "\n";
        }
        csvfile.close();
        QMessageBox::information(this,"Export Tabel","File Berhasil disimpan");
    }else{
        QMessageBox::warning(this,"Export Tabel","Export table error");
    }
}

void MainWindow::on_searchByEdit_returnPressed()
{
    this->on_searchButton_clicked();
}


void MainWindow::on_refreshDataAButton_clicked()
{
    this->loadDataA();
}

void MainWindow::on_editDataAButton_clicked()
{
    if(ui->daftarAnggotaTabel->currentRow() != -1){
        QString id = ui->daftarAnggotaTabel->item(ui->daftarAnggotaTabel->currentRow(),0)->text();
        editData = new editdataa(this);
        editData->setId(id);
        editData->exec();
        this->loadDataA();
    }else
        QMessageBox::information(this,"Informasi","Pilih data yang akan diedit");
}

void MainWindow::on_hapusDataAButton_clicked()
{
    if(ui->daftarAnggotaTabel->currentRow() != -1){
        QSqlQuery query;
        query.prepare("SELECT * FROM daftaranggota WHERE noID = ?");
        query.addBindValue((QVariant) ui->daftarAnggotaTabel->item(ui->daftarAnggotaTabel->currentRow(),0)->text());
        query.exec();
        query.next();
        QString id = query.value(0).toString();
        QString nama = query.value(1).toString();
        QString alamat = query.value(2).toString();
        QString status = query.value(3).toString();
        QString foto = query.value(4).toString();
        QPixmap pixmap;
        if(foto.trimmed().isEmpty())
            pixmap = QPixmap(":/user.jpg");
        else
            pixmap = QPixmap("imgAnggota/"+foto);
        pixmap = pixmap.scaled(100,100,Qt::KeepAspectRatio);
        msgbox = new QMessageBox(this);
        msgbox->setText(" No ID       : "+id+"\n Nama      : "+nama+"\n Alamat    : "+alamat+"\n Status      : "+status+"\n\n Hapus data ini?");
        msgbox->setWindowTitle("Hapus Anggota");
        msgbox->setIconPixmap(pixmap);
        msgbox->setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        if(msgbox->exec() == QMessageBox::Yes){
            QSqlQuery query;
            query.exec("SELECT foto FROM daftaranggota WHERE noID = '"+id+"'");
            query.next();
            QString fotoname = query.value(0).toString();
            query.clear();
            QString sql = "DELETE FROM daftaranggota WHERE noID = '"+id+"'";
            if(query.exec(sql)){
                if(!fotoname.trimmed().isEmpty()){
                    if(QFile::remove("imgAnggota/"+fotoname)){
                        QMessageBox::information(this,"Informasi","Hapus data berhasil");
                        this->loadDataA();
                    }
                }else{
                    QMessageBox::information(this,"Informasi","Hapus data berhasil");
                    this->loadDataA();
                }
            }
        }
    }else
        QMessageBox::information(this,"Informasi","Pilih data yang akan dihapus");
}

void MainWindow::on_tambahDataAButton_clicked()
{
    tambahData = new tambahDataA(this);
    tambahData->exec();
    this->loadDataA();
}

// **********************************
// ***** daftar Tamu page *****
// **********************************

void MainWindow::loadDataT()
{
    ui->daftarTamuTabel->clear();
    ui->daftarTamuTabel->setColumnCount(3);
    ui->daftarTamuTabel->setRowCount(this->getRowCount('t'));
    ui->daftarTamuTabel->setColumnWidth(0,150);
    ui->daftarTamuTabel->setColumnWidth(1,300);
    ui->daftarTamuTabel->setColumnWidth(2,500);
    QStringList columnHeaders;
    columnHeaders << "ID" << "Nama" << "Instansi";
    ui->daftarTamuTabel->setHorizontalHeaderLabels(columnHeaders);

    QSqlQuery query;
    int row = 0;
    query.exec("SELECT * FROM daftartamu");
    while(query.next()) {
        for(int i = 0; i < 3; i++) {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setText(query.value(i).toString());
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui->daftarTamuTabel->setItem(row,i,item);
        }
        row++;
    }
}

void MainWindow::on_hapusDataTButton_clicked()
{
    if(ui->daftarTamuTabel->currentRow() != -1){
        QSqlQuery query;
        query.prepare("SELECT * FROM daftartamu WHERE nama LIKE ? AND instansi LIKE ?");
        query.addBindValue((QVariant) ui->daftarTamuTabel->item(ui->daftarTamuTabel->currentRow(),1)->text());
        query.addBindValue((QVariant) ui->daftarTamuTabel->item(ui->daftarTamuTabel->currentRow(),2)->text());
        query.exec();
        query.next();
        QString id = query.value(0).toString();
        QString nama = query.value(1).toString();
        QString instansi = query.value(2).toString();
        QPixmap pixmap;
        pixmap = QPixmap(":/user.jpg");
        pixmap = pixmap.scaled(100,100,Qt::KeepAspectRatio);
        msgbox = new QMessageBox(this);
        msgbox->setText(" No ID       : "+id+"\n Nama      : "+nama+"\n Instansi    : "+instansi+"\n\n Hapus data ini?");
        msgbox->setWindowTitle("Hapus Tamu");
        msgbox->setIconPixmap(pixmap);
        msgbox->setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        if(msgbox->exec() == QMessageBox::Yes){
            QSqlQuery query;
            QString sql = "DELETE FROM daftartamu WHERE noID = '"+id+"'";
            if(query.exec(sql)){
                QMessageBox::information(this,"Informasi","Hapus data berhasil");
                this->loadDataT();
            }
        }
    }else
        QMessageBox::information(this,"Informasi","Pilih data yang akan dihapus");
}

void MainWindow::on_daftarTamuTabel_itemDoubleClicked(QTableWidgetItem *item)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM daftartamu WHERE nama LIKE ? AND instansi LIKE ?");
    query.addBindValue((QVariant) ui->daftarTamuTabel->item(item->row(),1)->text());
    query.addBindValue((QVariant) ui->daftarTamuTabel->item(item->row(),2)->text());
    query.exec();
    query.next();
    QString id = query.value(0).toString();
    QString nama = query.value(1).toString();
    QString instansi = query.value(2).toString();
    QPixmap pixmap;
    pixmap = QPixmap(":/user.jpg");
    pixmap = pixmap.scaled(100,100,Qt::KeepAspectRatio);
     msgbox = new QMessageBox(this);
    msgbox->setText(" No ID       : "+id+"\n Nama      : "+nama+"\n Instansi    : "+instansi);
    msgbox->setWindowTitle("Tamu Perpustakaan");
    msgbox->setIconPixmap(pixmap);
    msgbox->exec();
}

void MainWindow::on_refreshDataTButton_clicked()
{
    this->loadDataT();
}


void MainWindow::on_exportTableTButton_clicked()
{
    if(QMessageBox::information(this,"Export Tabel Tamu","Data yang tampil di Tabel daftar anggota \n akan diexport",QMessageBox::Ok | QMessageBox::Cancel) != QMessageBox::Ok)
        return;
    std::string savefile = QFileDialog::getSaveFileName(this,"Save Tabel Tamu","Daftar Tamu Perpustakaan","CSV File (*.csv)").toStdString();
    std::ofstream csvfile;
    csvfile.open(savefile,std::fstream::out);
    if(csvfile.is_open()){
        for(int row = 0; row < ui->daftarTamuTabel->rowCount(); row++){
            for(int col = 0; col < ui->daftarTamuTabel->columnCount(); col++){
                csvfile << "\""+ui->daftarTamuTabel->item(row,col)->text().toStdString()+"\",";
            }
            csvfile << "\n";
        }
        csvfile.close();
        QMessageBox::information(this,"Export Tabel","File Berhasil disimpan");
    }else{
        QMessageBox::warning(this,"Export Tabel","Export table error");
    }
}


void MainWindow::on_searchTTButton_clicked()
{
    if(ui->searchEdit->text().trimmed().isEmpty()){
        ui->searchEdit->setFocus();
        return;
    }
    ui->daftarTamuTabel->clearContents();

    QSqlQuery query;
    int row = 0;
    QString src = "%"+ui->searchEdit->text()+"%";
    query.prepare("SELECT * FROM daftartamu WHERE nama LIKE ? OR instansi LIKE ?");
    query.addBindValue(src);
    query.addBindValue(src);
    query.exec();
    ui->daftarTamuTabel->setRowCount(query.size());
    while(query.next()) {
        for(int i = 0; i < 3; i++) {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setText(query.value(i).toString());
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui->daftarTamuTabel->setItem(row,i,item);
        }
        row++;
    }
}

void MainWindow::on_searchEdit_returnPressed()
{
    this->on_searchTTButton_clicked();
}

// **********************************
// ***** daftar pengunjung page *****
// **********************************


void MainWindow::loadDataP() {
    ui->daftarPengunjungTabel->clear();
    ui->daftarPengunjungTabel->setRowCount(getRowCount('p'));
    ui->daftarPengunjungTabel->setColumnCount(6);
    ui->daftarPengunjungTabel->setColumnWidth(0,80); //no ID
    ui->daftarPengunjungTabel->setColumnWidth(1,180); //nama
    ui->daftarPengunjungTabel->setColumnWidth(2,303); //alamat
    ui->daftarPengunjungTabel->setColumnWidth(3,91); //status
    ui->daftarPengunjungTabel->setColumnWidth(4,110); //jam kunjung
    ui->daftarPengunjungTabel->setColumnWidth(5,110); //tgl kunjung
    QStringList columnHeaders;
    columnHeaders << "ID" << "Nama" << "Alamat / Instansi" << "Status" << "Jam Kunjung" << "Tanggal Kunjung";
    ui->daftarPengunjungTabel->setHorizontalHeaderLabels(columnHeaders);

    QSqlQuery query;
    int row = 0;
    query.exec("SELECT daftaranggota.noID, daftaranggota.nama, daftaranggota.alamat, daftaranggota.jenisAnggota, daftarpengunjung.jamKunjung, daftarpengunjung.tglKunjung  FROM daftarpengunjung INNER JOIN daftaranggota ON daftarpengunjung.daftarAnggota_noID = daftaranggota.noID");
    while(query.next()) {
        for(int i = 0; i < 6; i++) {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setText(query.value(i).toString());
            if(i == 4 ) {
                item->setText(QTime::fromString(item->text()).toString("hh:mm"));
            }
            if(i == 5) {
                item->setText(QDate::fromString(item->text(),"yyyy-MM-dd").toString("dd-MMM-yyyy"));
            }
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui->daftarPengunjungTabel->setItem(row,i,item);
        }
        row++;
    }
    query.exec("SELECT daftartamu.noID, daftartamu.nama, daftartamu.instansi, daftartamu.jenisAnggota, daftarpengunjung.jamKunjung, daftarpengunjung.tglKunjung  FROM daftarpengunjung INNER JOIN daftartamu ON daftarpengunjung.daftarAnggota_noID = daftartamu.noID");
    while(query.next()) {
        for(int i = 0; i < 6; i++) {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setText(query.value(i).toString());
            if(i == 4 ) {
                item->setText(QTime::fromString(item->text()).toString("hh:mm"));
            }
            if(i == 5) {
                item->setText(QDate::fromString(item->text(),"yyyy-MM-dd").toString("dd-MMM-yyyy"));
            }
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui->daftarPengunjungTabel->setItem(row,i,item);
        }
        row++;
    }
    ui->daftarPengunjungTabel->sortByColumn(5);
}

void MainWindow::on_refreshDataPButton_clicked()
{
    this->loadDataP();
}

void MainWindow::on_hapusDataPButton_clicked()
{
    if(ui->daftarPengunjungTabel->currentRow() != -1){
        QSqlQuery query;
        if(ui->daftarPengunjungTabel->item(ui->daftarPengunjungTabel->currentRow(),0)->text().mid(0,2) == "Tm")
            query.prepare("SELECT daftartamu.noID, daftartamu.nama, daftartamu.instansi, daftartamu.jenisAnggota, daftartamu.foto FROM daftarpengunjung INNER JOIN daftartamu ON daftarpengunjung.daftarAnggota_noID = daftartamu.noID WHERE daftarpengunjung.daftarAnggota_noID = ?");
        else
            query.prepare("SELECT daftaranggota.noID, daftaranggota.nama, daftaranggota.alamat, daftaranggota.jenisAnggota, daftaranggota.foto  FROM daftarpengunjung INNER JOIN daftaranggota ON daftarpengunjung.daftarAnggota_noID = daftaranggota.noID WHERE daftarpengunjung.daftarAnggota_noID = ?");
        query.addBindValue((QVariant) ui->daftarPengunjungTabel->item(ui->daftarPengunjungTabel->currentRow(),0)->text());
        query.exec();
        query.next();
        QString id = query.value(0).toString();
        QString nama = query.value(1).toString();
        QString alamat = query.value(2).toString();
        QString status = query.value(3).toString();
        QString foto = query.value(4).toString();
        QPixmap pixmap;
        if(foto.trimmed().isEmpty())
            pixmap = QPixmap(":/user.jpg");
        else
            pixmap = QPixmap("imgAnggota/"+foto);
        pixmap = pixmap.scaled(100,100,Qt::KeepAspectRatio);
        msgbox = new QMessageBox(this);
        msgbox->setText(" No ID       : "+id+"\n Nama      : "+nama+"\n Alamat    : "+alamat+"\n Status      : "+status+"\n\n Hapus data ini?");
        msgbox->setWindowTitle("Hapus Pengunjung");
        msgbox->setIconPixmap(pixmap);
        msgbox->setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        if(msgbox->exec() == QMessageBox::Yes){
            QSqlQuery query;
            QString sql = "DELETE FROM daftarpengunjung WHERE daftarAnggota_noID = '"+id+"'";
            if(query.exec(sql)){
                QMessageBox::information(this,"Informasi","Hapus data berhasil");
                this->loadDataP();
            }
        }
    }else
        QMessageBox::information(this,"Informasi","Pilih data yang akan dihapus");
}

void MainWindow::on_daftarPengunjungTabel_itemDoubleClicked(QTableWidgetItem *item)
{
    QSqlQuery query;
    if(ui->daftarPengunjungTabel->item(item->row(),0)->text().mid(0,2) == "Tm")
        query.prepare("SELECT daftartamu.noID, daftartamu.nama, daftartamu.instansi, daftartamu.jenisAnggota, daftartamu.foto FROM daftarpengunjung INNER JOIN daftartamu ON daftarpengunjung.daftarAnggota_noID = daftartamu.noID WHERE daftarpengunjung.daftarAnggota_noID = ?");
    else
        query.prepare("SELECT daftaranggota.noID, daftaranggota.nama, daftaranggota.alamat, daftaranggota.jenisAnggota, daftaranggota.foto  FROM daftarpengunjung INNER JOIN daftaranggota ON daftarpengunjung.daftarAnggota_noID = daftaranggota.noID WHERE daftarpengunjung.daftarAnggota_noID = ?");
    query.addBindValue((QVariant) ui->daftarPengunjungTabel->item(item->row(),0)->text());
    query.exec();
    query.next();
    QString id = query.value(0).toString();
    QString nama = query.value(1).toString();
    QString alamat = query.value(2).toString();
    QString status = query.value(3).toString();
    QString foto = query.value(4).toString();
    QPixmap pixmap;
    if(foto.trimmed().isEmpty())
        pixmap = QPixmap(":/user.jpg");
    else
        pixmap = QPixmap("imgAnggota/"+foto);

    pixmap = pixmap.scaled(100,100,Qt::KeepAspectRatio);
    msgbox = new QMessageBox(this);
    msgbox->setText(" No ID          : "+id+"\n Nama         : "+nama+"\n Alamat/Instansi  : "+alamat+"\n Status         : "+status);
    msgbox->setWindowTitle("Pengunjung Perpustakaan");
    msgbox->setIconPixmap(pixmap);
    msgbox->exec();
}

void MainWindow::on_statistikPButton_clicked()
{
    this->on_dataStatistikToolButton_clicked();
}

void MainWindow::on_searchPBy_currentTextChanged(const QString &arg1)
{
    if(arg1 == "Nama"){
        ui->stackedWidget_2->setCurrentWidget(ui->nama);
        ui->nama->setFocus();
    }
    if(arg1 == "Tanggal Kunjung"){
        ui->stackedWidget_2->setCurrentWidget(ui->tglKunjung);
        ui->searchDate->setDate(QDate::currentDate());
    }
}

void MainWindow::on_searchPButton_clicked()
{
    if(ui->stackedWidget_2->currentWidget() == ui->nama){
        if(ui->searchPByEdit->text().trimmed().isEmpty()){
            ui->searchPByEdit->setFocus();
            return;
        }
        ui->daftarPengunjungTabel->clearContents();

        QSqlQuery query;
        int row = 0;
        int rows;
        QString src = "%"+ui->searchPByEdit->text()+"%";
        query.prepare("SELECT daftaranggota.noID, daftaranggota.nama, daftaranggota.alamat, daftaranggota.jenisAnggota, daftarpengunjung.jamKunjung, daftarpengunjung.tglKunjung  FROM daftarpengunjung INNER JOIN daftaranggota ON daftarpengunjung.daftarAnggota_noID = daftaranggota.noID WHERE daftaranggota.nama LIKE ?");
        query.addBindValue(src);
        query.exec();
        rows = query.size();
        ui->daftarPengunjungTabel->setRowCount(rows);
        while(query.next()) {
            for(int i = 0; i < 6; i++) {
                QTableWidgetItem *item = new QTableWidgetItem();
                item->setText(query.value(i).toString());
                if(i == 4 ) {
                    item->setText(QTime::fromString(item->text()).toString("hh:mm"));
                }
                if(i == 5) {
                    item->setText(QDate::fromString(item->text(),"yyyy-MM-dd").toString("dd-MMM-yyyy"));
                }
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->daftarPengunjungTabel->setItem(row,i,item);
            }
            row++;
        }
        query.prepare("SELECT daftartamu.noID, daftartamu.nama, daftartamu.instansi, daftartamu.jenisAnggota, daftarpengunjung.jamKunjung, daftarpengunjung.tglKunjung  FROM daftarpengunjung INNER JOIN daftartamu ON daftarpengunjung.daftarAnggota_noID = daftartamu.noID WHERE daftartamu.nama LIKE ?");
        query.addBindValue(src);
        query.exec();
        rows += query.size();
        ui->daftarPengunjungTabel->setRowCount(rows);
        while(query.next()) {
            for(int i = 0; i < 6; i++) {
                QTableWidgetItem *item = new QTableWidgetItem();
                item->setText(query.value(i).toString());
                if(i == 4 ) {
                    item->setText(QTime::fromString(item->text()).toString("hh:mm"));
                }
                if(i == 5) {
                    item->setText(QDate::fromString(item->text(),"yyyy-MM-dd").toString("dd-MMM-yyyy"));
                }
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->daftarPengunjungTabel->setItem(row,i,item);
            }
            row++;
        }
        ui->daftarPengunjungTabel->sortByColumn(1);
    }else if(ui->stackedWidget_2->currentWidget() == ui->tglKunjung){
        ui->daftarPengunjungTabel->clearContents();

        QSqlQuery query;
        int row = 0;
        int rows;
        QString src = ui->searchDate->text();
        query.prepare("SELECT daftaranggota.noID, daftaranggota.nama, daftaranggota.alamat, daftaranggota.jenisAnggota, daftarpengunjung.jamKunjung, daftarpengunjung.tglKunjung  FROM daftarpengunjung INNER JOIN daftaranggota ON daftarpengunjung.daftarAnggota_noID = daftaranggota.noID WHERE daftarpengunjung.tglKunjung LIKE ?");
        query.addBindValue(src);
        query.exec();
        rows = query.size();
        ui->daftarPengunjungTabel->setRowCount(rows);
        while(query.next()) {
            for(int i = 0; i < 6; i++) {
                QTableWidgetItem *item = new QTableWidgetItem();
                item->setText(query.value(i).toString());
                if(i == 4 ) {
                    item->setText(QTime::fromString(item->text()).toString("hh:mm"));
                }
                if(i == 5) {
                    item->setText(QDate::fromString(item->text(),"yyyy-MM-dd").toString("dd-MMM-yyyy"));
                }
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->daftarPengunjungTabel->setItem(row,i,item);
            }
            row++;
        }
        query.prepare("SELECT daftartamu.noID, daftartamu.nama, daftartamu.instansi, daftartamu.jenisAnggota, daftarpengunjung.jamKunjung, daftarpengunjung.tglKunjung  FROM daftarpengunjung INNER JOIN daftartamu ON daftarpengunjung.daftarAnggota_noID = daftartamu.noID WHERE daftarpengunjung.tglKunjung LIKE ?");
        query.addBindValue(src);
        query.exec();
        rows += query.size();
        ui->daftarPengunjungTabel->setRowCount(rows);
        while(query.next()) {
            for(int i = 0; i < 6; i++) {
                QTableWidgetItem *item = new QTableWidgetItem();
                item->setText(query.value(i).toString());
                if(i == 4 ) {
                    item->setText(QTime::fromString(item->text()).toString("hh:mm"));
                }
                if(i == 5) {
                    item->setText(QDate::fromString(item->text(),"yyyy-MM-dd").toString("dd-MMM-yyyy"));
                }
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->daftarPengunjungTabel->setItem(row,i,item);
            }
            row++;
        }
        ui->daftarPengunjungTabel->sortByColumn(4);
    }
}


void MainWindow::on_searchPByEdit_returnPressed()
{
    this->on_searchPButton_clicked();
}


void MainWindow::on_exportTablePButton_clicked()
{
    if(QMessageBox::information(this,"Export Tabel Pengunjung","Data yang tampil di Tabel daftar anggota \n akan diexport",QMessageBox::Ok | QMessageBox::Cancel) != QMessageBox::Ok)
        return;
    std::string savefile = QFileDialog::getSaveFileName(this,"Save Tabel Pengunjung","Daftar Pengunjung Perpustakaan","CSV File (*.csv)").toStdString();
    std::ofstream csvfile;
    csvfile.open(savefile,std::fstream::out);
    if(csvfile.is_open()){
        for(int row = 0; row < ui->daftarPengunjungTabel->rowCount(); row++){
            for(int col = 0; col < ui->daftarPengunjungTabel->columnCount(); col++){
                csvfile << "\""+ui->daftarPengunjungTabel->item(row,col)->text().toStdString()+"\",";
            }
            csvfile << "\n";
        }
        csvfile.close();
        QMessageBox::information(this,"Export Tabel","File Berhasil disimpan");
    }else{
        QMessageBox::warning(this,"Export Tabel","Export table error");
    }
}

// **********************************
// ****** data statistik page *******
// **********************************
QStringList MainWindow::loadStats(char DorCorMorTorA,QString mode, int first, int last, int year)
{
    if(mode == "month"){
        QStringList categories;
        for(int i = first;i <= last; i++){
            categories.append(QVariant(year).toString()+"-"+noBulan.at(i)+"-%");
        }
        QSqlQuery query;
        QStringList jml;
        for(int i = 0; i < categories.count(); i++){
            switch (DorCorMorTorA) {
            case 'd':
            case 'D':
                query.prepare("SELECT COUNT(*) FROM daftarpengunjung INNER JOIN daftaranggota ON daftaranggota.noID = daftarpengunjung.daftarAnggota_noID WHERE daftaranggota.jenisAnggota = 'Dosen/Karyawan' AND daftarpengunjung.tglKunjung LIKE ?");
                break;
            case 'c':
            case 'C':
                query.prepare("SELECT COUNT(*) FROM daftarpengunjung INNER JOIN daftaranggota ON daftaranggota.noID = daftarpengunjung.daftarAnggota_noID WHERE daftaranggota.jenisAnggota = 'Co Ass' AND daftarpengunjung.tglKunjung LIKE ?");
                break;
            case 'm':
            case 'M':
                query.prepare("SELECT COUNT(*) FROM daftarpengunjung INNER JOIN daftaranggota ON daftaranggota.noID = daftarpengunjung.daftarAnggota_noID WHERE daftaranggota.jenisAnggota = 'Mahasiswa' AND daftarpengunjung.tglKunjung LIKE ?");
                break;
            case 't':
            case 'T':
                query.prepare("SELECT COUNT(*) FROM daftarpengunjung WHERE daftarAnggota_noID LIKE 'Tm%' AND tglKunjung LIKE ?");
                break;
            case 'a':
            case 'A':
                query.prepare("SELECT COUNT(*) FROM daftarpengunjung WHERE tglKunjung LIKE ?");
                break;
            default:
                break;
            }
            query.addBindValue(categories.at(i));
            query.exec();
            while(query.next())
                jml.append(query.value(0).toString());
        }
        return jml;
    }else{
        QStringList categories;
        for(int i = first;i <= last; i++){
            categories.append(QVariant(i).toString()+"-%");
        }
        QSqlQuery query;
        QStringList jml;
        for(int i = 0; i < categories.count(); i++){
            switch (DorCorMorTorA) {
            case 'd':
            case 'D':
                query.prepare("SELECT COUNT(*) FROM daftarpengunjung INNER JOIN daftaranggota ON daftaranggota.noID = daftarpengunjung.daftarAnggota_noID WHERE daftaranggota.jenisAnggota = 'Dosen/Karyawan' AND daftarpengunjung.tglKunjung LIKE ?");
                break;
            case 'c':
            case 'C':
                query.prepare("SELECT COUNT(*) FROM daftarpengunjung INNER JOIN daftaranggota ON daftaranggota.noID = daftarpengunjung.daftarAnggota_noID WHERE daftaranggota.jenisAnggota = 'Co Ass' AND daftarpengunjung.tglKunjung LIKE ?");
                break;
            case 'm':
            case 'M':
                query.prepare("SELECT COUNT(*) FROM daftarpengunjung INNER JOIN daftaranggota ON daftaranggota.noID = daftarpengunjung.daftarAnggota_noID WHERE daftaranggota.jenisAnggota = 'Mahasiswa' AND daftarpengunjung.tglKunjung LIKE ?");
                break;
            case 't':
            case 'T':
                query.prepare("SELECT COUNT(*) FROM daftarpengunjung WHERE daftarAnggota_noID LIKE 'Tm%' AND tglKunjung LIKE ?");
                break;
            case 'a':
            case 'A':
                query.prepare("SELECT COUNT(*) FROM daftarpengunjung WHERE tglKunjung LIKE ?");
                break;
            default:
                return jml;
                break;
            }
            query.addBindValue(categories.at(i));
            query.exec();
            while(query.next())
                jml.append(query.value(0).toString());
        }
        return jml;
    }
}

void MainWindow::loadChart(QString mode, int first, int last, int year)
{
    if(mode == "month"){
        QBarSet *set0 = new QBarSet("Dosen/Karyawan");
        QBarSet *set1 = new QBarSet("Co Ass");
        QBarSet *set2 = new QBarSet("Mahasiswa");
        QBarSet *set3 = new QBarSet("Umum");

        QStringList jmlDosen = this->loadStats('D',mode,first,last,year);
        QStringList jmlCoAss = this->loadStats('C',mode,first,last,year);
        QStringList jmlMahasiswa = this->loadStats('M',mode,first,last,year);
        QStringList jmlTamu = this->loadStats('T',mode,first,last,year);
        QStringList categories;
        for(int i = first,j=0;i <= last; i++,j++){
            categories.append(bulan.at(i));
            set0->append(QVariant(jmlDosen.at(j)).toInt());
            set1->append(QVariant(jmlCoAss.at(j)).toInt());
            set2->append(QVariant(jmlMahasiswa.at(j)).toInt());
            set3->append(QVariant(jmlTamu.at(j)).toInt());
        }

        QBarSeries *series = new QBarSeries();
        series->append(set0);
        series->append(set1);
        series->append(set2);
        series->append(set3);

        QChart *chart = new QChart();
        chart->addSeries(series);
        if(first == last)
            chart->setTitle("Statistik Pengunjung Perpustakaan Kedokteran Gigi UNSOED Bulan "+categories.first());
        else
            chart->setTitle("Statistik Pengunjung Perpustakaan Kedokteran Gigi UNSOED Bulan "+categories.first()+" - "+categories.last());
        chart->setAnimationOptions(QChart::SeriesAnimations);

        QBarCategoryAxis *axis = new QBarCategoryAxis();
        axis->append(categories);
        chart->createDefaultAxes();
        chart->setAxisX(axis, series);

        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignBottom);

        chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);

        ui->verticalLayout_3->layout()->addWidget(chartView);
        ui->dataStatistik->repaint();
    }else{
        // year chart
        QBarSet *set0 = new QBarSet("Dosen/Karyawan");
        QBarSet *set1 = new QBarSet("Co Ass");
        QBarSet *set2 = new QBarSet("Mahasiswa");
        QBarSet *set3 = new QBarSet("Umum");

        QStringList jmlDosen = this->loadStats('D',mode,first,last,year);
        QStringList jmlCoAss = this->loadStats('C',mode,first,last,year);
        QStringList jmlMahasiswa = this->loadStats('M',mode,first,last,year);
        QStringList jmlTamu = this->loadStats('T',mode,first,last,year);
        QStringList categories;
        for(int i = first,j=0;i <= last; i++,j++){
            categories.append(QVariant(i).toString());
            set0->append(QVariant(jmlDosen.at(j)).toInt());
            set1->append(QVariant(jmlCoAss.at(j)).toInt());
            set2->append(QVariant(jmlMahasiswa.at(j)).toInt());
            set3->append(QVariant(jmlTamu.at(j)).toInt());
        }

        QBarSeries *series = new QBarSeries();
        series->append(set0);
        series->append(set1);
        series->append(set2);
        series->append(set3);

        QChart * chart = new QChart();
        chart->addSeries(series);
        if(first == last)
            chart->setTitle("Statistik Pengunjung Perpustakaan Kedokteran Gigi UNSOED Tahun "+categories.first());
        else
            chart->setTitle("Statistik Pengunjung Perpustakaan Kedokteran Gigi UNSOED Tahun "+categories.first()+" - "+categories.last());
        chart->setAnimationOptions(QChart::SeriesAnimations);

        QBarCategoryAxis *axisx = new QBarCategoryAxis();
        axisx->append(categories);
        chart->createDefaultAxes();
        chart->setAxisX(axisx, series);

        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignBottom);

        chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);

        ui->verticalLayout_3->layout()->addWidget(chartView);
        ui->dataStatistik->repaint();
    }
}

void MainWindow::loadTableStats(QString mode, int first, int last, int year)
{
    this->tableStatistik = new QTableWidget();
    this->tableStatistik->setFixedSize(1024,512);
    this->tableStatistik->setStyleSheet("font: 18pt 'Comic Sans MS';");
    if(mode == "month"){
        QStringList jmlDosen = this->loadStats('D',mode,first,last,year);
        QStringList jmlCoAss = this->loadStats('C',mode,first,last,year);
        QStringList jmlMahasiswa = this->loadStats('M',mode,first,last,year);
        QStringList jmlTamu = this->loadStats('T',mode,first,last,year);
        this->tableStatistik->setRowCount(5);
        int col = 2+last-first;
        this->tableStatistik->setColumnCount(col);
        QStringList headerH;
        for(int i = first;i <= last; i++){
            headerH.append(bulan.at(i));
        }
        headerH.append("Jumlah");
        this->tableStatistik->setHorizontalHeaderLabels(headerH);
        QStringList headerV;
        headerV << "Dosen / Karyawan" << "Co Ass" << "Mahasiswa" << "Tamu" << "Jumlah";
        this->tableStatistik->setVerticalHeaderLabels(headerV);
        for(int i = 0;i < col-1; i++){
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setText(jmlDosen.at(i));
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            this->tableStatistik->setItem(0,i,item);
        }
        for(int i = 0;i < col-1; i++){
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setText(jmlCoAss.at(i));
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            this->tableStatistik->setItem(1,i,item);
        }
        for(int i = 0;i < col-1; i++){
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setText(jmlMahasiswa.at(i));
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            this->tableStatistik->setItem(2,i,item);
        }
        for(int i = 0;i < col-1; i++){
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setText(jmlTamu.at(i));
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            this->tableStatistik->setItem(3,i,item);
        }
        int jml;
        for(int i = 0;i < col-1; i++){
            QTableWidgetItem *item = new QTableWidgetItem();
            jml = QVariant(jmlDosen.at(i)).toInt() + QVariant(jmlCoAss.at(i)).toInt() + QVariant(jmlMahasiswa.at(i)).toInt() + QVariant(jmlTamu.at(i)).toInt();
            item->setText(QVariant(jml).toString());
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            this->tableStatistik->setItem(4,i,item);
        }
        for(int i = 0; i < 5; i++){
            int sumI = 0;
            for(int j = 0; j < col-1; j++){
                sumI += this->tableStatistik->item(i,j)->text().toInt();
            }
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setText(QVariant(sumI).toString());
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            this->tableStatistik->setItem(i,col-1,item);
        }
    }else{
        QStringList jmlDosen = this->loadStats('D',mode,first,last,year);
        QStringList jmlCoAss = this->loadStats('C',mode,first,last,year);
        QStringList jmlMahasiswa = this->loadStats('M',mode,first,last,year);
        QStringList jmlTamu = this->loadStats('T',mode,first,last,year);
        this->tableStatistik->setRowCount(5);
        int col = 2+last-first;
        this->tableStatistik->setColumnCount(col);
        QStringList headerH;
        for(int i = first;i <= last; i++){
            headerH.append(QVariant(i).toString());
        }
        headerH.append("Jumlah");
        this->tableStatistik->setHorizontalHeaderLabels(headerH);
        QStringList headerV;
        headerV << "Dosen / Karyawan" << "Co Ass" << "Mahasiswa" << "Tamu" << "Jumlah";
        this->tableStatistik->setVerticalHeaderLabels(headerV);
        for(int i = 0;i < col-1; i++){
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setText(jmlDosen.at(i));
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            this->tableStatistik->setItem(0,i,item);
        }
        for(int i = 0;i < col-1; i++){
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setText(jmlCoAss.at(i));
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            this->tableStatistik->setItem(1,i,item);
        }
        for(int i = 0;i < col-1; i++){
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setText(jmlMahasiswa.at(i));
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            this->tableStatistik->setItem(2,i,item);
        }
        for(int i = 0;i < col-1; i++){
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setText(jmlTamu.at(i));
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            this->tableStatistik->setItem(3,i,item);
        }
        int jml;
        for(int i = 0;i < col-1; i++){
            QTableWidgetItem *item = new QTableWidgetItem();
            jml = QVariant(jmlDosen.at(i)).toInt() + QVariant(jmlCoAss.at(i)).toInt() + QVariant(jmlMahasiswa.at(i)).toInt() + QVariant(jmlTamu.at(i)).toInt();
            item->setText(QVariant(jml).toString());
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            this->tableStatistik->setItem(4,i,item);
        }
        for(int i = 0; i < 5; i++){
            int sumI = 0;
            for(int j = 0; j < col-1; j++){
                sumI += this->tableStatistik->item(i,j)->text().toInt();
            }
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setText(QVariant(sumI).toString());
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            this->tableStatistik->setItem(i,col-1,item);
        }
    }
    this->tableStatistik->verticalHeader()->setStyleSheet("background-color: #ffffff;");
    this->tableStatistik->resizeColumnsToContents();
    this->tableStatistik->resizeRowsToContents();
}

void MainWindow::on_previewButton_clicked()
{
    if(!ui->verticalLayout_3->isEmpty())
        ui->verticalLayout_3->takeAt(0);
    if(statistikBy == 'm'){
        int first;
        int last;
        for(int i = 0; i < 12; i++){
            if(bulan.at(i) == ui->dariBulan->currentText())
                first = i;
            if(bulan.at(i) == ui->sampaiBulan->currentText())
                last = i;
        }
        if(first > last){
            int x = first;
            first = last;
            last = x;
        }
        this->loadChart("month",first,last,ui->bulanTahun->currentText().toInt());
    }else if(statistikBy == 'y'){
        int first = ui->dariTahun->currentText().toInt();
        int last = ui->sampaiTahun->currentText().toInt();
        if(first > last){
            int x = first;
            first = last;
            last = x;
        }
        this->loadChart("year",first,last);
    }
}

void MainWindow::on_perBulanButton_clicked()
{
    if(statistikBy != 'm'){
        QStringList listTahun;
        QSqlQuery query;
        query.exec("SELECT tglKunjung FROM daftarpengunjung ORDER BY tglKunjung ASC");
        query.next();
        listTahun.append(QDate::fromString(query.value(0).toString(),"yyyy-MM-dd").toString("yyyy"));
        while(query.next()){
            if(listTahun.last() != QDate::fromString(query.value(0).toString(),"yyyy-MM-dd").toString("yyyy"))
                listTahun.append(QDate::fromString(query.value(0).toString(),"yyyy-MM-dd").toString("yyyy"));
        }
        ui->bulanTahun->clear();
        for(int i = 0; i < listTahun.length(); i++)
            ui->bulanTahun->addItem(listTahun.at(i));
        ui->dariSampai->setCurrentWidget(ui->bulanPage);
        statistikBy = 'm';
    }
}

void MainWindow::on_perTahunButton_clicked()
{
    if(statistikBy != 'y'){
        QStringList listTahun;
        QSqlQuery query;
        query.exec("SELECT tglKunjung FROM daftarpengunjung ORDER BY tglKunjung ASC");
        query.next();
        listTahun.append(QDate::fromString(query.value(0).toString(),"yyyy-MM-dd").toString("yyyy"));
        while(query.next()){
            if(listTahun.last() != QDate::fromString(query.value(0).toString(),"yyyy-MM-dd").toString("yyyy"))
                listTahun.append(QDate::fromString(query.value(0).toString(),"yyyy-MM-dd").toString("yyyy"));
        }
        ui->dariTahun->clear();
        ui->sampaiTahun->clear();
        for(int i = 0; i < listTahun.length(); i++){
            ui->dariTahun->addItem(listTahun.at(i));
            ui->sampaiTahun->addItem(listTahun.at(i));
        }
        listTahun.clear();
        ui->dariSampai->setCurrentWidget(ui->tahunPage);
        statistikBy = 'y';
    }
}

void MainWindow::on_bulanTahun_currentTextChanged(const QString &arg1)
{
    QString search = arg1+"-%";
    QStringList bulanKunjung;
    QSqlQuery query;
    query.prepare("SELECT tglKunjung FROM daftarpengunjung WHERE tglKunjung LIKE ? ORDER BY tglKunjung ASC");
    query.addBindValue(search);
    query.exec();
    query.next();
    bulanKunjung.append(QDate::fromString(query.value(0).toString(),"yyyy-MM-dd").toString("MM"));
    while(query.next()){
        if(bulanKunjung.last() != QDate::fromString(query.value(0).toString(),"yyyy-MM-dd").toString("MM"))
            bulanKunjung.append(QDate::fromString(query.value(0).toString(),"yyyy-MM-dd").toString("MM"));
    }
    ui->sampaiBulan->clear();
    ui->dariBulan->clear();
    for(int i=0;i < bulanKunjung.count(); i++){
        for(int j=0; j < 12; j++){
            if(bulanKunjung.at(i) == noBulan.at(j)){
                ui->dariBulan->addItem(bulan.at(j));
                ui->sampaiBulan->addItem(bulan.at(j));
            }
        }
    }
    bulanKunjung.clear();
}


void MainWindow::on_printButton_clicked()
{
    if(ui->verticalLayout_3->isEmpty()) return;
    QPrinter printer;
//    printer.setPaperSize(QSizeF(215,297),QPrinter::Millimeter);
    QPrintDialog dialog(&printer, this);
    dialog.setWindowTitle(tr("Print Document"));
    if (dialog.exec() != QDialog::Accepted)
        return;

    QPainter painter;
    painter.begin(&printer);
    double xscale = printer.pageRect().width()/double(this->chartView->width());
    double yscale = printer.pageRect().height()/double(this->chartView->height());
    double scale = qMin(xscale, yscale);
    painter.translate(printer.paperRect().x() + printer.pageRect().width()/2,
                       printer.paperRect().y() + printer.pageRect().height()/3);
    painter.scale(scale, scale);
    painter.translate(-this->chartView->width()/2, -this->chartView->height()/2);

    this->chartView->render(&painter);

    if(statistikBy == 'm'){
        int first;
        int last;
        for(int i = 0; i < 12; i++){
            if(bulan.at(i) == ui->dariBulan->currentText())
                first = i;
            if(bulan.at(i) == ui->sampaiBulan->currentText())
                last = i;
        }
        if(first > last){
            int x = first;
            first = last;
            last = x;
        }
        this->loadTableStats("month",first,last,ui->bulanTahun->currentText().toInt());
    }else if(statistikBy == 'y'){
        int first = ui->dariTahun->currentText().toInt();
        int last = ui->sampaiTahun->currentText().toInt();
        if(first > last){
            int x = first;
            first = last;
            last = x;
        }
        this->loadTableStats("year",first,last);
    }
    xscale = printer.pageRect().width()/double(this->tableStatistik->width());
    yscale = printer.pageRect().height()/double(this->tableStatistik->height());
    scale = qMin(xscale, yscale);
    painter.translate(printer.paperRect().x() + printer.pageRect().width()/2,
                       printer.paperRect().y() + printer.pageRect().height()/3);
    painter.scale(scale, scale);
    painter.translate(-this->tableStatistik->width()/2, -this->tableStatistik->height()/2+this->chartView->height());


    this->tableStatistik->render(&painter);
}

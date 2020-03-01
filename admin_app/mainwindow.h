#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "tambahdataa.h"
#include "editdataa.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QKeyEvent>
#include <QStandardItem>

#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void keyPressEvent(QKeyEvent *event);

    // **********************************
    // ******** tool bar button *********
    // **********************************

    void on_daftarAnggotaToolButton_clicked();

    void on_daftarPengunjungToolButton_clicked();

    void on_dataStatistikToolButton_clicked();

    void on_daftarTamuToolButton_clicked();

    void on_daftarAnggotaTabel_itemDoubleClicked(QTableWidgetItem *item);

    // **********************************
    // ***** daftar anggota page ******
    // **********************************

    void on_refreshDataAButton_clicked();

    void on_editDataAButton_clicked();

    void on_hapusDataAButton_clicked();

    void on_tambahDataAButton_clicked();

    void on_refreshDataPButton_clicked();

    void on_hapusDataPButton_clicked();

    void on_daftarPengunjungTabel_itemDoubleClicked(QTableWidgetItem *item);

    // **********************************
    // ******** data statistik **********
    // **********************************

    void on_statistikPButton_clicked();

    void on_previewButton_clicked();

    void on_perBulanButton_clicked();

    void on_perTahunButton_clicked();

    void on_bulanTahun_currentTextChanged(const QString &arg1);

    void on_refreshDataTButton_clicked();

    void on_hapusDataTButton_clicked();

    void on_daftarTamuTabel_itemDoubleClicked(QTableWidgetItem *item);

    void on_printButton_clicked();

    void on_searchButton_clicked();

    void on_searchByEdit_returnPressed();

    void on_exportTableAButton_clicked();

    void on_exportTableTButton_clicked();

    void on_searchTTButton_clicked();

    void on_searchEdit_returnPressed();

    void on_searchPButton_clicked();

    void on_searchPBy_currentTextChanged(const QString &arg1);

    void on_searchPByEdit_returnPressed();

    void on_exportTablePButton_clicked();

private:
    Ui::MainWindow *ui;
    QString hostname = "127.0.0.1";
    QString dbname = "dbperpustakaan";
    QString username = "root";
    QString password = "unsoed2018";
    QSqlDatabase db;

    QMessageBox *msgbox;

    // page daftar anggota
    tambahDataA *tambahData;
    editdataa *editData;
    void loadDataA();
    int getRowCount(QChar aorp);

    // page daftar pengunjung
    void loadDataP();

    // page daftar tamu
    void loadDataT();

    // page data statistik
    QStringList bulan = {"Januari","Februari","Maret","April","Mei","Juni","Juli","Agustus","September","Oktober","November","Desember"};
    QStringList noBulan = {"01","02","03","04","05","06","07","08","09","10","11","12"};
    char statistikBy;
    QChartView *chartView;
    void loadChart(QString mode, int first, int last, int year = 0);
    QTableWidget *tableStatistik;
    void loadTableStats(QString mode, int first, int last, int year = 0);
    QStringList loadStats(char DorCorMorTorA,QString mode, int first, int last, int year = 0);
    QStandardItemModel *model;
};

#endif // MAINWINDOW_H

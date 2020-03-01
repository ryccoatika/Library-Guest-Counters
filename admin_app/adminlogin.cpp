#include "adminlogin.h"
#include "ui_adminlogin.h"

#include <QMessageBox>
#include <QDesktopWidget>

adminLogin::adminLogin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::adminLogin)
{
    ui->setupUi(this);

    int x = QApplication::desktop()->screen()->width()/2 - ui->groupBox->width()/2;
    int y = QApplication::desktop()->screen()->height()/2 - ui->groupBox->height()/2;
    ui->groupBox->move(x,y);
}

adminLogin::~adminLogin()
{
    delete ui;
}

void adminLogin::on_exitButton_clicked()
{
    this->close();
    parentWidget()->close();
}

void adminLogin::on_loginButton_clicked()
{
    if(ui->usernameEdit->text().trimmed().isEmpty() || ui->passwordEdit->text().isEmpty()){
        QMessageBox::information(this,"Admin Login","Masukkan username dan password anda");
        ui->usernameEdit->setFocus();
        return;
    }
    if(ui->usernameEdit->text() != "unsoed" || ui->passwordEdit->text() != "unsoed2018"){
        QMessageBox::warning(this,"Admin Login","Username atau password salah");
        ui->passwordEdit->clear();
        ui->usernameEdit->setFocus();
        return;
    }
    this->close();
    parentWidget()->setFixedSize(QApplication::desktop()->screen()->width(),QApplication::desktop()->screen()->height());
    parentWidget()->showFullScreen();
}

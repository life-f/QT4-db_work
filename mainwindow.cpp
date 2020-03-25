/**
  * @file mainwindow.cpp
  * @author Bagrova Polina
  * @brief Class MainWindow
  */
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlQueryModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    ui->WTable->hide();
    ui->Redactor->hide();

    db = QSqlDatabase::addDatabase("QSQLITE");

    connect(ui->openDB, &QPushButton::clicked, this, &MainWindow::connectDatabase);
    connect(ui->showTable, &QPushButton::clicked, this, &MainWindow::showTable);
    connect(ui->queryTable,  &QPushButton::clicked, this, &MainWindow::initQuery);
    connect(ui->add, &QPushButton::clicked, this, &MainWindow::addS);
    connect(ui->delete_2, &QPushButton::clicked, this, &MainWindow::deleteS);
    connect(ui->save, &QPushButton::clicked, this, &MainWindow::updateS);
    connect(ui->spinBox, &QSpinBox::textChanged, this, &MainWindow::change);
    connect(ui->Table, &QLineEdit::textChanged, this, &MainWindow::changeTable);
}

MainWindow::~MainWindow() {
    db.close();
    delete ui;
}

void MainWindow::connectDatabase(){
    db.setDatabaseName(ui->lineEdit->displayText());
    if (db.open()){
        ui->WTable->show();
    }
    else {
        qDebug() << "Невозможно открыть БД";
        return;
    }
}

void MainWindow::showTable(){
    QSqlQuery q;
    QSqlTableModel *table = new QSqlTableModel(parent(),db);
    if (!q.exec("SELECT * FROM " + ui->Table->displayText())) {
        createTable(ui->Table->displayText());
    } else {
        table->setTable(ui->Table->displayText());
        table->select();
        ui->tableView->setModel(table);
    }
}

void MainWindow::initQuery(){
    QSqlQuery q;
    QString select = "SELECT " + ui->entFields->displayText() + " FROM " + ui->nameTable->displayText();
    if (ui->where->displayText() != "")
        select+=" WHERE " + ui->where->displayText();
    if (q.exec(select)) {
        QSqlQueryModel *query = new QSqlQueryModel;
        query->setQuery(select);
        ui->tableView->setModel(query);
    } else {
        qDebug() << "Некорректный запрос";
    }
}

void MainWindow::createTable(QString name){
    ui->Redactor->show();
    QSqlQuery q;
    if(tableName1 == "0"){
        tableName1 = name;
        q.exec("CREATE TABLE " + name + " (sID TEXT PRIMARY KEY NOT NULL, sName TEXT NOT NULL, GPA NUMERIC NOT NULL)");
        setNameFields();
    } else if(tableName2 == "0"){
        tableName2 = name;
        q.exec("CREATE TABLE " + name + " (cName TEXT PRIMARY KEY NOT NULL, state TEXT NOT NULL, enrollment INTEGER NOT NULL)");
        setNameFields();
        clearFields();
    } else
        q.exec("CREATE TABLE " + name + " (id INT PRIMARY KEY AUTOINCREMENT NOT NULL)");

}

void MainWindow::addS(){
    QString name = ui->Table->displayText();
    QSqlQuery query;
    if(name == tableName1){
        query.prepare("INSERT INTO " + name + "(sID, sName, GPA) VALUES (:sID, :sName, :GPA);");
        query.bindValue(":sID", ui->field1->displayText());
        query.bindValue(":sName", ui->field2->displayText());
        query.bindValue(":GPA", ui->field3->displayText());
    } else if (name == tableName2) {
        query.prepare("INSERT INTO " + name + "(cName, state, enrollment) VALUES (:cName, :state, :enrollment);");
        query.bindValue(":cName", ui->field1->displayText());
        query.bindValue(":state", ui->field2->displayText());
        query.bindValue(":enrollment", ui->field3->displayText());
    }
    if(query.exec()){
        qDebug() << "Строка добавлена";
        spinBox();
    }
}

void MainWindow::deleteS(){
    QString name = ui->Table->displayText();
    QSqlQuery query;
    if(name == tableName1){
        query.prepare("DELETE FROM " + name + " WHERE sID = :sID AND sName = :sName AND GPA = :GPA");
        query.bindValue(":sID", ui->field1->displayText());
        query.bindValue(":sName", ui->field2->displayText());
        query.bindValue(":GPA", ui->field3->displayText());
    } else if (name == tableName2) {
        query.prepare("DELETE FROM " + name + " WHERE cName = :cName AND state = :state AND enrollment = :enrollment");
        query.bindValue(":cName", ui->field1->displayText());
        query.bindValue(":state", ui->field2->displayText());
        query.bindValue(":enrollment", ui->field3->displayText());
    }
    if(query.exec()){
        qDebug() << "Строка удалена";
        spinBox();
    }
}

void MainWindow::updateS(){
    QString name = ui->Table->displayText();
    QSqlQuery query;
    if(name == tableName1){
        query.prepare("UPDATE " + name + " SET sID = :sID , sName = :sName , GPA = :GPA WHERE sID = :ressID");
        query.bindValue(":sID", ui->field1->displayText());
        query.bindValue(":sName", ui->field2->displayText());
        query.bindValue(":GPA", ui->field3->displayText());
        query.bindValue(":ressID", this->field1);
    } else if (name == tableName2) {
        query.prepare("UPDATE " + name + " SET cName = :cName , state = :state , enrollment = :enrollment WHERE cName = :rescName");
        query.bindValue(":cName", ui->field1->displayText());
        query.bindValue(":state", ui->field2->displayText());
        query.bindValue(":enrollment", ui->field3->displayText());
        query.bindValue(":rescName", this->field1);
    }
    if(query.exec())
        qDebug() << "Строка отредактирована";
}

void MainWindow::change(QString n){
    int i = n.toInt();
    QString name = ui->Table->displayText();
    QSqlQuery query;
    query.exec("SELECT COUNT(*) FROM " + name);
    query.next();
    if(query.value(0).toInt() > 0){
        query.exec("SELECT * FROM " + name);
        int k=0;
        while(k!=i){
            query.next();
            k++;
        }
        ui->field1->setText(query.value(0).toString());
        ui->field2->setText(query.value(1).toString());
        ui->field3->setText(query.value(2).toString());
        this->field1 = ui->field1->displayText();
    } else {
        ui->spinBox->setMinimum(0);
        qDebug() << "Нет записей";
        return;
    }
}

void MainWindow::setNameFields(){
    if(ui->Table->displayText() == tableName1){
        ui->nameField1->setText("sID");
        ui->nameField2->setText("sName");
        ui->nameField3->setText("GPA");
    } else if(ui->Table->displayText() == tableName2){
        ui->nameField1->setText("cName");
        ui->nameField2->setText("state");
        ui->nameField3->setText("enrollment");
    }
}

void MainWindow::clearFields(){
    ui->field1->clear();
    ui->field2->clear();
    ui->field3->clear();
}

void MainWindow::spinBox(){
        QSqlQuery q;
        q.exec("SELECT COUNT(*) FROM " + ui->Table->displayText());
        q.next();
        if(q.value(0).toInt() == 0)
            return;
        ui->spinBox->setMinimum(1);
        ui->spinBox->setMaximum(q.value(0).toInt());
}

void MainWindow::changeTable(){
    if(ui->Table->displayText() == tableName1 || ui->Table->displayText() == tableName2){
        spinBox();
        setNameFields();
        change("1");
    }
}

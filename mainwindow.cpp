#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include "activitydialog.h"
#include <QToolButton>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QDebug>
#include <QIcon>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    load();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::load(){
    QFile file(log);
    if(file.open(QIODevice::ReadOnly)){
        QTextStream in(&file);
        while(!in.atEnd()){
            QStringList dati;
            QString riga = in.readLine();
            if(riga.length()>0){
                dati = riga.split('\t');
                activities.push_back(attivita(dati));
            }
        }
    }
    file.close();
}

void MainWindow::showActivities(){
    QStringList headers = {"Tipo Attività", "Titolo", "Ora Inizio", "Ora Fine", "Ripeti", "Azioni"};
    QDate data = ui->calendarWidget->selectedDate();
    std::vector<int> pos;
    for(unsigned int i=0; i<activities.size(); i++){
        if(activities.at(i).finish.date() >= data){
            QString rep = activities.at(i).repeats;
            int r = -1;
            if(rep == "Mai")
                r = 0;
            if(rep == "Ogni giorno")
                r = 1;
            if(rep == "Ogni mese")
                r = 2;
            if(rep == "Ogni anno")
                r = 3;
            if(rep == "Ogni settimana")
                r = 4;
            switch(r){
            case 0:
                if(activities.at(i).data==data){
                    pos.push_back(i);
                }
                break;
            case 1:
                if(activities.at(i).data <= data){
                    pos.push_back(i);
                }
                break;
            case 2:
                if(activities.at(i).data.day()==data.day() && activities.at(i).data <= data){
                    pos.push_back(i);
                }
                break;
            case 3:
                if(activities.at(i).data.day()==data.day() && activities.at(i).data.month()==data.month() && activities.at(i).data <= data){
                    pos.push_back(i);
                }
                break;
            case 4:
                if(activities.at(i).data.dayOfWeek() == data.dayOfWeek() && activities.at(i).data <= data){
                    pos.push_back(i);
                }
                break;
            default: break;
            }
        }
    }
    ui->Attivita->setColumnCount(6);
    ui->Attivita->setColumnWidth(0, 130);
    ui->Attivita->setColumnWidth(1, 250);
    ui->Attivita->setColumnWidth(2, 75);
    ui->Attivita->setColumnWidth(3, 75);
    ui->Attivita->setColumnWidth(4, 120);
    ui->Attivita->setColumnWidth(5, 80);
    ui->Attivita->setRowCount(pos.size());
    ui->Attivita->setFont(QFont ("Times", 10, QFont::Bold));
    ui->Attivita->setHorizontalHeaderLabels(headers.last(6));
    ui->Attivita->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->Attivita->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->Attivita->setStyleSheet("QTableWidget::item { padding: 0px }");
    ui->Attivita->setSortingEnabled(false);
    for(unsigned int i=0; i<pos.size(); i++){
        ui->Attivita->setRowHeight(i, 40);
        QStringList ActInfo = activities.at(pos.at(i)).getInfo();
        for(int j=0; j<5; j++){
            QTableWidgetItem *el = new QTableWidgetItem();
            el->setData(Qt::EditRole, ActInfo.at(j));
            ui->Attivita->setItem(i, j, el);
            ui->Attivita->item(i, j)->setFlags((ui->Attivita->item(i,j)->flags() ^ Qt::ItemIsEditable));
        }
        QToolButton *del = new QToolButton(this);
        QToolButton *edit = new QToolButton(this);
        del->setToolButtonStyle(Qt::ToolButtonIconOnly);
        edit->setToolButtonStyle(Qt::ToolButtonIconOnly);
        del->setIcon(QIcon::fromTheme("list-add", QIcon("delete.png")));
        edit->setIcon(QIcon::fromTheme("list-add", QIcon("edit.png")));
        del->setText(QString::number(pos.at(i)));
        edit->setText(QString::number(pos.at(i)));
        QGroupBox *box = new QGroupBox();
        QHBoxLayout *layout = new QHBoxLayout;
        layout->addWidget(del);
        layout->addWidget(edit);
        box->setLayout(layout);
        connect(del, &QToolButton::clicked , this, &MainWindow::removeActivity);
        connect(edit, &QToolButton::clicked , this, &MainWindow::editActivity);
        ui->Attivita->setCellWidget(i, 5, box);
    }
    ui->Attivita->setSortingEnabled(true);
    ui->Attivita->sortByColumn(2, Qt::AscendingOrder);
}


void MainWindow::on_AddActivity_clicked()
{
    QDate selectedDate = ui->calendarWidget->selectedDate();
    ActivityDialog *act = new ActivityDialog(this, selectedDate, &activities);
    if(act->exec()){
        showActivities();
    }
}

void MainWindow::addActivity(attivita a){
    activities.push_back(a);
}

void MainWindow::removeActivity(){
    QToolButton *sender = qobject_cast<QToolButton*>(QObject::sender());
    int pos = std::stoi(sender->text().toStdString());
    activities.erase(activities.begin()+pos);
    QFile file(log);
    int i=0;
    QStringList testo;
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream in(&file);
        while(!in.atEnd()){
            QString riga = in.readLine();
            if(i != pos){
                testo.append(riga);
            }
            i++;
        }
    }
    file.close();
    if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream in(&file);
        for(int i=0; i<testo.size(); i++){
            in << testo.at(i) << '\n';
        }
    }
    file.close();
    showActivities();
}

void MainWindow::editActivity(){
    QToolButton *sender = qobject_cast<QToolButton*>(QObject::sender());
    int pos = std::stoi(sender->text().toStdString());
    QFile file(log);
    int i=0;
    QStringList testo;
    int result = -1;
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream in(&file);
        while(!in.atEnd()){
            QString riga = in.readLine();
            if(i != pos){
                testo.append(riga);
            }else{
                QDate selectedDate = activities.at(i).getData();
                ActivityDialog *act = new ActivityDialog(this, selectedDate, &activities, true, pos);
                result = act->exec();
                if(result == QDialog::Accepted){
                    activities.at(pos) = act->edited_activity;
                    int y = selectedDate.year();
                    int m = selectedDate.month();
                    int d = selectedDate.day();
                    QString dt = QString::number(y)+":"+QString::number(m)+":"+QString::number(d);
                    QStringList info = act->edited_activity.getInfo();
                    QString fin = info.at(5);;
                    if(info.at(3).length() == 0)
                        fin += " 23:59:59";
                    else
                        fin += " " + info.at(3);
                    testo.append(info.at(0)+'\t'+info.at(1)+'\t'+info.at(2)+'\t'+fin+'\t'+info.at(4)+'\t'+dt);
                    riga = "";
                }
            }
            i++;
        }
    }
    file.close();
    if(result == QDialog::Accepted){
        if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
            QTextStream in(&file);
            for(int i=0; i<testo.size(); i++){
                in << testo.at(i) << '\n';
            }
        }
        file.close();
    }
    showActivities();
}

void MainWindow::on_calendarWidget_selectionChanged()
{
    showActivities();
}


void MainWindow::on_calendarWidget_activated(const QDate &date)
{
    showActivities();
}


void MainWindow::on_calendarWidget_clicked(const QDate &date)
{
    showActivities();
}


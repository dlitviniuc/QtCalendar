#include "activitydialog.h"
#include "ui_activitydialog.h"
#include "mainwindow.h"
#include <QTextStream>
#include <QFile>
#include <QDebug>


ActivityDialog::ActivityDialog(QWidget *parent, QDate data, std::vector<attivita> *v, bool edit, int pos) :
    QDialog(parent),
    ui(new Ui::ActivityDialog)
{
    this->pos = pos;
    this->edit = edit;
    this->data = data;
    this->punt = v;
    ui->setupUi(this);
    ui->buttonBox->buttons().at(0)->setEnabled(false);
    ui->fine->setDate(data);
    this->data_fine = data;
    if(pos!=-1){
        this->data_fine = punt->at(pos).getFinish().date();
        if(punt->at(pos).tipo == "Evento")
            ui->tipo->setCurrentIndex(1);
        else
            ui->tipo->setCurrentIndex(0);
        ui->titolo->setText(punt->at(pos).getTitolo());
        ui->inizio->setTime(punt->at(pos).getStart());
        if(punt->at(pos).getFinish().time() == QTime(23, 59, 59))
            ui->NoEnd->setChecked(true);
        else
            ui->fine->setDateTime(punt->at(pos).getFinish());
        if(punt->at(pos).getRepeats() == "Mai")
            ui->ripeti->setCurrentIndex(0);
        if(punt->at(pos).getRepeats() == "Ogni giorno")
            ui->ripeti->setCurrentIndex(1);
        if(punt->at(pos).getRepeats() == "Ogni mese")
            ui->ripeti->setCurrentIndex(3);
        if(punt->at(pos).getRepeats() == "Ogni anno")
            ui->ripeti->setCurrentIndex(4);
        if(punt->at(pos).getRepeats() == "Ogni settimana")
            ui->ripeti->setCurrentIndex(2);
    }
    ui->buttonBox->buttons().at(0)->setEnabled(edit);
}

ActivityDialog::~ActivityDialog()
{
    delete ui;
}

void ActivityDialog::on_buttonBox_accepted()
{
    this->setAttivita();
}

void ActivityDialog::setAttivita(){
    attivita act;
    QStringList info;
    info.append(ui->tipo->currentText());
    info.append(ui->titolo->text());
    info.append(ui->inizio->text());
    QDate dttf = ui->fine->date();
    int yf = dttf.year();
    int mf = dttf.month();
    int df = dttf.day();
    QString dtf = QString::number(yf)+":"+QString::number(mf)+":"+QString::number(df);
    info.append(dtf + " " +ui->fine->time().toString());
    info.append(ui->ripeti->currentText());
    int y = data.year();
    int m = data.month();
    int d = data.day();
    QString dt = QString::number(y)+":"+QString::number(m)+":"+QString::number(d);
    info.append(dt);
    act = attivita(info);
    if(!edit)
        punt->push_back(act);
    QFile file("logs.txt");
    if(file.open(QIODevice::WriteOnly | QIODevice::Append | QFile::Text)){
        QTextStream out(&file);
        QStringList info = act.getInfo();
        QString fin = "";
        if(info.at(3).length() == 0){
            fin = info.at(5);
            fin += " 23:59:59";
        }
        else{
            fin = info.at(5);
            fin += " " + info.at(3);
        }
        if(!edit)
            out << info.at(0)+'\t'+info.at(1)+'\t'+info.at(2)+'\t'+fin+'\t'+info.at(4)+'\t'+dt+'\n';
    }
    file.close();
    edited_activity = act;
}


void ActivityDialog::on_inizio_userTimeChanged(const QTime &time)
{
    if(time >= ui->fine->time()){
        ui->Error->setText("<font color='red'> Orario errato <font>");
        ui->buttonBox->buttons().at(0)->setEnabled(false);
    }else{
        ui->buttonBox->buttons().at(0)->setEnabled(checkFree());
        if(checkFree())
            ui->Error->setText("");
    }
}


bool ActivityDialog::checkFree(){
    QTime inizio = ui->inizio->time();
    QTime fine = ui->fine->time();
    if(ui->tipo->currentText()== "Evento")
        return true;
    for(unsigned int i=0; i<punt->size(); i++){
        if(i != pos){
            if(ui->fine->date() >= punt->at(i).getData() && punt->at(i).finish.date() >= data){
                QTime pIn = punt->at(i).getStart();
                QTime pF = punt->at(i).getFinish().time();
                QString rep = punt->at(i).repeats;
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
                    if(punt->at(i).data==data || ui->fine->date() > data){
                        if((inizio >= pIn && inizio<pF) || (fine > pIn && fine < pF)|| (inizio <pIn && fine >= pF)){
                            ui->Error->setText("<font color='red'>Si sovrappone con " + punt->at(i).titolo + " da " + punt->at(i).data.toString() +"<font>");
                            return false;
                        }
                    }
                    break;
                case 1:
                    if((inizio>=pIn && inizio<pF) || (fine > pIn && fine < pF)|| (inizio <pIn && fine >= pF)){
                        if(punt->at(i).data > data && ui->ripeti->currentText() == "Mai")
                            break;
                        ui->Error->setText("<font color='red'>Si sovrappone con " + punt->at(i).titolo + " da " + punt->at(i).data.toString() +"<font>");
                        return false;
                    }
                    break;
                case 2:
                    if(punt->at(i).data.day()==data.day()){
                        if((inizio>=pIn && inizio<pF) || (fine > pIn && fine < pF)|| (inizio <pIn && fine >= pF)){
                            if(punt->at(i).data > data && ui->ripeti->currentText() != "Ogni mese" && ui->ripeti->currentText() != "Ogni anno" && ui->ripeti->currentText() != "Ogni giorno")
                                break;
                            ui->Error->setText("<font color='red'>Si sovrappone con " + punt->at(i).titolo + " da " + punt->at(i).data.toString() +"<font>");
                            return false;
                        }
                    }
                    break;
                case 3:
                    if(punt->at(i).data.day()==data.day() && punt->at(i).data.month()==data.month()){
                        if((inizio>=pIn && inizio<pF) || (fine > pIn && fine < pF) || (inizio <pIn && fine >= pF)){
                            if(punt->at(i).data > data && ui->ripeti->currentText() != "Ogni anno" && ui->ripeti->currentText() != "Ogni mese" && ui->ripeti->currentText() != "Ogni giorno")
                                break;
                            ui->Error->setText("<font color='red'>Si sovrappone con " + punt->at(i).titolo + " da " + punt->at(i).data.toString() +"<font>");
                            return false;
                        }
                    }
                    break;
                case 4:if(punt->at(i).data.dayOfWeek() == data.dayOfWeek()){
                        if((inizio>=pIn && inizio<pF) || (fine > pIn && fine < pF) || (inizio <pIn && fine >= pF)){
                            if(punt->at(i).data > data && ui->ripeti->currentText() != "Ogni settimana" && ui->ripeti->currentText() != "Ogni giorno")
                                break;
                            ui->Error->setText("<font color='red'>Si sovrappone con " + punt->at(i).titolo + " da " + punt->at(i).data.toString() +"<font>");
                            return false;
                        }
                    }
                    break;
                default: break;
                }
            }
        }
    }
    return true;
}

void ActivityDialog::on_NoEnd_clicked(bool checked)
{
    if(checked){
        QDate d(2999, 12, 30);
        ui->fine->setDate(d);
        QTime t;
        t.setHMS(23, 59, 59);
        ui->fine->setTime(t);
        ui->fine->setEnabled(false);
    }else{
        ui->fine->setDate(data_fine);
        ui->fine->setTime(QTime(12, 00));
        ui->fine->setEnabled(true);
    }
}


void ActivityDialog::on_ripeti_currentIndexChanged(int index)
{
    finishCheck();
    if(index == 0){
        ui->fine->setDate(data);
    }
}


void ActivityDialog::on_fine_dateTimeChanged(const QDateTime &dateTime)
{
    finishCheck();
}


void ActivityDialog::on_tipo_currentIndexChanged(int index)
{
    finishCheck();
}

void ActivityDialog::finishCheck(){
    QTime time = ui->fine->dateTime().time();
    QDate date = ui->fine->dateTime().date();
    bool err = true;
    if(ui->ripeti->currentIndex() == 0)
        ui->fine->setDate(data_fine);
    if(ui->ripeti->currentIndex() > 0 && date == data){
        ui->Error->setText("<font color='red'> Data fine = Data inizio, no ripetizione <font>");
        err = false;
    }
    date = ui->fine->dateTime().date();
    if(time <= ui->inizio->time() || date < data){
        ui->Error->setText("<font color='red'> Orario errato <font>");
        ui->buttonBox->buttons().at(0)->setEnabled(false);
    }else{
        ui->buttonBox->buttons().at(0)->setEnabled(checkFree());
        if(checkFree() && err)
            ui->Error->setText("");
    }
}

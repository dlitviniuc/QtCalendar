#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct attivita{
    QString tipo;
    QString titolo;
    QTime start;
    QDateTime finish;
    QDate data;
    QString repeats;

    attivita(){
        tipo = "Vuoto";
        titolo = "Vuoto";
        start = QTime();
        finish = QDateTime();
        repeats = "Mai";
    }

    attivita(QString type, QString title, QTime st, QDateTime end, QString rep, QDate date){
        tipo = type;
        titolo = title;
        start = st;
        finish = end;
        repeats = rep;
        data = date;
    }
    attivita(QStringList dati){
        tipo = dati.at(0);
        titolo = dati.at(1);
        QStringList st = dati.at(2).split(':');
        start = QTime(std::stoi(st.at(0).toStdString()), std::stoi(st.at(1).toStdString()));
        QStringList fin = dati.at(3).split(' ');
        QStringList tmp_data = fin.at(0).split(":");
        QStringList ora = fin.at(1).split(":");
        QDate d = QDate(std::stoi(tmp_data.at(0).toStdString()), std::stoi(tmp_data.at(1).toStdString()), std::stoi(tmp_data.at(2).toStdString()));
        QTime t = QTime(std::stoi(ora.at(0).toStdString()), std::stoi(ora.at(1).toStdString()));
        finish = QDateTime(d, t);
        repeats = dati.at(4);
        QStringList dt = dati.at(5).split(':');
        data = QDate(std::stoi(dt.at(0).toStdString()), std::stoi(dt.at(1).toStdString()), std::stoi(dt.at(2).toStdString()));
    }

    const QString getTipo(){
        return tipo;
    }
    const QString getTitolo(){
        return titolo;
    }
    const QString getRepeats(){
        return repeats;
    }

    const QTime getStart(){
        return start;
    }
    const QDateTime getFinish(){
        return finish;
    }
    const QDate getData(){
        return data;
    }

    QStringList getInfo(){
        QStringList info;
        info.append(tipo);
        info.append(titolo);
        info.append(start.toString());
        if(finish.time() != QTime(23, 59))
            info.append(finish.time().toString());
        else
            info.append("");
        info.append(repeats);
        QDate fin_date = finish.date();
        int yf = fin_date.year();
        int mf = fin_date.month();
        int df = fin_date.day();
        QString dt = QString::number(yf)+":"+QString::number(mf)+":"+QString::number(df);
        info.append(dt);
        return info;
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void addActivity(attivita a);

private slots:
    void on_AddActivity_clicked();
    void removeActivity();
    void editActivity();
    void on_calendarWidget_selectionChanged();

    void on_calendarWidget_activated(const QDate &date);

    void on_calendarWidget_clicked(const QDate &date);

private:
    Ui::MainWindow *ui;
    QString log = "logs.txt";
    int position = 0;

    std::vector<attivita> activities;
    void load();
    void showActivities();

};
#endif // MAINWINDOW_H

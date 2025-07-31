#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QString>
#include "qcustomplot.h"


class MainWindow : public QMainWindow {
    Q_OBJECT

private slots:
    void addShoe();
    void removeShoe();
    void filterByManufacturer();
    void checkShoeAvailability();
    void showManufacturerSummary();
    void loadDatabase();
    void saveDatabase();
    void showSizeChart();
    void resetFilter();

private:
    QTableWidget *tableWidget;
    QString databaseFile = "shoes_database.txt";
    void setupUI();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};

#endif // MAINWINDOW_H

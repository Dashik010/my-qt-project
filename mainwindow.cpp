#include "mainwindow.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QInputDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QMap>

#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
    loadDatabase();
}

MainWindow::~MainWindow() {
    saveDatabase();
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    // Стилизация таблицы
    tableWidget = new QTableWidget(0, 4, this);
    tableWidget->setHorizontalHeaderLabels({"Модель", "Производитель", "Размер", "Кол-во пар"});
    tableWidget->setStyleSheet("QTableWidget { border: 1px solid #ddd; background: #f8f8f8; } ");
    layout->addWidget(tableWidget);

    // Создаем кнопки
    QPushButton *btnAdd = new QPushButton("Добавить обувь", this);
    QPushButton *btnRemove = new QPushButton("Удалить обувь", this);
    QPushButton *btnFilter = new QPushButton("Фильтр по производителю", this);
    QPushButton *btnCheck = new QPushButton("Проверить наличие", this);
    QPushButton *btnSummary = new QPushButton("Сводка по производителям", this);
    QPushButton *btnChart = new QPushButton("Построить график", this);
    QPushButton *btnResetFilter = new QPushButton("Сбросить фильтр", this);
    QPushButton *btnSave = new QPushButton("Сохранить данные", this);
    QPushButton *btnLoad = new QPushButton("Загрузить данные", this);

    // Применение стиля ко всем кнопкам
    btnAdd->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 5px 15px; border-radius: 5px; } QPushButton:hover { opacity: 0.9; }");
    btnRemove->setStyleSheet("QPushButton { background-color: #F44336; color: white; padding: 5px 15px; border-radius: 5px; } QPushButton:hover { opacity: 0.9; }");
    btnFilter->setStyleSheet("QPushButton { background-color: #8BC34A; color: white; padding: 5px 15px; border-radius: 5px; } QPushButton:hover { opacity: 0.9; }");
    btnCheck->setStyleSheet("QPushButton { background-color: #FFC107; color: white; padding: 5px 15px; border-radius: 5px; } QPushButton:hover { opacity: 0.9; }");
    btnSummary->setStyleSheet("QPushButton { background-color: #2196F3; color: white; padding: 5px 15px; border-radius: 5px; } QPushButton:hover { opacity: 0.9; }");
    btnChart->setStyleSheet("QPushButton { background-color: #00BCD4; color: white; padding: 5px 15px; border-radius: 5px; } QPushButton:hover { opacity: 0.9; }");
    btnResetFilter->setStyleSheet("QPushButton { background-color: #CE93D8; color: white; padding: 5px 15px; border-radius: 5px; } QPushButton:hover { opacity: 0.9; }");
    btnSave->setStyleSheet("QPushButton { background-color: #9C27B0; color: white; padding: 5px 15px; border-radius: 5px; } QPushButton:hover { opacity: 0.9; }");
    btnLoad->setStyleSheet("QPushButton { background-color: #009688; color: white; padding: 5px 15px; border-radius: 5px; } QPushButton:hover { opacity: 0.9; }");

    // Компоновка кнопок
    QGridLayout *buttonLayout = new QGridLayout();
    buttonLayout->addWidget(btnAdd, 0, 0);
    buttonLayout->addWidget(btnRemove, 0, 1);
    buttonLayout->addWidget(btnFilter, 1, 0);
    buttonLayout->addWidget(btnCheck, 1, 1);
    buttonLayout->addWidget(btnSummary, 2, 0, 1, 2);
    buttonLayout->addWidget(btnChart, 3, 0, 1, 2);
    buttonLayout->addWidget(btnResetFilter, 4, 0, 1, 2);
    buttonLayout->addWidget(btnSave, 5, 0);
    buttonLayout->addWidget(btnLoad, 5, 1);

    layout->addLayout(buttonLayout);

    setCentralWidget(centralWidget);

    // Привязка кнопок к функциям
    connect(btnAdd, &QPushButton::clicked, this, &MainWindow::addShoe);
    connect(btnRemove, &QPushButton::clicked, this, &MainWindow::removeShoe);
    connect(btnFilter, &QPushButton::clicked, this, &MainWindow::filterByManufacturer);
    connect(btnCheck, &QPushButton::clicked, this, &MainWindow::checkShoeAvailability);
    connect(btnSummary, &QPushButton::clicked, this, &MainWindow::showManufacturerSummary);
    connect(btnChart, &QPushButton::clicked, this, &MainWindow::showSizeChart);
    connect(btnResetFilter, &QPushButton::clicked, this, &MainWindow::resetFilter);
    connect(btnSave, &QPushButton::clicked, this, &MainWindow::saveDatabase);
    connect(btnLoad, &QPushButton::clicked, this, &MainWindow::loadDatabase);
}


void MainWindow::loadDatabase() {
    QFile file(databaseFile);

    // Если файл не существует — создаем пустой
    if (!file.exists()) {
        QMessageBox::information(this, "Загрузка данных", "Файл не найден. Создан новый файл.");
        file.open(QIODevice::WriteOnly);
        file.close();
        return;
    }

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        tableWidget->setRowCount(0);  // Очищаем таблицу перед загрузкой новых данных
        QTextStream in(&file);
        while (!in.atEnd()) {
            QStringList data = in.readLine().split(",");
            if (data.size() == 4) {
                int row = tableWidget->rowCount();
                tableWidget->insertRow(row);
                for (int col = 0; col < 4; ++col) {
                    tableWidget->setItem(row, col, new QTableWidgetItem(data[col]));
                }
            }
        }
        file.close();
        QMessageBox::information(this, "Загрузка данных", "Данные успешно загружены!");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл!");
    }
}

void MainWindow::saveDatabase() {
    QFile file(databaseFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (int row = 0; row < tableWidget->rowCount(); ++row) {
            for (int col = 0; col < 4; ++col) {
                out << tableWidget->item(row, col)->text() << (col < 3 ? "," : "\n");
            }
        }
        file.close();
        QMessageBox::information(this, "Сохранение данных", "Данные успешно сохранены!");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл!");
    }
}

// Вынесенная функция для проверки чисел
bool isNumber(const QString &s) {
    bool ok;
    int num = s.toInt(&ok);
    return ok && num > 0;  // проверка на положительное число
}

void MainWindow::addShoe() {
    QString model = QInputDialog::getText(this, "Добавить обувь", "Модель:");
    QString manufacturer = QInputDialog::getText(this, "Добавить обувь", "Производитель:");
    QString size = QInputDialog::getText(this, "Добавить обувь", "Размер:");
    QString quantity = QInputDialog::getText(this, "Добавить обувь", "Количество пар:");

    // Проверка на пустые поля
    if (model.isEmpty() || manufacturer.isEmpty() || size.isEmpty() || quantity.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Все поля должны быть заполнены!");
        return;
    }

    // Проверка на корректные числа
    if (!isNumber(size) || !isNumber(quantity)) {
        QMessageBox::warning(this, "Ошибка", "Размер и количество пар должны быть положительными числами!");
        return;
    }

    // Добавление строки в таблицу только после всех проверок
    int row = tableWidget->rowCount();
    tableWidget->insertRow(row);
    tableWidget->setItem(row, 0, new QTableWidgetItem(model));
    tableWidget->setItem(row, 1, new QTableWidgetItem(manufacturer));
    tableWidget->setItem(row, 2, new QTableWidgetItem(size));
    tableWidget->setItem(row, 3, new QTableWidgetItem(quantity));
}

void MainWindow::removeShoe() {
    int row = tableWidget->currentRow();
    if (row >= 0) tableWidget->removeRow(row);
}

void MainWindow::filterByManufacturer() {
    QString manufacturer = QInputDialog::getText(this, "Фильтр", "Введите производителя:");
    for (int row = 0; row < tableWidget->rowCount(); ++row) {
        tableWidget->setRowHidden(row, tableWidget->item(row, 1)->text() != manufacturer);
    }
}

void MainWindow::checkShoeAvailability() {
    QString model = QInputDialog::getText(this, "Проверка", "Введите модель:");
    QString size = QInputDialog::getText(this, "Проверка", "Введите размер:");
    for (int row = 0; row < tableWidget->rowCount(); ++row) {
        if (tableWidget->item(row, 0)->text() == model && tableWidget->item(row, 2)->text() == size) {
            QMessageBox::information(this, "Наличие", "Обувь в наличии!");
            return;
        }
    }
    QMessageBox::warning(this, "Наличие", "Обувь отсутствует.");
}

void MainWindow::showManufacturerSummary() {
    QMap<QString, QPair<int, int>> countMap;

    for (int row = 0; row < tableWidget->rowCount(); ++row) {
        QString manufacturer = tableWidget->item(row, 1)->text();
        int quantity = tableWidget->item(row, 3)->text().toInt();
        countMap[manufacturer].first++;  // количество моделей
        countMap[manufacturer].second += quantity;  // количество пар
    }

    QString summary;
    for (auto it = countMap.begin(); it != countMap.end(); ++it) {
        summary += it.key() + " - " + QString::number(it.value().first) + " моделей, " + QString::number(it.value().second) + " пар\n";
    }

    QMessageBox::information(this, "Сводка", summary);
}


void MainWindow::showSizeChart() {
    QString model = QInputDialog::getText(this, "Построить график", "Введите модель:");
    if (model.isEmpty()) return;

    QMap<QString, int> sizeCount;
    for (int row = 0; row < tableWidget->rowCount(); ++row) {
        if (tableWidget->item(row, 0)->text() == model) {
            QString size = tableWidget->item(row, 2)->text();
            int quantity = tableWidget->item(row, 3)->text().toInt();
            sizeCount[size] += quantity;
        }
    }

    if (sizeCount.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Модель не найдена!");
        return;
    }

    QDialog *chartDialog = new QDialog(this);
    chartDialog->setWindowTitle("График модели: " + model);
    chartDialog->resize(600, 400);

    QVBoxLayout *layout = new QVBoxLayout(chartDialog);
    QCustomPlot *customPlot = new QCustomPlot(chartDialog);

    QVector<double> ticks;
    QVector<double> values;
    QStringList labels;

    int index = 1;
    for (auto it = sizeCount.begin(); it != sizeCount.end(); ++it) {
        ticks << index++;
        labels << it.key();
        values << it.value();
    }

    // Создаем столбчатую диаграмму
    QCPBars *bars = new QCPBars(customPlot->xAxis, customPlot->yAxis);
    bars->setData(ticks, values);
    bars->setWidth(0.5);
    bars->setPen(QPen(Qt::blue));
    bars->setBrush(QColor(61, 174, 233));


    customPlot->xAxis->setRange(0, ticks.size() + 1);


    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    for (int i = 0; i < ticks.size(); ++i) {
        textTicker->addTick(ticks[i], labels[i]);
    }
    customPlot->xAxis->setTicker(textTicker);
    customPlot->xAxis->setTickLabelRotation(45);
    customPlot->xAxis->setLabel("Размеры");

    // Ось Y
    customPlot->yAxis->setRange(0, *std::max_element(values.begin(), values.end()) + 2);
    customPlot->yAxis->setLabel("Количество пар");

    // Добавляем интерактивность (масштаб и перетаскивание)
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    layout->addWidget(customPlot);
    chartDialog->setLayout(layout);
    chartDialog->exec();
}


void MainWindow::resetFilter() {
    for (int row = 0; row < tableWidget->rowCount(); ++row) {
        tableWidget->setRowHidden(row, false);
    }
}

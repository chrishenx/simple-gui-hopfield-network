#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

#include <utility>

using namespace std;

const QString MainWindow::ASSOCIATE_BUTTON_DEFAULT_STRING = "Asociar";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->neuronCountSpinBox->setValue(neuron_count);
    ui->patternCountSpinBox->setValue(pattern_count);

    configureWindow();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::configureWindow()
{
    ui->patternsTableWidget->horizontalHeader()->setDefaultSectionSize(TABLE_COLUMNS_SIZE);
    ui->patternsTableWidget->setColumnCount(neuron_count);
    ui->patternsTableWidget->setRowCount(pattern_count);

    // The followong line disables further cell style changes
    // ui->patternsTableWidget->setStyleSheet("QTableView::Item { border-bottom: 1px solid gray; } ");

    ui->inputPatternTableWidget->horizontalHeader()->setDefaultSectionSize(TABLE_COLUMNS_SIZE);
    ui->inputPatternTableWidget->setColumnCount(neuron_count);
    ui->inputPatternTableWidget->setRowCount(1);
    ui->inputPatternTableWidget->setEnabled(weighted_matrix_computed);

    ui->outputPatternTableWidget->horizontalHeader()->setDefaultSectionSize(TABLE_COLUMNS_SIZE);

    ui->associateButton->setText(ASSOCIATE_BUTTON_DEFAULT_STRING);
    ui->associateButton->setEnabled(weighted_matrix_computed);

    ui->resetButton->setEnabled(false);
    ui->resetButton->setVisible(false);

    ui->neuronCountSpinBox->setMinimum(neuron_count);
    ui->patternCountSpinBox->setMinimum(pattern_count);

    for (int j = 0; j < neuron_count; ++j) {
        for (int i = 0; i < pattern_count; i++) {
            auto tableWidgetItem = new QTableWidgetItem("1");
            tableWidgetItem->setFlags(tableWidgetItem->flags() ^ Qt::ItemIsEditable ^ Qt::ItemIsSelectable);
            tableWidgetItem->setTextAlignment(Qt::AlignCenter);
            tableWidgetItem->setBackground(QBrush(Qt::white));
            tableWidgetItem->setForeground(QBrush(Qt::black));
            ui->patternsTableWidget->setItem(i, j, tableWidgetItem);
        }
        auto tableWidgetItem = new QTableWidgetItem("1");
        tableWidgetItem->setFlags(tableWidgetItem->flags() ^ Qt::ItemIsEditable ^  Qt::ItemIsSelectable);
        tableWidgetItem->setTextAlignment(Qt::AlignCenter);
        ui->inputPatternTableWidget->setItem(0, j, tableWidgetItem);
    }
}

void MainWindow::on_neuronCountSpinBox_valueChanged(int value)
{
    ui->patternsTableWidget->setColumnCount(value);
    ui->inputPatternTableWidget->setColumnCount(value);
    for (int j = neuron_count; j < value; ++j) {
        for (int i = 0; i < pattern_count; ++i) {
            auto tableWidgetItem = new QTableWidgetItem("1");
            tableWidgetItem->setFlags(tableWidgetItem->flags() ^ Qt::ItemIsEditable ^ Qt::ItemIsSelectable);
            tableWidgetItem->setTextAlignment(Qt::AlignCenter);
            ui->patternsTableWidget->setItem(i, j, tableWidgetItem);
        }
        auto tableWidgetItem = new QTableWidgetItem("1");
        tableWidgetItem->setFlags(tableWidgetItem->flags() ^ Qt::ItemIsEditable ^ Qt::ItemIsSelectable);
        tableWidgetItem->setTextAlignment(Qt::AlignCenter);
        ui->inputPatternTableWidget->setItem(0, j, tableWidgetItem);
    }
    neuron_count = value;
}

void MainWindow::on_patternCountSpinBox_valueChanged(int value)
{
    ui->patternsTableWidget->setRowCount(value);
    for (int i = pattern_count; i < value; ++i) {
        for (int j = 0; j < neuron_count; ++j) {
            auto tableWidgetItem = new QTableWidgetItem("1");
            tableWidgetItem->setFlags(tableWidgetItem->flags() ^ Qt::ItemIsEditable ^ Qt::ItemIsSelectable);
            ui->patternsTableWidget->setItem(i, j, tableWidgetItem);
        }
    }
    pattern_count = value;
}

void MainWindow::on_generateWeightedMatrixButton_clicked()
{
    auto patterns = processInputPatterns();
    weighted_matrix = computeWeightedMatrix(patterns);

    QString formatedMatrix = "{";
    for (const auto& weighted_matrix_row : weighted_matrix) {
        formatedMatrix += "  { ";
        for (const auto& weighted_matrix_element: weighted_matrix_row) {
            formatedMatrix += QString::number(weighted_matrix_element) + " ";
        }
        formatedMatrix += "}  ";
    }
    formatedMatrix += "}";

    qDebug() << formatedMatrix;

    weighted_matrix_computed = true;
    ui->inputPatternTableWidget->setEnabled(weighted_matrix_computed);
    ui->associateButton->setEnabled(weighted_matrix_computed);
    ui->outputPatternTableWidget->setColumnCount(0);
    ui->outputPatternTableWidget->setRowCount(0);
    ui->resetButton->setEnabled(false);
    ui->resetButton->setVisible(false);
    ui->associateButton->setText(ASSOCIATE_BUTTON_DEFAULT_STRING);
    association_step = INIT;
}

void MainWindow::changeTableWidgetItemState(QTableWidget* tableWidget, int row, int column)
{
    auto tableWidgetItem = tableWidget->item(row, column);
    if (tableWidgetItem->text() == "-1") {
        tableWidgetItem->setText("1");
        tableWidgetItem->setBackground(QBrush(Qt::white));
        tableWidgetItem->setForeground(QBrush(Qt::black));
    } else {
        tableWidgetItem->setText("-1");
        tableWidgetItem->setBackground(QBrush(Qt::black));
        tableWidgetItem->setForeground(QBrush(Qt::white));
    }
}

void MainWindow::on_inputPatternTableWidget_cellClicked(int row, int column)
{
    changeTableWidgetItemState(ui->inputPatternTableWidget, row, column);
}

void MainWindow::on_inputPatternTableWidget_cellEntered(int row, int column)
{
    changeTableWidgetItemState(ui->inputPatternTableWidget, row, column);
}

void MainWindow::on_patternsTableWidget_cellClicked(int row, int column)
{
    changeTableWidgetItemState(ui->patternsTableWidget, row, column);
}

void MainWindow::on_patternsTableWidget_cellEntered(int row, int column)
{
    changeTableWidgetItemState(ui->patternsTableWidget, row, column);
}

void MainWindow::addStepPatternToView(const std::vector<int> &pattern)
{
    int output_patterns_count = ui->outputPatternTableWidget->rowCount();
    ui->outputPatternTableWidget->insertRow(output_patterns_count);
    for (int j = 0; j < neuron_count; ++j) {
        int neuron = pattern[j];
        auto tableWidgetItem = new QTableWidgetItem(QString::number(neuron));
        tableWidgetItem->setFlags(tableWidgetItem->flags() ^ Qt::ItemIsSelectable);
        if (neuron == 1) {
            tableWidgetItem->setBackground(QBrush(Qt::white));
            tableWidgetItem->setForeground(QBrush(Qt::black));
        } else {
            tableWidgetItem->setBackground(QBrush(Qt::black));
            tableWidgetItem->setForeground(QBrush(Qt::white));
        }
        ui->outputPatternTableWidget->setItem(output_patterns_count, j, tableWidgetItem);
    }
}

void MainWindow::enableResetButton()
{
    ui->resetButton->setEnabled(true);
    ui->resetButton->setVisible(true);
}

MainWindow::FloatMatrix MainWindow::computeWeightedMatrix(const MainWindow::IntMatrix &patterns) const
{
    FloatMatrix weighted_matrix(neuron_count, vector<float>(neuron_count, 0.0));
    for (int i = 0; i < neuron_count; ++i) {
        auto& weighted_matrix_row = weighted_matrix[i];
        for (int j = 0; j < neuron_count; ++j) {
            if (i != j) {
                for (const auto& pattern : patterns) {
                    weighted_matrix_row[j] += float(pattern[i]) * pattern[j] / neuron_count;
                }
            }
        }
    }
    return weighted_matrix;
}

vector<int> MainWindow::associate_pattern(const vector<int>& pattern, AssociationStatus& association_status)
{
    auto output = vector<int>(neuron_count, 0);
    // pattern = weighted_matrix * pattern
    for (int i = 0; i < neuron_count; ++i) {
        float neuron = 0.0;
        for (int j = 0; j < neuron_count; ++j) {
            neuron += weighted_matrix[i][j] * pattern[j];
        }
        output[i] = neuron >= 0 ? 1 : -1; // Transfer function
    }
    if (output == last_output) {
        association_status = ASSOCIATED;
    } else {
        for (const auto& past_output : past_outputs) {
            if (past_output == output) {
                association_status = FAILED;
                break;
            }
        }
    }
    past_outputs.push_back(output);
    return output;
}

MainWindow::IntMatrix MainWindow::processInputPatterns() const
{
    IntMatrix patterns;
    patterns.reserve(pattern_count * 2);
    for (int i = 0; i < pattern_count; ++i) {
        vector<int> pattern;
        pattern.reserve(neuron_count * 2);
        for (int j = 0; j < neuron_count; j++) {
            auto tableWidgetItem = ui->patternsTableWidget->item(i, j);
            pattern.push_back(tableWidgetItem->text().toInt());
        }
        patterns.push_back(pattern);
    }
    return ::move(patterns);
}

void MainWindow::on_associateButton_clicked()
{
    switch (association_step) {
    case INIT:
        qDebug() << "Iniciando proceso de asociacion.";
        ui->associateButton->setText("Siguiente iteración");
        // Process input pattern
        last_output.clear();
        qDebug() << "Patron de entrada: ";
        for (int j = 0; j < neuron_count; ++j) {
            QString neuron_text = ui->inputPatternTableWidget->item(0, j)->text();
            qDebug() << "  " << neuron_text;
            last_output.push_back(neuron_text.toInt());
        }
        association_status = ITERATING;
        ui->outputPatternTableWidget->setColumnCount(neuron_count);
        addStepPatternToView(last_output);
        past_outputs.clear();
        past_outputs.push_back(last_output);
        last_output = associate_pattern(last_output, association_status);
        addStepPatternToView(last_output);
        if (association_status == ASSOCIATED) {
            ui->associateButton->setEnabled(false);
            ui->associateButton->setText("Proceso terminado");
            ui->statusBar->showMessage("Patron asociado.");
            enableResetButton();
        }
        association_step = EXECUTING;
        break;
    case EXECUTING:
        qDebug() << "Ejecutando iteracion";
        last_output = associate_pattern(last_output, association_status);
        addStepPatternToView(last_output);
        if (association_status == ASSOCIATED) {
            ui->associateButton->setEnabled(false);
            enableResetButton();
            ui->statusBar->showMessage("Patron asociado.");
            ui->associateButton->setText("Proceso terminado");
        } else if (association_status == FAILED) {
            ui->associateButton->setText("Proceso terminado");
            ui->associateButton->setEnabled(false);
            enableResetButton();
            ui->statusBar->showMessage("Patron no asociado, se encontro un ciclo.");
        }
        break;
    }
}

void MainWindow::on_resetButton_clicked()
{
    association_step = INIT;
    ui->associateButton->setText(ASSOCIATE_BUTTON_DEFAULT_STRING);
    ui->associateButton->setEnabled(true);
    ui->resetButton->setEnabled(false);
    ui->resetButton->setVisible(false);
    ui->outputPatternTableWidget->setColumnCount(0);
    ui->outputPatternTableWidget->setRowCount(0);
}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <vector>

class QTableWidget;

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
    void on_neuronCountSpinBox_valueChanged(int value);
    void on_patternCountSpinBox_valueChanged(int value);
    void on_generateWeightedMatrixButton_clicked();
    void on_patternsTableWidget_cellEntered(int row, int column);
    void on_patternsTableWidget_cellClicked(int row, int column);
    void on_associateButton_clicked();

    void on_inputPatternTableWidget_cellEntered(int row, int column);

    void on_inputPatternTableWidget_cellClicked(int row, int column);

    void on_resetButton_clicked();

private:
    using IntMatrix = std::vector<std::vector<int>>;
    using FloatMatrix = std::vector<std::vector<float>>;

    Ui::MainWindow *ui;

    int neuron_count = 2;
    int pattern_count = 2;

    bool weighted_matrix_computed = false;

    FloatMatrix weighted_matrix;
    std::vector<int> last_output;
    IntMatrix past_outputs;

    static const int TABLE_COLUMNS_SIZE = 30;
    static const QString ASSOCIATE_BUTTON_DEFAULT_STRING;

    enum AssociationStep { INIT, EXECUTING } association_step;

    void configureWindow();
    void changeTableWidgetItemState(QTableWidget *tableWidget, int row, int column);
    void addStepPatternToView(const std::vector<int>& pattern);
    void enableResetButton();

    IntMatrix processInputPatterns() const;
    FloatMatrix computeWeightedMatrix(const IntMatrix& patterns) const;

    enum AssociationStatus { ITERATING, FAILED, ASSOCIATED } association_status;

    std::vector<int> associate_pattern(const std::vector<int>& pattern, AssociationStatus& association_status);
};

#endif // MAINWINDOW_H

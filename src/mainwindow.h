#pragma once

#include <QMainWindow>
#include <QGraphicsView>
#include <string>
#include "diagramscene.h"
#include <algorithm> 
#include <qmessagebox.h>
#include <qfile.h>
#include "graph.h"

// Структура одного шага алгоритма поиска
struct SolutionPart {
    vector<int> node; // Список рассматриваемых узлов на текущем шаге алгоритма
    vector<vector<float>> dist; // Матрица расстояний между узлами
    vector<float> min_size; // Минимальное расстояние для каждого узла на текущем шаге
    vector<vector<int>> best_var; // Список оптимальных переходов
};

// Класс, управляющий интерфейсом и логикой работы программы
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr); // Конструктор класса
    ~MainWindow(); // Деструктор класса

private slots:
    void setNodeMode(); // Метод установки режима добавления узлов
    void setArrowMode(); // Метод установки режима создания связей
    void setEditMode(); // Метод установки режима редактирования
    void deleteItem(); // Метод удаления выбранного элемента
    void loadGraph(); // Метод загрузки графа из файла
    void startExecute(); // Метод начала работы выполнения алгоритма поиска оптимального пути
    void syncGraphFromScene(); // Метод обновления внутренней модели графа
    void printSolution(); // Метод сохранения результатов работы алгоритма
    void onGraphChanged(int from, int to, Arrow* arrow); // Метод обработки изменений графа (ID начального узла, ID конечного узла, указатель на стрелку)
    void updateExecuteButton(); // Метод управления доступом к кнопке "Выполнить"
    void updateSaveButton(); // Метод управления доступом к кнопке "Сохранить решение"

private:
    void setupUI(); // Метод создания графической сцены
    void createToolBar(); // Метод создания панели инструментов
    void updateNodesMovable(bool movable);  // Метод управления перемещением узлов (флаг разрешения на перемещение узлов)
    void executeGraph(); // Метод основного алгоритма поиска
    vector<vector<float>> createDistanceMatrix(const vector<GraphArrow>& arrows); // Метод формирования матрицы расстояний между узлами (вектор стрелок графа)
    // Метод прямого хода алгоритма поиска (Список шагов решения, номер текущего шага, текущий путь, словарь найденных путей, текущий узел, минимальное расстояние)
    void findSolution(vector<SolutionPart> solution, int step, vector<int>& currentPath, map<float, vector<string>>& result, int currentValue, float minDist);
    bool checkNodes(); // Метод проверки корректности узлов
    void checkDoubleArrows(); // Метод проверки наличия двойных связей

    bool hasCycle = false; // Флаг наличия цикла 
    bool hasDouble = false; // Флаг наличия двойной связи

    map<float, vector<string>> ways; // Словарь найденных путей
    vector<SolutionPart> solution; // Перечень выполненных шагов при решении задачи
    int startNode; // Номер начального узла для поиска оптимального пути
    int endNode; // Номер конечного узла для поиска оптимального пути

    Graph graph; // Объект представления графа
    DiagramScene* scene; // Графическая сцена
    QGraphicsView* view; // Компонент отображения сцены
    QToolButton* node_button; // Кнопка переключения в режим добавления узлов
    QToolButton* arrow_button; // Кнопка переключения в режим создания связей
    QToolButton* edit_button; // Кнопка переключения в режим редактирования
    QToolButton* save_button; // Кнопка сохранения результатов работы алгоритма в файл
};
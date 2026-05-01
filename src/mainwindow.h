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
    // Список рассматриваемых узлов на текущем шаге алгоритма
    vector<int> node;
    // Матрица расстояний между узлами
    vector<vector<float>> dist;
    // Минимальное расстояние для каждого узла на текущем шаге
    vector<float> min_size;
    // Список оптимальных переходов
    vector<vector<int>> best_var;
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
    // Метод начала работы выполнения алгоритма поиска оптимального пути
    void startExecute();
    void syncGraphFromScene(); // Метод обновления внутренней модели графа
    // Метод сохранения результатов работы алгоритма
    void printSolution();
    // Метод обработки изменений графа
    void onGraphChanged(int from, int to, Arrow* arrow);
    // Метод управления доступом к кнопке "Выполнить"
    void updateExecuteButton();
    // Метод управления доступом к кнопке "Сохранить решение"
    void updateSaveButton();

private:
    void setupUI(); // Метод создания графической сцены
    void createToolBar(); // Метод создания панели иснтрументов
    // Метод управления перемещением узлов
    void updateNodesMovable(bool movable);
    void executeGraph(); // Метод основного алгоритма поиска
    // Метод формирования матрицы расстояний между узлами
    vector<vector<float>> createDistanceMatrix(const vector<GraphArrow>& arrows);
    // Метод прямого хода алгоритма поиска
    void findSolution(vector<SolutionPart> solution, int step, vector<int>& currentPath, map<float, vector<string>>& result, int currentValue, float minDist);
    // Метод проверки корректности узлов
    bool checkNodes();
    // Метод проверки наличия двойных связей
    void checkDoubleArrows();

    // Информация о наличии циклов
    bool hasCycle = false;
    // Информация о наличии двойных связей
    bool hasDouble = false;

    map<float, vector<string>> ways; // Словарь найденных путей
    // Перечень выполненных шагов при решении задачи
    vector<SolutionPart> solution;

    Graph graph; // Объект представления графа
    DiagramScene* scene; // Графическая сцена
    QGraphicsView* view; // Компонент отображения сцены
    // Кнопка переключения в режим добавления узлов
    QToolButton* node_button;
    // Кнопка переключения в режим создания связей
    QToolButton* arrow_button;
    // Кнопка переключения в режим редактирования
    QToolButton* edit_button;
    // Кнопка сохранения результатов работы алгоритма в файл
    QToolButton* save_button;
    // Номер начального узла для поиска оптимального пути
    int startNode;
    // Номер конечного узла для поиска оптимального пути
    int endNode;
};
#pragma once

// Отключаем Windows SDK conflict
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

// Отключаем std::byte
#define _HAS_STD_BYTE 0

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <map>
#include <queue>
#include <cctype>
#include <QString>
#include <QStringList>

using namespace std; 

// Структура одного узла графа
struct GraphNode {
    int nodeID; // Порядковый номер узла
    QVector<int> prev_nodesID; //Список узлов, из которых можно попасть в данный узел
    int K; // Уровень узла (пояс)
    pair<int, int> coordinates; // Координаты узла на сцене
    bool inCycle; // Флаг принадлежности к циклу
};

// Структура одной связи между двумя узлами графа
struct GraphArrow {
    int node_1; // Номер начального узла
    int node_2; // Номер конечного узла
    string weight; // Вес связи
    bool isLoop; // Флаг некорректности связи
};

// Класс, хранящий структуру графа
class Graph {
private:
    vector<GraphNode> nodes; // Список узлов графа
    vector<vector<int>> adj_matrix; // Матрица смежности графа
    vector<GraphArrow> arrows; // Список связей графа
    map <int, vector<int>> zones; // Список всех узлов, разбитых по поясам

    // Метод рекурсивного обхода графа для поиска циклов (текущий узел, массив статусов узлов на посещение, массив родителя для восстановления пути, вектор найденных связей)
    void dfsCycle(int v, vector<int>& visited, vector<int>& parent, vector<pair<int, int>>& cycleArrows) const;
    // Метод проверки графа на ацикличность (текущий узел, массив статусов узлов на посещение, флаг наличия цикла при обходе)
    void dfsDAG(int v, vector<int>& visited, bool& hasCycle) const;

public:
    void loadMatrix(const string& filename); // Метод загрузки матрицы смежности из файла (название файла)
    void convertToArrows(); // Метод создания связей из матрицы смежности
    void findZone(); // Метод разбиения узлов на пояса

    vector<pair<int, pair<int, int>>> getNodesData() const;  // Метод получения списка узлов графа
    vector<GraphArrow> getArrowsData() const { return arrows; } // Метод получения списка связей графа
    void rebuildFromArrows(const vector<GraphArrow>& newArrows); // Метод перестройки матрицы смежности (список стрелок)

    bool isDAG() const; // Метод проверки на ацикличность
    vector<pair<int, int>> findCycleArrows() const; // Метод нахождения всех связей, образующих цикл
};


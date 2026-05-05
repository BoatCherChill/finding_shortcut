
#include "graph.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

// Метод загрузки матрицы смежности из файла (название файла)
void Graph::loadMatrix(const string& filename) {
    ifstream file(filename); // Открыть файл для чтения
    string line; // Буфер для текущей строки
    adj_matrix.clear(); 

    while (getline(file, line)) {
        istringstream iss(line); // Создать строковый поток для разбора строки
        vector<int> row;
        int num;

        // Считать все числа из строки
        while (iss >> num) {
            row.push_back(num);
        }
        adj_matrix.push_back(row); // Добавить строку в матрицу
    }

    if (!adj_matrix.empty()) { // Проверить, что матрица квадратная (N x N)
        int size = adj_matrix.size();
        for (const auto& row : adj_matrix) {
            if (row.size() != size) { // Если строка имеет другую длину
                adj_matrix.clear(); // Очистить матрицу (ошибка)
                return;
            }
        }
    }

    if (adj_matrix.empty()) { // Если матрица пустая (файл пуст или ошибка)
        return;
    }

    convertToArrows(); // Преобразовать матрицу в список связей
    findZone(); // Расставить узлы по поясам
}

// Метод создания связей из матрицы смежности
void Graph::convertToArrows() {
    arrows.clear(); // Очистить предыдущий список связей
    int size = adj_matrix.size(); // Размер матрицы (количество узлов)

    for (int i = 0; i < size; i++) { // Перебирать все возможные пары узлов (i -> j)
        for (int j = 0; j < size; j++) {
            if (adj_matrix[i][j] > 0 && i != j) { // Если есть ребро и это не петля
                bool found = false; // Флаг, найдена ли обратная связь

                for (auto& visited : arrows) { // Проверить, существует ли обратная связь
                    if (visited.node_1 == j && visited.node_2 == i) {
                        visited.weight = visited.weight + "," + to_string(adj_matrix[i][j]);
                        visited.isLoop = true; // Поднять флаг двойной связи
                        found = true;
                        break;
                    }
                }

                if (!found) { // Если обратной связи нет, создать новую
                    GraphArrow newArrow;
                    newArrow.node_1 = i;
                    newArrow.node_2 = j;
                    newArrow.weight = to_string(adj_matrix[i][j]);
                    newArrow.isLoop = false;
                    arrows.push_back(newArrow);
                }
            }
        }
    }
}

// Метод разбиения узлов на пояса
void Graph::findZone() {
    int size = adj_matrix.size();
    if (size == 0) return;

    int lastNode = size - 1; // Последний узел
    nodes.clear(); // Очистить предыдущий список узлов
    zones.clear(); // Очистить предыдущие зоны

    for (int i = 0; i < size; i++) { // Заполнить список предков для каждого узла
        GraphNode node;
        node.nodeID = i;
        node.K = -1; // Уровень не вычислен
        node.prev_nodesID.clear(); // Очистить список предков

        for (int j = 0; j < size; j++) { // Найти все узлы, из которых есть ребро в текущий
            if (adj_matrix[j][i] > 0) {
                node.prev_nodesID.push_back(j);
            }
        }
        nodes.push_back(node);
    }

    queue<int> q;
    nodes[lastNode].K = 0; // Последний узел имеет уровень 0
    q.push(lastNode);

    while (!q.empty()) { // Вычислить расстояние до последнего узла для каждого (уровень K)
        int current = q.front();
        q.pop();

        for (int parent : nodes[current].prev_nodesID) { 
            if (nodes[parent].K == -1) { // Если уровень еще не вычислен
                nodes[parent].K = nodes[current].K + 1; // Уровень = уровень потомка + 1
                q.push(parent);
            }
        }
    }

    for (const auto& node : nodes) { // Сгруппировать узлы по уровням 
        zones[node.K].push_back(node.nodeID);
    }

    int maxK = -1; // Найти максимальный уровень 
    for (const auto& pair : zones) {
        int dist = pair.first;
        if (dist > maxK)
            maxK = dist;
    }

    int start_x = -250; // Начальная X координата (левый край)
    int start_y = 0; // Начальная Y координата (центр)
    int offset = 150; // Шаг между узлами

    for (const auto& [zone, node_neighbours] : zones) { // Расставить координаты для каждого узла
        int col = maxK - zone; // Колонка (X координата)
        int x = start_x + col * offset;

        int totalNodes = node_neighbours.size(); // Количество узлов в зоне
        int startY = start_y - (totalNodes - 1) * offset / 2; // Центрировать по вертикали

        for (size_t i = 0; i < node_neighbours.size(); i++) {
            int nodeId = node_neighbours[i];
            int y = startY + i * offset; // Y координата
            nodes[nodeId].coordinates = { x, y }; // Сохранить координаты узла
        }
    }
}

// Метод получения списка узлов графа
vector<pair<int, pair<int, int>>> Graph::getNodesData() const {
    vector<pair<int, pair<int, int>>> result;
    for (const auto& node : nodes) {
        result.push_back({ node.nodeID, node.coordinates });
    }
    return result;
}

// Метод перестройки матрицы смежности (список стрелок)
void Graph::rebuildFromArrows(const vector<GraphArrow>& newArrows) {
    arrows = newArrows;
    // Найти максимальный ID узла
    int maxNode = 0;
    for (const auto& a : arrows) {
        maxNode = max(maxNode, max(a.node_1, a.node_2));
    }

    adj_matrix.assign(maxNode + 1, vector<int>(maxNode + 1, 0)); // Создать пустую матрицу

    // Заполнить матрицу весами из списка связей
    for (const auto& a : arrows) {
        adj_matrix[a.node_1][a.node_2] = stoi(a.weight);
    }
}

// Метод проверки графа на ацикличность (текущий узел, массив статусов узлов на посещение, флаг наличия цикла при обходе)
void Graph::dfsDAG(int v, vector<int>& visited, bool& hasCycle) const {
    visited[v] = 1;  // Отметить узел как находящийся в стеке рекурсии
    int n = adj_matrix.size();

    for (int u = 0; u < n; u++) { // Обойти всех соседей текущего узла
        if (adj_matrix[v][u] > 0) { // Если есть ребро v -> u
            if (visited[u] == 0) { // Узел не посещен
                dfsDAG(u, visited, hasCycle);
            }
            else if (visited[u] == 1) { // Узел уже в стеке -> найден цикл
                hasCycle = true;
                return;
            }
        }
    }
    visited[v] = 2; // Отметить узел как обработанный
}

// Метод проверки на ацикличность
bool Graph::isDAG() const {
    int n = adj_matrix.size();
    if (n == 0) 
        return true; // Отметить пустой граф ациклическим

    vector<int> visited(n, 0); // 0 - не посещен, 1 - в стеке, 2 - обработан
    bool hasCycle = false;

    for (int i = 0; i < n; i++) { // Запустить обход для каждой непосещенной вершины
        if (visited[i] == 0) {
            dfsDAG(i, visited, hasCycle);
            if (hasCycle) return false; // Найден цикл
        }
    }
    return true; // Циклов нет
}

// Метод рекурсивного обхода графа для поиска циклов
void Graph::dfsCycle(int v, vector<int>& visited, vector<int>& parent, vector<pair<int, int>>& cycleArrows) const {
    visited[v] = 1; // Отметить узел как находящийся в стеке рекурсии
    int n = adj_matrix.size();

    for (int u = 0; u < n; u++) { // Обойти всех соседей текущего узла
        if (adj_matrix[v][u] > 0) { // Если есть ребро v -> u
            if (visited[u] == 0) { // Узел не посещен
                parent[u] = v; // Сохранить родителя
                dfsCycle(u, visited, parent, cycleArrows);
            }
            else if (visited[u] == 1) { // Найден цикл (узел u уже в стеке)
                int current = v;
                while (current != u && current != -1) { // Восстановить все ребра цикла
                    if (parent[current] != -1) {
                        cycleArrows.push_back({ parent[current], current });
                    }
                    current = parent[current];
                }
                cycleArrows.push_back({ v, u }); // Добавить последнее ребро цикла
            }
        }
    }
    visited[v] = 2; // Отметить узел как обработанный
}

// Метод нахождения всех связей, образующих цикл
vector<pair<int, int>> Graph::findCycleArrows() const {
    int n = adj_matrix.size();
    if (n == 0) return {};

    vector<int> visited(n, 0);
    vector<int> parent(n, -1);
    vector<pair<int, int>> cycleArrows;

    // Запустить обход для каждой вершины
    for (int i = 0; i < n; i++) { 
        vector<int> tempVisited(n, 0); // Временные массивы для обхода от текущей вершины
        vector<int> tempParent(n, -1);

        dfsCycle(i, tempVisited, tempParent, cycleArrows);
    }

    sort(cycleArrows.begin(), cycleArrows.end()); // Удалить дубликаты
    cycleArrows.erase(unique(cycleArrows.begin(), cycleArrows.end()), cycleArrows.end());

    return cycleArrows;
}

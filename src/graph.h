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

struct GraphNode {
    int nodeID;
    QVector<int> prev_nodesID;
    int K;
    pair<int, int> coordinates;
};

struct GraphArrow {
    int node_1;
    int node_2;
    string weight;
    bool isLoop;
};

class Graph {
private:
    vector<GraphNode> nodes;
    vector<vector<int>> adj_matrix;
    vector<GraphArrow> arrows;
    map <int, vector<int>> zones;
    //vector<vector<int>> adjacencyMatrix();
    //bool troubles();

public:
    //void execute();
    void loadMatrix(const string& filename);
    void convertToArrows();
    void findZone();

    vector<pair<int, pair<int, int>>> getNodesData() const;
    vector<GraphArrow> getArrowsData() const;

    int startNode;
    int endNode;

    //Node* findNodeById(int id); // функция поиска узла по ID
    //void addNode(int x, int y, NodeType type); // функция добавления узла
    //void deleteNode(int id); // функция удаления узла
    //void addRelation(int fromId, int toId); // функция добавления связи между узлами
    //void deleteRelation(int fromId, int toId); // функция удаления связи между узлами


};


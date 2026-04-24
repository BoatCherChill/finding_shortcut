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
    bool inCycle;
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
    void dfsCycle(int v, vector<int>& visited, vector<int>& parent, vector<pair<int, int>>& cycleArrows) const;
    void dfsDAG(int v, vector<int>& visited, bool& hasCycle) const;

public:
    void loadMatrix(const string& filename);
    void convertToArrows();
    void findZone();

    vector<pair<int, pair<int, int>>> getNodesData() const;
    vector<GraphArrow> getArrowsData() const;
    void rebuildFromArrows(const vector<GraphArrow>& newArrows);

    bool isDAG() const;
    vector<pair<int, int>> findCycleArrows() const;



};



#include "graph.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

void Graph::loadMatrix(const string& filename) {
    ifstream file(filename);
    string line;
    adj_matrix.clear();

    while (getline(file, line)) {
        istringstream iss(line);
        vector<int> row;
        int num;

        while (iss >> num) {
            row.push_back(num);
        }
        adj_matrix.push_back(row);
    }

    convertToArrows();
    findZone();
}

void Graph::convertToArrows() {
    arrows.clear();
    int size = adj_matrix.size();

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (adj_matrix[i][j] > 0) {
                bool found = false;

                for (auto& visited : arrows) {
                    if (visited.node_1 == j && visited.node_2 == i) {
                        visited.weight = visited.weight + "," + to_string(adj_matrix[i][j]);
                        visited.isLoop = true;
                        found = true;
                        break;
                    }
                }

                if (!found) {
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

void Graph::findZone() {
    int size = adj_matrix.size();
    if (size == 0) return;

    int lastNode = size - 1;
    nodes.clear();
    zones.clear();

    // заполнить предков
    for (int i = 0; i < size; i++) {
        GraphNode node;
        node.nodeID = i;
        node.K = -1;
        node.prev_nodesID.clear();

        for (int j = 0; j < size; j++) {
            if (adj_matrix[j][i] > 0) {
                node.prev_nodesID.push_back(j);
            }
        }
        nodes.push_back(node);
    }

    // посчитать расстояние до последнего узла для каждого узла (К)
    queue<int> q;
    nodes[lastNode].K = 0;
    q.push(lastNode);

    while (!q.empty()) {
        int current = q.front();
        q.pop();

        for (int parent : nodes[current].prev_nodesID) {
            if (nodes[parent].K == -1) {
                nodes[parent].K = nodes[current].K + 1;
                q.push(parent);
            }
        }
    }

    for (const auto& node : nodes) {
        zones[node.K].push_back(node.nodeID);
    }

    int maxK = -1;
    for (const auto& pair : zones) {
        int dist = pair.first;
        if (dist > maxK) 
            maxK = dist;
    }

    int start_x = -250;
    int start_y = 0; 
    int offset = 150;

    for (const auto& [zone, node_neighbours] : zones) {
        int col = maxK - zone; 
        int x = start_x + col * offset;

        int totalNodes = node_neighbours.size();
        int startY = start_y - (totalNodes - 1) * offset / 2;

        for (size_t i = 0; i < node_neighbours.size(); i++) {
            int nodeId = node_neighbours[i];
            int y = startY + i * offset;
            nodes[nodeId].coordinates = { x, y };
        }
    }
}


vector<pair<int, pair<int, int>>> Graph::getNodesData() const {
    vector<pair<int, pair<int, int>>> result;
    for (const auto& node : nodes) {
        result.push_back({ node.nodeID, node.coordinates });
    }
    return result;
}

vector<GraphArrow> Graph::getArrowsData() const {
    return arrows;
}


void Graph::rebuildFromArrows(const vector<GraphArrow>& newArrows) {
    arrows = newArrows;

    int maxNode = 0;
    for (const auto& a : arrows) {
        maxNode = max(maxNode, max(a.node_1, a.node_2));
    }

    adj_matrix.assign(maxNode + 1, vector<int>(maxNode + 1, 0));
    for (const auto& a : arrows) {
        adj_matrix[a.node_1][a.node_2] = stoi(a.weight);
    }
}

bool Graph::isDAG() const {
    int n = adj_matrix.size();
    if (n == 0) return true;

    vector<int> visited(n, 0);

    function<bool(int)> dfs = [&](int v) {
        visited[v] = 1;
        for (int u = 0; u < n; u++) {
            if (adj_matrix[v][u] > 0) {
                if (visited[u] == 1) return true;
                if (visited[u] == 0 && dfs(u)) return true;
            }
        }
        visited[v] = 2;
        return false;
        };

    for (int i = 0; i < n; i++) {
        if (visited[i] == 0) {
            if (dfs(i)) return false;
        }
    }
    return true;
}

vector<pair<int, int>> Graph::findCycleArrows() const {
    int n = adj_matrix.size();
    if (n == 0) return {};

    vector<int> visited(n, 0);
    vector<int> parent(n, -1);
    vector<pair<int, int>> cycleArrows;

    function<void(int)> dfs = [&](int v) {
        visited[v] = 1;
        for (int u = 0; u < n; u++) {
            if (adj_matrix[v][u] > 0) {
                if (visited[u] == 0) {
                    parent[u] = v;
                    dfs(u);
                }
                else if (visited[u] == 1) {
                    cycleArrows.push_back({ v, u });
                    int current = v;
                    while (current != u && current != -1) {
                        if (parent[current] != -1) {
                            cycleArrows.push_back({ parent[current], current });
                        }
                        current = parent[current];
                    }
                }
            }
        }
        visited[v] = 2;
        };

    for (int i = 0; i < n; i++) {
        if (visited[i] == 0) {
            dfs(i);
        }
    }

    return cycleArrows;
}
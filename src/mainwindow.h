#pragma once

#include <QMainWindow>
#include <QGraphicsView>
#include <string>
#include "diagramscene.h"
#include <algorithm> 
#include <qmessagebox.h>
#include <qfile.h>
#include "graph.h"

struct SolutionPart {
    vector<int> node;
    vector<vector<float>> dist;
    vector<float> min_size;
    vector<vector<int>> best_var;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void setNodeMode();
    void setArrowMode();
    void setEditMode();
    void deleteItem();
    void loadGraph();
    void startExecute();
    void syncGraphFromScene();

    void printSolution();

    void onGraphChanged(int from, int to, Arrow* arrow);
    void updateExecuteButton();
    void updateSaveButton();

private:
    void setupUI();
    void createToolBar(); 
    void updateNodesMovable(bool movable);
    void executeGraph();
    vector<vector<float>> createDistanceMatrix(const vector<GraphArrow>& arrows);
    void findSolution(vector<SolutionPart> solution, int step, vector<int>& currentPath, map<float, vector<string>>& result, int currentValue, float minDist);

    bool checkNodes();
    void checkDoubleArrows();

    bool hasCycle = false;
    bool hasDouble = false;

    map<float, vector<string>> ways;
    vector<SolutionPart> solution;

    Graph graph;
    DiagramScene* scene;
    QGraphicsView* view;
    QToolButton* node_button;
    QToolButton* arrow_button;
    QToolButton* edit_button;
    QToolButton* save_button;

    int startNode;
    int endNode;
};
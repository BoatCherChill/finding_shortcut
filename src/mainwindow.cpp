#include "mainwindow.h"
#include "diagramscene.h"
#include "graph.h"
#include <QtWidgets>
#include <QToolButton>
#include <QButtonGroup>
#include <QMessageBox>
#include <QGraphicsEllipseItem>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUI();
    createToolBar();
    setWindowTitle("Графический редактор графов");
    resize(1000, 700);
}

MainWindow::~MainWindow() {
}

void MainWindow::setupUI() {
    QMenu* itemMenu = new QMenu(this);
    scene = new DiagramScene(itemMenu, this);
    scene->setMode(DiagramScene::InsertNode);

    view = new QGraphicsView(scene, this);
    view->setRenderHint(QPainter::Antialiasing);
    view->setDragMode(QGraphicsView::NoDrag);
    view->setContextMenuPolicy(Qt::CustomContextMenu);

    setCentralWidget(view);
}

void MainWindow::createToolBar() {
    QToolBar* bar = addToolBar("Инструменты");
    bar->setMovable(false);

    QAction* deleteAction = bar->addAction("Удалить");
    QObject::connect(deleteAction, &QAction::triggered, this, &MainWindow::deleteItem);
    bar->addSeparator();

    QButtonGroup* modeGroup = new QButtonGroup(this);
    modeGroup->setExclusive(true);

    node_button = new QToolButton;
    node_button->setText("Узлы");
    node_button->setCheckable(true);
    node_button->setChecked(true);

    arrow_button = new QToolButton;
    arrow_button->setText("Связи");
    arrow_button->setCheckable(true);

    edit_button = new QToolButton;
    edit_button->setText("Редактирование");
    edit_button->setCheckable(true);

    modeGroup->addButton(node_button, 0);
    modeGroup->addButton(arrow_button, 1);
    modeGroup->addButton(edit_button, 2);

    bar->addWidget(node_button);
    bar->addWidget(arrow_button);
    bar->addWidget(edit_button);

    QObject::connect(node_button, &QToolButton::clicked, [this]() {
        setNodeMode();
        view->setCursor(Qt::ArrowCursor);
        });

    QObject::connect(arrow_button, &QToolButton::clicked, [this]() {
        scene->setMode(DiagramScene::InsertArrow);
        setArrowMode();
        view->setCursor(Qt::CrossCursor);
        });

    QObject::connect(edit_button, &QToolButton::clicked, [this]() {
        scene->setMode(DiagramScene::EditItems);
        setEditMode();
        view->setCursor(Qt::ArrowCursor);
        });

    bar->addSeparator();

    QPushButton* loadBtn = new QPushButton("Загрузить матрицу");
    loadBtn->setObjectName("loadButton");
    bar->addWidget(loadBtn);
    QObject::connect(loadBtn, &QPushButton::clicked, this, &MainWindow::loadGraph);

    bar->addSeparator();

    QPushButton* execBtn = new QPushButton("Выполнить");
    execBtn->setObjectName("executeButton");
    execBtn->setEnabled(false);
    bar->addWidget(execBtn);
    QObject::connect(execBtn, &QPushButton::clicked, this, &MainWindow::executeGraph);
    execBtn->setEnabled(true);

    bar->addSeparator();
}

void MainWindow::setNodeMode() {
    scene->setMode(DiagramScene::InsertNode);
    view->setCursor(Qt::ArrowCursor);
    updateNodesMovable(true);
    scene->clearSelection();
}

void MainWindow::setArrowMode() {
    scene->setMode(DiagramScene::InsertArrow);
    view->setCursor(Qt::CrossCursor);
    updateNodesMovable(false);
    scene->clearSelection();
}

void MainWindow::setEditMode() {
    scene->setMode(DiagramScene::EditItems);
    view->setCursor(Qt::ArrowCursor);
    updateNodesMovable(true);
    scene->clearSelection();
}


void MainWindow::updateNodesMovable(bool movable) {
    for (QGraphicsItem* item : scene->items()) {
        if (QGraphicsEllipseItem* node = qgraphicsitem_cast<QGraphicsEllipseItem*>(item)) {
            node->setFlag(QGraphicsItem::ItemIsMovable, movable);
        }
    }
}

void MainWindow::deleteItem() {
    if (!scene) return;
    scene->deleteSelectedItem();
}

void MainWindow::loadGraph() {
    QString fileName = QFileDialog::getOpenFileName(this, "Открыть файл", "", "Text files (*.txt)");
    if (fileName.isEmpty()) return;
    graph.loadMatrix(fileName.toStdString());

    auto nodesData = graph.getNodesData();
    auto arrowsData = graph.getArrowsData();

    scene->loadGraph(nodesData, arrowsData);

    scene->setMode(DiagramScene::EditItems);
    edit_button->setChecked(true);
    updateNodesMovable(true);

}

void MainWindow::executeGraph(){

    int lastnode = 10;
    int firstnode = 1;
    vector<GraphArrow> arrows;
    arrows = graph.getArrowsData();
    map<string, float> ways;

    vector<vector<float>> weights = createDistanceMatrix(arrows);

    vector<SolutionPart> solution;
    int countLoop = 0;
    while (true) {
        if (solution.size() != 0) {
            vector<int> nodes = solution[solution.size() - 1].node;
            bool flagOfExit = false;
            if (std::find(nodes.begin(), nodes.end(), firstnode) != nodes.end()) {
                int position = std::distance(nodes.begin(),
                    std::find(nodes.begin(), nodes.end(), firstnode));
                vector<int> path;
                float a = solution[solution.size() - 1].min_size[position];
                findSolution(solution, solution.size() - 1, path, ways, firstnode, solution[solution.size()-1].min_size[position]);
            }
            if (nodes.size() == 0) break;
        }
        SolutionPart step;
        if (countLoop == 0)
        {
            countLoop++;
            vector<float> node;
            node.push_back(lastnode);
            step.dist.push_back(node);
            for (int i = 0; i < weights.size(); i++) {
                if (weights[i][lastnode - 1] != 0) {
                    step.node.push_back(i + 1);
                    vector<float> d;
                    d.push_back(weights[i][lastnode - 1]);
                    step.dist.push_back(d);
                    step.min_size.push_back(weights[i][lastnode - 1]);
                    vector<int> best;
                    best.push_back(lastnode);
                    step.best_var.push_back(best);
                }
            }
        }
        else {
            int count = 0;
            vector<float> temp;
            step.dist.push_back(temp);
            for (int i = 0; i < solution[solution.size() - 1].node.size(); i++) {
                step.dist[0].push_back(solution[solution.size() - 1].node[i]);
                int t = weights.size();
                for (int j = 0; j < t; j++) {
                    if (weights[j][solution[solution.size() - 1].node[i] - 1] != 0) {
                        float weight = weights[j][solution[solution.size() - 1].node[i] - 1];
                        if (std::find(step.node.begin(), step.node.end(), j + 1) == step.node.end()) {
                            step.node.push_back(j + 1);
                            vector<float> newWeights;
                            for (int k = 0; k < step.dist[0].size() - 1; k++) newWeights.push_back(1000000);
                            newWeights.push_back(weight + solution[solution.size() - 1].min_size[i]);
                            step.dist.push_back(newWeights);
                            if (count < newWeights.size()) count = newWeights.size();
                        }
                        else {
                            int position = std::distance(step.node.begin(),
                                std::find(step.node.begin(), step.node.end(), j + 1)) + 1;
                            for (int k = step.dist[position].size(); k < count; k++) step.dist[position].push_back(1000000);
                            step.dist[position].push_back(weight + solution[solution.size() - 1].min_size[i]);
                            if (count < step.dist[position].size()) count = step.dist[position].size();
                        }
                    }
                }
                for (int j = 0; j < step.node.size(); j++) {
                    if (step.dist[j + 1].size() < count) 
                        for (int k = step.dist[j + 1].size(); k < count; k++) 
                            step.dist[j + 1].push_back(1000000);
                }
            }
            for (int j = 0; j < step.node.size(); j++) {
                auto it = std::min_element(step.dist[j+1].begin(), step.dist[j + 1].end());
                float minValue = *it;
                step.min_size.push_back(minValue);
                vector<int> best;
                for (int k = 0; k < step.dist[0].size(); k++) {
                    if (step.dist[j + 1][k] == minValue) {
                        best.push_back(step.dist[0][k]);
                    }
                }
                step.best_var.push_back(best);
            }

        }
        solution.push_back(step);
        printSolution(solution);
        cout << "--------------" << endl;
    }

    
    /*vector<vector<int>> belts;

    belts = getBelt(arrows);*/
}

vector<vector<float>> MainWindow::createDistanceMatrix(const vector<GraphArrow>& arrows) {
    if (arrows.empty()) {
        return {};
    }

    int maxNode = 0;
    for (const auto& arrow : arrows) {
        maxNode = max(maxNode, max(arrow.node_1, arrow.node_2));
    }

    vector<vector<float>> matrix(maxNode + 1, vector<float>(maxNode + 1, 0.0f));

    for (const auto& arrow : arrows) {
        /*if (arrow.isLoop) {
            continue;
        }*/

        float weightValue = 0.0f;
        try {
            weightValue = stof(arrow.weight);
        }
        catch (const exception& e) {
            continue;
        }

        matrix[arrow.node_1][arrow.node_2] = weightValue;
    }

    return matrix;
}

//vector<vector<int>> MainWindow::getBelt(vector<GraphArrow> arrows) {
//    int count = 1;
//    int countNextLoop = 0;
//
//    for (int count = 0; )
//}

void MainWindow::findSolution(vector<SolutionPart>& solution, int step, vector<int>& currentPath, map<string, float>& result, int currentValue, float minDist) {

    currentPath.push_back(currentValue);

    if (step == 0) {

        auto it = std::find(solution[step].node.begin(), solution[step].node.end(), currentValue);
        if (it != solution[step].node.end()) {
            int position = std::distance(solution[step].node.begin(), it);

            if (!solution[step].best_var[position].empty()) {
                int previousValue = solution[step].best_var[position][0];
                
                string pathStr = "";
                for (int i = 0; i < currentPath.size(); i++) {
                    if (i == 0) pathStr += to_string(currentPath[i]);
                    else pathStr += "-" + to_string(currentPath[i]);
                }
                pathStr += "-" + to_string(previousValue);
                result[pathStr] = minDist;
            }
        }
        currentPath.pop_back();
        return;
    }

    auto it = std::find(solution[step].node.begin(), solution[step].node.end(), currentValue);
    if (it == solution[step].node.end()) {
        currentPath.pop_back();
        return;
    }

    int position = distance(solution[step].node.begin(), it);

    vector<int> nextValues = solution[step].best_var[position];

    for (int nextValue : nextValues) {
        findSolution(solution, step - 1, currentPath, result, nextValue, minDist);
    }
    currentPath.pop_back();
}

void MainWindow::printSolution(vector<SolutionPart> s) {
    for (SolutionPart i : s) {
        int sizenode =  i.node.size();
        for (int j = -1; j < sizenode; j++) {
            if (j == -1) {
                cout << "  ";
                for (int k = 0; k < i.dist[0].size(); k++) {
                    cout << i.dist[0][k] << "  ";
                }
            }
            else {
                cout << i.node[j] << "  ";
                for (int k = 0; k < i.dist[j].size(); k++) {
                    cout << i.dist[j + 1][k] << "  ";
                }
                cout << i.min_size[j] << "  ";
                for (int k = 0; k < i.best_var[j].size(); k++) {
                    cout << i.best_var[j][k] << "  ";
                }
            }
            cout << endl;
        }
        cout << endl;
    }
}


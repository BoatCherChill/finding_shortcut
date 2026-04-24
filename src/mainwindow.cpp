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

    connect(scene, &DiagramScene::checkCycle, this, &MainWindow::onCheckCycle);
    connect(scene, &DiagramScene::graphChanged, this, &MainWindow::syncGraphFromScene);

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
   // execBtn->setEnabled(false);
    bar->addWidget(execBtn);
    QObject::connect(execBtn, &QPushButton::clicked, this, &MainWindow::startExecute);
    execBtn->setEnabled(true);

    bar->addSeparator();

    QPushButton* saveBtn = new QPushButton("Сохранить решение");
    saveBtn->setObjectName("saaveButton");
    bar->addWidget(saveBtn);
    QObject::connect(saveBtn, &QPushButton::clicked, this, &MainWindow::printSolution);

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

    syncGraphFromScene();
    onCheckCycle(0, 0, nullptr);

    scene->setMode(DiagramScene::EditItems);
    edit_button->setChecked(true);
    updateNodesMovable(true);

}

void MainWindow::executeGraph(){

    if (startNode == endNode) {
        QString message = QString("Начальный и конечный узлы пути не могут совпадать.");

        QMessageBox::warning(this, "Ошибка", message);
        return;
    }

    vector<GraphArrow> arrows;
    arrows = graph.getArrowsData();
    ways.clear();
    solution.clear();

    vector<vector<float>> weights = createDistanceMatrix(arrows);

    
    int countLoop = 0;
    while (true) {
        if (solution.size() != 0) {
            vector<int> nodes = solution[solution.size() - 1].node;
            bool flagOfExit = false;
            if (std::find(nodes.begin(), nodes.end(), startNode) != nodes.end()) {
                int position = std::distance(nodes.begin(),
                    std::find(nodes.begin(), nodes.end(), startNode));
                vector<int> path;
                float a = solution[solution.size() - 1].min_size[position];
                findSolution(solution, solution.size() - 1, path, ways, startNode, solution[solution.size()-1].min_size[position]);
                
            }
            if (nodes.size() == 0) break;
        }
        SolutionPart step;
        if (countLoop == 0)
        {
            countLoop++;
            vector<float> node;
            node.push_back(endNode);
            step.dist.push_back(node);
            for (int i = 0; i < weights.size(); i++) {
                if (weights[i][endNode - 1] != 0) {
                    step.node.push_back(i + 1);
                    vector<float> d;
                    d.push_back(weights[i][endNode - 1]);
                    step.dist.push_back(d);
                    step.min_size.push_back(weights[i][endNode - 1]);
                    vector<int> best;
                    best.push_back(endNode);
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
        
    }
    /*vector<vector<int>> belts;

    belts = getBelt(arrows);*/
    //printSolution(this);
    if (ways.size() == 0) {
        QString message = QString("Путь из пункта '%1' в пункт '%2' не найден.\n\n"
            "Возможные причины:\n"
            "• Отсутствует соединение между пунктами\n"
            "• Один из пунктов не существует\n"
            "• Нет доступных маршрутов")
            .arg(startNode)
            .arg(endNode);

        QMessageBox::information(this, "Путь не найден", message);
    }
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

void MainWindow::findSolution(vector<SolutionPart> solution, int step, vector<int>& currentPath, map<float, vector<string>>& result, int currentValue, float minDist) {

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
                result[minDist].push_back(pathStr);
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

void MainWindow::startExecute() {


    QDialog dialog(this);
    dialog.setWindowTitle("Поиск кратчайшего пути");
    dialog.setFixedSize(300, 150);

    QFormLayout* layout = new QFormLayout(&dialog);

    QLineEdit* start = new QLineEdit();
    QLineEdit* end = new QLineEdit();

    layout->addRow("Начальный узел:", start);
    layout->addRow("Конечный узел:", end);

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok);

    layout->addRow(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);

    if (dialog.exec() == QDialog::Accepted) {
        startNode = start->text().toInt();
        endNode = end->text().toInt();
        

    }

    syncGraphFromScene();
    executeGraph();

    QStringList allPaths;
    for (const auto& [weight, paths] : ways) {
        for (const auto& path : paths) {
            allPaths << QString::fromStdString(path);
        }
    }
    scene->drawWays(allPaths);
    

}

void MainWindow::printSolution() {
    if (solution.size() == 0) {
        QString message = QString("Необходимо сначала выполнить расчет кратчайшего пути для графа.");

        QMessageBox::warning(this, "Путь не найден", message);
        return;
    }


    QString filePath = QFileDialog::getSaveFileName(
        this,
        "Сохранить файл с результатами",          // Заголовок окна
        QDir::homePath(),                    // Начальная директория (домашняя папка)
        "Текстовые файлы (*.txt);" // Фильтры типов файлов
    );

    // Проверяем, не нажал ли пользователь "Отмена"
    if (filePath.isEmpty()) {
        return;
    }

    // Добавляем расширение .txt, если его нет
    if (!filePath.endsWith(".txt", Qt::CaseInsensitive)) {
        filePath += ".txt";
    }

    // Создаём файл и записываем в него строку
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка",
            "Не удалось создать или открыть файл для записи:\n" + filePath);
        return;
    }

    QTextStream out(&file);

    const int FIELD_WIDTH = 15;
    const int PRECISION = 3;

    out.setRealNumberNotation(QTextStream::FixedNotation);
    out.setRealNumberPrecision(PRECISION);

    int count = 0;
    for (SolutionPart i : solution) {
        count++;
        if (count == solution.size()) break;
        for (int k = 0; k < (FIELD_WIDTH * (i.dist[0].size() + 3) + (i.dist[0].size() + 4)); k++) out << "-";
        out << qSetFieldWidth(0) << Qt::endl;
        QString step = "| Step: " + QString::fromStdString(to_string(count));
        out << step;
        for (int k = 0; k < (FIELD_WIDTH * (i.dist[0].size() + 3) + (i.dist[0].size() + 4) - step.size() - 1); k++) out << " ";
        out << qSetFieldWidth(1) << "|";

        out << qSetFieldWidth(0) << Qt::endl;


        int sizenode = i.node.size();
        for (int j = -1; j < sizenode; j++) {
            for (int k = 0; k < (FIELD_WIDTH * (i.dist[0].size() + 3) + (i.dist[0].size() + 4)); k++) out << "-";
            out << qSetFieldWidth(0) << Qt::endl;
            out << qSetFieldWidth(1) << "|";
            if (j == -1) {
                //cout << "  ";
                out << qSetFieldWidth(FIELD_WIDTH) << " ";
                for (int k = 0; k < i.dist[0].size(); k++) {
                    if (k == 0) out << qSetFieldWidth(1) << "|";
                    out << qSetFieldWidth(FIELD_WIDTH) << (int)i.dist[0][k];
                    out << qSetFieldWidth(1) << "|";
                }
                for (int k = 0; k < 2; k++) out << qSetFieldWidth(FIELD_WIDTH + 1) << "|";
            }
            else {
                out << qSetFieldWidth(FIELD_WIDTH) << i.node[j];
                out << qSetFieldWidth(1) << "|";
                for (int k = 0; k < i.dist[j].size(); k++) {
                    if (i.dist[j + 1][k] != 1000000) out << qSetFieldWidth(FIELD_WIDTH) << i.dist[j + 1][k];
                    else out << qSetFieldWidth(FIELD_WIDTH) << "None";
                    out << qSetFieldWidth(1) << "|";
                }
                out << qSetFieldWidth(FIELD_WIDTH) << i.min_size[j];
                out << qSetFieldWidth(1) << "|";
                QString best;
                for (int k = 0; k < i.best_var[j].size(); k++) {
                    if (k != 0) best += ",";
                    best += QString::fromStdString(to_string(i.best_var[j][k]));
                }
                out << qSetFieldWidth(FIELD_WIDTH) << best;
                out << qSetFieldWidth(1) << "|";
            }
            out << qSetFieldWidth(0) << Qt::endl;
        }
        for (int k = 0; k < (FIELD_WIDTH * (i.dist[0].size() + 3) + (i.dist[0].size() + 4)); k++) out << "-";
        out << qSetFieldWidth(0) << Qt::endl << Qt::endl;
    }

    if (ways.size() != 0) {
        auto& minElement = *ways.begin();
        int minKey = minElement.first;
        vector<string> minValue = minElement.second;
        if (minValue.size() > 1) out << "Best ways ("  << QString::fromStdString(to_string(startNode))  << " -> " << QString::fromStdString(to_string(endNode))  << "): " << Qt::endl;
        else out << "Best way: " << Qt::endl;
        out << "Length: " << QString::fromStdString(to_string(minKey)) << Qt::endl;
        count = 0;
        for (string str : minValue) {
            count++;
            out << QString::fromStdString(to_string(count)) << ") " << QString::fromStdString(str) << Qt::endl;
        }
    }
    else {
        out << "No ways found (" << QString::fromStdString(to_string(startNode))  << " -> " << QString::fromStdString(to_string(endNode))  << ")" << Qt::endl;
    }
    file.close();

    QString message = QString("Файл сохранен в директорию:\n'%1'")
        .arg(filePath);

    QMessageBox::information(this, "Файл сохранен", message);

}

void MainWindow::syncGraphFromScene() {
    vector<GraphArrow> currentArrows;
    for (Arrow* a : scene->getArrows()) {
        GraphArrow ga;
        ga.node_1 = a->startItem()->data(0).toInt();
        ga.node_2 = a->endItem()->data(0).toInt();
        ga.weight = to_string(a->getWeight());
        ga.isLoop = a->getLoop();
        currentArrows.push_back(ga);
    }

    graph.rebuildFromArrows(currentArrows);
}

void MainWindow::onCheckCycle(int from, int to, Arrow* arrow) {
    syncGraphFromScene();

    graphHasCycle = !graph.isDAG();

    if (graphHasCycle) {
        vector<pair<int, int>> cycleArrows = graph.findCycleArrows();

        for (Arrow* a : scene->getArrows()) {
            int f = a->startItem()->data(0).toInt();
            int t = a->endItem()->data(0).toInt();

            bool isCyclic = false;
            for (const auto& ca : cycleArrows) {
                if (ca.first == f && ca.second == t) {
                    isCyclic = true;
                    break;
                }
            }
            a->setCycle(isCyclic);
        }

        if (arrow) {
            QMessageBox::warning(this, "Цикл", "Эта стрелка создает цикл!");
        }
    }
    else {
        for (Arrow* a : scene->getArrows()) {
            a->setCycle(false);
        }
    }

    updateExecuteButton();
}

void MainWindow::updateExecuteButton() {
    QPushButton* execBtn = findChild<QPushButton*>("executeButton");
    if (execBtn) {
        execBtn->setEnabled(!graphHasCycle && !graph.getArrowsData().empty());
    }
}


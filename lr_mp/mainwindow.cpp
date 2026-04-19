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
    //QObject::connect(execBtn, &QPushButton::clicked, this, &MainWindow::executeGraph);

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

    Graph graph;
    graph.loadMatrix(fileName.toStdString());

    auto nodesData = graph.getNodesData();
    auto arrowsData = graph.getArrowsData();

    scene->loadGraph(nodesData, arrowsData);

    scene->setMode(DiagramScene::EditItems);
    edit_button->setChecked(true);
    updateNodesMovable(true);

}

/*
void executeGraph{

}
*/

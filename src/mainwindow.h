#pragma once

#include <QMainWindow>
#include <QGraphicsView>
#include "diagramscene.h"
#include "graph.h"

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

private:
    void setupUI();
    void createToolBar(); 
    void updateNodesMovable(bool movable);

    DiagramScene* scene;
    QGraphicsView* view;
    QToolButton* node_button;
    QToolButton* arrow_button;
    QToolButton* edit_button;
};
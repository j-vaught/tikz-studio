#ifndef TOOLPALETTE_H
#define TOOLPALETTE_H

#include <QToolBar>
#include "common.h"

class QActionGroup;

class ToolPalette : public QToolBar {
    Q_OBJECT

public:
    explicit ToolPalette(QWidget *parent = nullptr);

    Tool currentTool() const;
    void setCurrentTool(Tool tool);

signals:
    void toolChanged(Tool tool);

private:
    void setupActions();

    QAction *m_selectAction;
    QAction *m_pointAction;
    QAction *m_lineAction;
    QAction *m_polygonAction;
    QAction *m_curveAction;
    QAction *m_rectAction;
    QAction *m_circleAction;
    QAction *m_ellipseAction;
    QAction *m_triangleAction;
    QAction *m_regPolyAction;

    QActionGroup *m_actionGroup;
};

#endif // TOOLPALETTE_H

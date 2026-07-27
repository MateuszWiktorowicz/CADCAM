#pragma once

#include <QMainWindow>

#include "ToolpathView.hpp"
#include "toolpath/core/Geometry.hpp"

#include <optional>

class QAction;
class QActionGroup;
class QDoubleSpinBox;
class QPushButton;
class QTextEdit;

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void generateToolpath();
    void exportGCode();
    void useSelectTool();
    void useLineTool();
    void useRectangleTool();
    void useCircleTool();
    void useArcTool();
    void setSketchMode(ToolpathView::Mode mode);
    void setActiveProfile(const toolpath::core::Polyline2D& profile);

    ToolpathView* view_{nullptr};
    QPushButton* selectButton_{nullptr};
    QPushButton* lineButton_{nullptr};
    QPushButton* rectangleButton_{nullptr};
    QPushButton* circleButton_{nullptr};
    QPushButton* arcButton_{nullptr};
    QAction* selectAction_{nullptr};
    QAction* lineAction_{nullptr};
    QAction* rectangleAction_{nullptr};
    QAction* circleAction_{nullptr};
    QAction* arcAction_{nullptr};
    QActionGroup* geometryActions_{nullptr};
    QDoubleSpinBox* toolInput_{nullptr};
    QDoubleSpinBox* depthInput_{nullptr};
    QDoubleSpinBox* stepdownInput_{nullptr};
    QDoubleSpinBox* feedInput_{nullptr};
    QTextEdit* gcodePreview_{nullptr};
    std::optional<toolpath::core::Polyline2D> activeProfile_;
};

#pragma once

#include <QMainWindow>

#include "toolpath/core/Geometry.hpp"

#include <optional>

class QDoubleSpinBox;
class QPushButton;
class QTextEdit;
class ToolpathView;

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void generateToolpath();
    void exportGCode();
    void useSelectTool();
    void useRectangleTool();
    void setActiveProfile(const toolpath::core::Polyline2D& profile);

    ToolpathView* view_{nullptr};
    QPushButton* selectButton_{nullptr};
    QPushButton* rectangleButton_{nullptr};
    QDoubleSpinBox* toolInput_{nullptr};
    QDoubleSpinBox* depthInput_{nullptr};
    QDoubleSpinBox* stepdownInput_{nullptr};
    QDoubleSpinBox* feedInput_{nullptr};
    QTextEdit* gcodePreview_{nullptr};
    std::optional<toolpath::core::Polyline2D> activeProfile_;
};

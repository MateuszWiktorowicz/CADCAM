#include "MainWindow.hpp"

#include "ToolpathView.hpp"
#include "toolpath/core/GCodeWriter.hpp"
#include "toolpath/core/ProfileOperation.hpp"

#include <QAction>
#include <QActionGroup>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include <fstream>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Toolpath Studio");

    auto* geometryMenu = menuBar()->addMenu("Geometry");
    geometryActions_ = new QActionGroup(this);
    geometryActions_->setExclusive(true);

    selectAction_ = geometryMenu->addAction("Select");
    lineAction_ = geometryMenu->addAction("Line");
    rectangleAction_ = geometryMenu->addAction("Rectangle");
    circleAction_ = geometryMenu->addAction("Circle");
    arcAction_ = geometryMenu->addAction("Arc");

    for (auto* action : {selectAction_, lineAction_, rectangleAction_, circleAction_, arcAction_}) {
        action->setCheckable(true);
        geometryActions_->addAction(action);
    }
    rectangleAction_->setChecked(true);

    auto* root = new QWidget(this);
    auto* layout = new QHBoxLayout(root);

    auto* controls = new QWidget(root);
    controls->setFixedWidth(320);
    auto* controlLayout = new QVBoxLayout(controls);

    auto* sketchLabel = new QLabel("Sketch", controls);
    selectButton_ = new QPushButton("Select", controls);
    lineButton_ = new QPushButton("Line", controls);
    rectangleButton_ = new QPushButton("Rectangle", controls);
    circleButton_ = new QPushButton("Circle", controls);
    arcButton_ = new QPushButton("Arc", controls);
    selectButton_->setCheckable(true);
    lineButton_->setCheckable(true);
    rectangleButton_->setCheckable(true);
    circleButton_->setCheckable(true);
    arcButton_->setCheckable(true);
    rectangleButton_->setChecked(true);

    auto* sketchTools = new QGridLayout();
    sketchTools->addWidget(selectButton_, 0, 0);
    sketchTools->addWidget(lineButton_, 0, 1);
    sketchTools->addWidget(rectangleButton_, 1, 0);
    sketchTools->addWidget(circleButton_, 1, 1);
    sketchTools->addWidget(arcButton_, 2, 0, 1, 2);

    auto* form = new QFormLayout();
    toolInput_ = new QDoubleSpinBox(controls);
    toolInput_->setRange(0.1, 50.0);
    toolInput_->setValue(3.175);
    toolInput_->setSuffix(" mm");

    depthInput_ = new QDoubleSpinBox(controls);
    depthInput_->setRange(0.1, 100.0);
    depthInput_->setValue(2.0);
    depthInput_->setSuffix(" mm");

    stepdownInput_ = new QDoubleSpinBox(controls);
    stepdownInput_->setRange(0.1, 20.0);
    stepdownInput_->setValue(0.5);
    stepdownInput_->setSuffix(" mm");

    feedInput_ = new QDoubleSpinBox(controls);
    feedInput_->setRange(1.0, 10000.0);
    feedInput_->setValue(300.0);
    feedInput_->setSuffix(" mm/min");

    form->addRow("Tool diameter", toolInput_);
    form->addRow("Depth", depthInput_);
    form->addRow("Stepdown", stepdownInput_);
    form->addRow("Feed", feedInput_);

    auto* generateButton = new QPushButton("Generate", controls);
    auto* exportButton = new QPushButton("Export G-code", controls);
    gcodePreview_ = new QTextEdit(controls);
    gcodePreview_->setReadOnly(true);

    controlLayout->addWidget(sketchLabel);
    controlLayout->addLayout(sketchTools);
    controlLayout->addLayout(form);
    controlLayout->addWidget(generateButton);
    controlLayout->addWidget(exportButton);
    controlLayout->addWidget(gcodePreview_, 1);

    view_ = new ToolpathView(root);

    layout->addWidget(controls);
    layout->addWidget(view_, 1);
    setCentralWidget(root);

    view_->setMode(ToolpathView::Mode::DrawRectangle);

    connect(selectAction_, &QAction::triggered, this, &MainWindow::useSelectTool);
    connect(lineAction_, &QAction::triggered, this, &MainWindow::useLineTool);
    connect(rectangleAction_, &QAction::triggered, this, &MainWindow::useRectangleTool);
    connect(circleAction_, &QAction::triggered, this, &MainWindow::useCircleTool);
    connect(arcAction_, &QAction::triggered, this, &MainWindow::useArcTool);
    connect(selectButton_, &QPushButton::clicked, this, &MainWindow::useSelectTool);
    connect(lineButton_, &QPushButton::clicked, this, &MainWindow::useLineTool);
    connect(rectangleButton_, &QPushButton::clicked, this, &MainWindow::useRectangleTool);
    connect(circleButton_, &QPushButton::clicked, this, &MainWindow::useCircleTool);
    connect(arcButton_, &QPushButton::clicked, this, &MainWindow::useArcTool);
    connect(view_, &ToolpathView::profileChanged, this, &MainWindow::setActiveProfile);
    connect(generateButton, &QPushButton::clicked, this, &MainWindow::generateToolpath);
    connect(exportButton, &QPushButton::clicked, this, &MainWindow::exportGCode);
    gcodePreview_->setPlainText("Draw a rectangle in the viewport, then generate a toolpath.");
}

void MainWindow::generateToolpath()
{
    using namespace toolpath::core;

    if (!activeProfile_) {
        QMessageBox::information(this, "No sketch profile", "Draw a rectangle in the viewport first.");
        return;
    }

    ProfileInput input;
    input.profile = *activeProfile_;
    input.tool.diameterMm = toolInput_->value();
    input.cutting.cutDepthMm = -depthInput_->value();
    input.cutting.stepDownMm = stepdownInput_->value();
    input.cutting.feedRateMmMin = feedInput_->value();

    try {
        const auto path = ProfileOperation{}.generateProfile(input);
        const auto code = GCodeWriter{}.writeGrblProgram(path, input.tool);
        view_->setToolpath(path);
        gcodePreview_->setPlainText(QString::fromStdString(code));
    } catch (const std::exception& error) {
        QMessageBox::warning(this, "Generation failed", error.what());
    }
}

void MainWindow::exportGCode()
{
    const auto path = QFileDialog::getSaveFileName(this, "Export G-code", "rectangle_profile.nc", "G-code (*.nc *.gcode *.tap)");
    if (path.isEmpty()) {
        return;
    }

    std::ofstream file(path.toStdString());
    file << gcodePreview_->toPlainText().toStdString();
}

void MainWindow::useSelectTool()
{
    setSketchMode(ToolpathView::Mode::Select);
}

void MainWindow::useLineTool()
{
    setSketchMode(ToolpathView::Mode::DrawLine);
}

void MainWindow::useRectangleTool()
{
    setSketchMode(ToolpathView::Mode::DrawRectangle);
}

void MainWindow::useCircleTool()
{
    setSketchMode(ToolpathView::Mode::DrawCircle);
}

void MainWindow::useArcTool()
{
    setSketchMode(ToolpathView::Mode::DrawArc);
}

void MainWindow::setSketchMode(ToolpathView::Mode mode)
{
    selectButton_->setChecked(mode == ToolpathView::Mode::Select);
    lineButton_->setChecked(mode == ToolpathView::Mode::DrawLine);
    rectangleButton_->setChecked(mode == ToolpathView::Mode::DrawRectangle);
    circleButton_->setChecked(mode == ToolpathView::Mode::DrawCircle);
    arcButton_->setChecked(mode == ToolpathView::Mode::DrawArc);

    selectAction_->setChecked(mode == ToolpathView::Mode::Select);
    lineAction_->setChecked(mode == ToolpathView::Mode::DrawLine);
    rectangleAction_->setChecked(mode == ToolpathView::Mode::DrawRectangle);
    circleAction_->setChecked(mode == ToolpathView::Mode::DrawCircle);
    arcAction_->setChecked(mode == ToolpathView::Mode::DrawArc);

    view_->setMode(mode);
}

void MainWindow::setActiveProfile(const toolpath::core::Polyline2D& profile)
{
    activeProfile_ = profile;
    gcodePreview_->setPlainText("Profile ready. Generate a toolpath or draw a new rectangle.");
}

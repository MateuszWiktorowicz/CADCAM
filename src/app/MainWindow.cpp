#include "MainWindow.hpp"

#include "ToolpathView.hpp"
#include "toolpath/core/GCodeWriter.hpp"
#include "toolpath/core/ProfileOperation.hpp"

#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
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

    auto* root = new QWidget(this);
    auto* layout = new QHBoxLayout(root);

    auto* controls = new QWidget(root);
    controls->setFixedWidth(320);
    auto* controlLayout = new QVBoxLayout(controls);

    auto* sketchLabel = new QLabel("Sketch", controls);
    selectButton_ = new QPushButton("Select", controls);
    rectangleButton_ = new QPushButton("Rectangle", controls);
    selectButton_->setCheckable(true);
    rectangleButton_->setCheckable(true);
    rectangleButton_->setChecked(true);

    auto* sketchTools = new QHBoxLayout();
    sketchTools->addWidget(selectButton_);
    sketchTools->addWidget(rectangleButton_);

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

    connect(selectButton_, &QPushButton::clicked, this, &MainWindow::useSelectTool);
    connect(rectangleButton_, &QPushButton::clicked, this, &MainWindow::useRectangleTool);
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
    selectButton_->setChecked(true);
    rectangleButton_->setChecked(false);
    view_->setMode(ToolpathView::Mode::Select);
}

void MainWindow::useRectangleTool()
{
    selectButton_->setChecked(false);
    rectangleButton_->setChecked(true);
    view_->setMode(ToolpathView::Mode::DrawRectangle);
}

void MainWindow::setActiveProfile(const toolpath::core::Polyline2D& profile)
{
    activeProfile_ = profile;
    gcodePreview_->setPlainText("Profile ready. Generate a toolpath or draw a new rectangle.");
}

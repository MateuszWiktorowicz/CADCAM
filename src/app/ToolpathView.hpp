#pragma once

#include "ViewportCamera2D.hpp"
#include "toolpath/core/Toolpath.hpp"
#include "toolpath/sketch/Arc2D.hpp"
#include "toolpath/sketch/Circle2D.hpp"
#include "toolpath/sketch/LineSegment2D.hpp"
#include "toolpath/sketch/Sketch2D.hpp"

#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QPointF>

#include <optional>

class ToolpathView final : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    enum class Mode {
        Select,
        DrawLine,
        DrawRectangle,
        DrawCircle,
        DrawArc
    };

    explicit ToolpathView(QWidget* parent = nullptr);

    void setMode(Mode mode);
    void setProfile(toolpath::core::Polyline2D profile);
    void setToolpath(toolpath::core::Toolpath toolpath);

signals:
    void profileChanged(const toolpath::core::Polyline2D& profile);

protected:
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    [[nodiscard]] toolpath::core::Point2D screenToWorld(const QPoint& point) const;
    [[nodiscard]] QPointF worldToScreen(const toolpath::core::Point2D& point) const;
    [[nodiscard]] toolpath::core::Polyline2D rectangleFromCorners(
        const toolpath::core::Point2D& first,
        const toolpath::core::Point2D& second
    ) const;
    [[nodiscard]] toolpath::core::Point2D snapToGrid(const toolpath::core::Point2D& point) const;

    void startPan(const QPointF& screenPosition);
    void updatePan(const QPointF& screenPosition);
    void finishPan();
    void drawGrid(QPainter& painter) const;
    void drawProfile(QPainter& painter, const toolpath::core::Polyline2D& profile, const QColor& color, double width) const;
    void drawLineSegment(QPainter& painter, const toolpath::sketch::LineSegment2D& line, const QColor& color, double width) const;
    void drawCircle(QPainter& painter, const toolpath::sketch::Circle2D& circle, const QColor& color, double width) const;
    void drawArc(QPainter& painter, const toolpath::sketch::Arc2D& arc, const QColor& color, double width) const;
    void drawSketch(QPainter& painter) const;
    void drawPreview(QPainter& painter) const;
    void drawToolpath(QPainter& painter) const;
    void drawCoordinateSystem(QPainter& painter) const;
    [[nodiscard]] toolpath::sketch::LineSegment2D previewLine() const;
    [[nodiscard]] toolpath::sketch::Circle2D previewCircle() const;
    [[nodiscard]] toolpath::sketch::Arc2D previewArc() const;

    Mode mode_{Mode::Select};
    double gridStepMm_{5.0};
    ViewportCamera2D camera_;
    bool panning_{false};
    QPointF lastPanScreenPosition_{0.0, 0.0};
    bool drawing_{false};
    toolpath::core::Point2D dragStart_{0.0, 0.0};
    toolpath::core::Point2D dragCurrent_{0.0, 0.0};
    std::optional<toolpath::core::Polyline2D> profile_;
    toolpath::sketch::Sketch2D sketch_;
    toolpath::core::Toolpath toolpath_;
};

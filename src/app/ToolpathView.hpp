#pragma once

#include "toolpath/core/Toolpath.hpp"

#include <QOpenGLFunctions>
#include <QOpenGLWidget>

#include <optional>

class ToolpathView final : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    enum class Mode {
        Select,
        DrawRectangle
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

private:
    [[nodiscard]] toolpath::core::Point2D screenToWorld(const QPoint& point) const;
    [[nodiscard]] QPointF worldToScreen(const toolpath::core::Point2D& point) const;
    [[nodiscard]] toolpath::core::Polyline2D rectangleFromCorners(
        const toolpath::core::Point2D& first,
        const toolpath::core::Point2D& second
    ) const;
    [[nodiscard]] toolpath::core::Point2D snapToGrid(const toolpath::core::Point2D& point) const;

    void drawGrid(QPainter& painter) const;
    void drawProfile(QPainter& painter, const toolpath::core::Polyline2D& profile, const QColor& color, double width) const;
    void drawToolpath(QPainter& painter) const;
    void drawCoordinateSystem(QPainter& painter) const;

    Mode mode_{Mode::Select};
    double scalePxPerMm_{8.0};
    double gridStepMm_{5.0};
    bool drawing_{false};
    toolpath::core::Point2D dragStart_{0.0, 0.0};
    toolpath::core::Point2D dragCurrent_{0.0, 0.0};
    std::optional<toolpath::core::Polyline2D> profile_;
    toolpath::core::Toolpath toolpath_;
};

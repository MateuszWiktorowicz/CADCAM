#include "ToolpathView.hpp"

#ifdef TOOLPATH_HAS_CAD
#include "toolpath/cad/CartesianCoordinateSystem.hpp"
#endif

#include <QMouseEvent>
#include <QPainter>

#include <algorithm>
#include <cmath>
#include <type_traits>
#include <variant>

ToolpathView::ToolpathView(QWidget* parent)
    : QOpenGLWidget(parent)
{
    setMinimumSize(640, 480);
    setMouseTracking(true);
}

void ToolpathView::setMode(Mode mode)
{
    mode_ = mode;
    setCursor(mode_ == Mode::Select ? Qt::ArrowCursor : Qt::CrossCursor);
}

void ToolpathView::setProfile(toolpath::core::Polyline2D profile)
{
    profile_ = std::move(profile);
    toolpath_.moves.clear();
    update();
}

void ToolpathView::setToolpath(toolpath::core::Toolpath toolpath)
{
    toolpath_ = std::move(toolpath);
    update();
}

void ToolpathView::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.08f, 0.09f, 0.10f, 1.0f);
}

void ToolpathView::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}

void ToolpathView::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(21, 24, 28));

    drawGrid(painter);
    drawCoordinateSystem(painter);

    if (profile_) {
        drawProfile(painter, *profile_, QColor(235, 238, 242), 2.0);
    }

    drawSketch(painter);
    drawPreview(painter);

    drawToolpath(painter);
}

void ToolpathView::mousePressEvent(QMouseEvent* event)
{
    if (mode_ == Mode::Select || event->button() != Qt::LeftButton) {
        return;
    }

    dragStart_ = snapToGrid(screenToWorld(event->pos()));
    dragCurrent_ = dragStart_;
    drawing_ = true;
    update();
}

void ToolpathView::mouseMoveEvent(QMouseEvent* event)
{
    if (!drawing_) {
        return;
    }

    dragCurrent_ = snapToGrid(screenToWorld(event->pos()));
    update();
}

void ToolpathView::mouseReleaseEvent(QMouseEvent* event)
{
    if (!drawing_ || event->button() != Qt::LeftButton) {
        return;
    }

    drawing_ = false;
    dragCurrent_ = snapToGrid(screenToWorld(event->pos()));

    if (mode_ == Mode::DrawLine) {
        auto line = previewLine();
        if (!line.isDegenerate()) {
            sketch_.add(line);
            toolpath_.moves.clear();
        }
    } else if (mode_ == Mode::DrawRectangle && std::abs(dragCurrent_.x() - dragStart_.x()) >= 1.0 && std::abs(dragCurrent_.y() - dragStart_.y()) >= 1.0) {
        profile_ = rectangleFromCorners(dragStart_, dragCurrent_);
        toolpath_.moves.clear();
        emit profileChanged(*profile_);
    } else if (mode_ == Mode::DrawCircle) {
        auto circle = previewCircle();
        if (!circle.isDegenerate()) {
            sketch_.add(circle);
            toolpath_.moves.clear();
        }
    } else if (mode_ == Mode::DrawArc) {
        auto arc = previewArc();
        if (!arc.isDegenerate()) {
            sketch_.add(arc);
            toolpath_.moves.clear();
        }
    }

    update();
}

toolpath::core::Point2D ToolpathView::screenToWorld(const QPoint& point) const
{
    const double x = (point.x() - width() * 0.5) / scalePxPerMm_;
    const double y = (height() * 0.5 - point.y()) / scalePxPerMm_;
    return toolpath::core::Point2D{x, y};
}

QPointF ToolpathView::worldToScreen(const toolpath::core::Point2D& point) const
{
    return QPointF{
        width() * 0.5 + point.x() * scalePxPerMm_,
        height() * 0.5 - point.y() * scalePxPerMm_
    };
}

toolpath::core::Polyline2D ToolpathView::rectangleFromCorners(
    const toolpath::core::Point2D& first,
    const toolpath::core::Point2D& second
) const
{
    const double minX = std::min(first.x(), second.x());
    const double maxX = std::max(first.x(), second.x());
    const double minY = std::min(first.y(), second.y());
    const double maxY = std::max(first.y(), second.y());

    return toolpath::core::Polyline2D{
        {
            toolpath::core::Point2D{minX, minY},
            toolpath::core::Point2D{maxX, minY},
            toolpath::core::Point2D{maxX, maxY},
            toolpath::core::Point2D{minX, maxY},
            toolpath::core::Point2D{minX, minY},
        },
        true
    };
}

toolpath::core::Point2D ToolpathView::snapToGrid(const toolpath::core::Point2D& point) const
{
    return toolpath::core::Point2D{
        std::round(point.x()),
        std::round(point.y())
    };
}

void ToolpathView::drawGrid(QPainter& painter) const
{
    painter.save();
    painter.setPen(QPen(QColor(42, 47, 54), 1.0));

    const auto topLeft = screenToWorld(QPoint{0, 0});
    const auto bottomRight = screenToWorld(QPoint{width(), height()});

    const double minX = std::floor(topLeft.x() / gridStepMm_) * gridStepMm_;
    const double maxX = std::ceil(bottomRight.x() / gridStepMm_) * gridStepMm_;
    const double minY = std::floor(bottomRight.y() / gridStepMm_) * gridStepMm_;
    const double maxY = std::ceil(topLeft.y() / gridStepMm_) * gridStepMm_;

    for (double x = minX; x <= maxX; x += gridStepMm_) {
        painter.drawLine(worldToScreen({x, minY}), worldToScreen({x, maxY}));
    }

    for (double y = minY; y <= maxY; y += gridStepMm_) {
        painter.drawLine(worldToScreen({minX, y}), worldToScreen({maxX, y}));
    }

    painter.setPen(QPen(QColor(85, 93, 105), 1.5));
    painter.drawLine(worldToScreen({0.0, minY}), worldToScreen({0.0, maxY}));
    painter.drawLine(worldToScreen({minX, 0.0}), worldToScreen({maxX, 0.0}));
    painter.restore();
}

void ToolpathView::drawProfile(QPainter& painter, const toolpath::core::Polyline2D& profile, const QColor& color, double width) const
{
    if (profile.points.size() < 2) {
        return;
    }

    painter.save();
    painter.setPen(QPen(color, width));
    for (std::size_t i = 1; i < profile.points.size(); ++i) {
        painter.drawLine(worldToScreen(profile.points[i - 1]), worldToScreen(profile.points[i]));
    }
    painter.restore();
}

void ToolpathView::drawLineSegment(QPainter& painter, const toolpath::sketch::LineSegment2D& line, const QColor& color, double width) const
{
    painter.save();
    painter.setPen(QPen(color, width));
    painter.drawLine(worldToScreen(line.startPoint), worldToScreen(line.endPoint));
    painter.restore();
}

void ToolpathView::drawCircle(QPainter& painter, const toolpath::sketch::Circle2D& circle, const QColor& color, double width) const
{
    if (circle.isDegenerate()) {
        return;
    }

    const auto center = worldToScreen(circle.center);
    const double radiusPx = circle.radiusMm * scalePxPerMm_;

    painter.save();
    painter.setPen(QPen(color, width));
    painter.drawEllipse(center, radiusPx, radiusPx);
    painter.restore();
}

void ToolpathView::drawArc(QPainter& painter, const toolpath::sketch::Arc2D& arc, const QColor& color, double width) const
{
    if (arc.isDegenerate()) {
        return;
    }

    constexpr int sampleCount = 48;

    painter.save();
    painter.setPen(QPen(color, width));

    auto previous = worldToScreen(arc.startPoint());
    const double sweep = arc.sweepAngleRad();
    const double direction = arc.clockwise ? -1.0 : 1.0;

    for (int i = 1; i <= sampleCount; ++i) {
        const double t = static_cast<double>(i) / static_cast<double>(sampleCount);
        const double angle = arc.startAngleRad + direction * sweep * t;
        const auto current = worldToScreen(arc.pointAtAngle(angle));
        painter.drawLine(previous, current);
        previous = current;
    }

    painter.restore();
}

void ToolpathView::drawSketch(QPainter& painter) const
{
    const auto drawEntity = [&](const auto& entity) {
        using Entity = std::decay_t<decltype(entity)>;
        if constexpr (std::is_same_v<Entity, toolpath::sketch::LineSegment2D>) {
            drawLineSegment(painter, entity, QColor(235, 238, 242), 2.0);
        } else if constexpr (std::is_same_v<Entity, toolpath::sketch::Circle2D>) {
            drawCircle(painter, entity, QColor(235, 238, 242), 2.0);
        } else if constexpr (std::is_same_v<Entity, toolpath::sketch::Arc2D>) {
            drawArc(painter, entity, QColor(235, 238, 242), 2.0);
        }
    };

    for (const auto& entity : sketch_.entities) {
        std::visit(drawEntity, entity);
    }
}

void ToolpathView::drawPreview(QPainter& painter) const
{
    if (!drawing_) {
        return;
    }

    const QColor previewColor{255, 205, 90};

    switch (mode_) {
    case Mode::Select:
        break;
    case Mode::DrawLine:
        drawLineSegment(painter, previewLine(), previewColor, 1.5);
        break;
    case Mode::DrawRectangle:
        drawProfile(painter, rectangleFromCorners(dragStart_, dragCurrent_), previewColor, 1.5);
        break;
    case Mode::DrawCircle:
        drawCircle(painter, previewCircle(), previewColor, 1.5);
        break;
    case Mode::DrawArc:
        drawArc(painter, previewArc(), previewColor, 1.5);
        break;
    }
}

void ToolpathView::drawToolpath(QPainter& painter) const
{
    painter.save();

    QPen rapidPen(QColor(90, 170, 255), 1.4);
    QPen cutPen(QColor(80, 220, 150), 2.0);
    QPen plungePen(QColor(255, 190, 90), 1.7);

    for (std::size_t i = 1; i < toolpath_.moves.size(); ++i) {
        const auto& previous = toolpath_.moves[i - 1];
        const auto& current = toolpath_.moves[i];

        switch (current.type) {
        case toolpath::core::MoveType::Rapid:
            painter.setPen(rapidPen);
            break;
        case toolpath::core::MoveType::Plunge:
            painter.setPen(plungePen);
            break;
        case toolpath::core::MoveType::Cut:
            painter.setPen(cutPen);
            break;
        }

        painter.drawLine(
            worldToScreen({previous.position.x(), previous.position.y()}),
            worldToScreen({current.position.x(), current.position.y()})
        );
    }

    painter.restore();
}

void ToolpathView::drawCoordinateSystem(QPainter& painter) const
{
#ifdef TOOLPATH_HAS_CAD
    const toolpath::cad::CartesianCoordinateSystem coordinateSystem;
    const double axisLengthMm = 25.0;

    const auto project = [](const toolpath::core::Point3D& point) {
        return toolpath::core::Point2D{
            point.x() + point.z() * 0.35,
            point.y() + point.z() * 0.35
        };
    };

    const auto drawAxis = [&](const toolpath::cad::AxisSegment3D& axis, const QColor& color, const QString& label) {
        const auto start = worldToScreen(project(axis.start));
        const auto end = worldToScreen(project(axis.end));

        painter.setPen(QPen(color, 2.4));
        painter.drawLine(start, end);

        painter.setBrush(color);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(end, 4.0, 4.0);

        painter.setPen(color);
        painter.drawText(end + QPointF{6.0, -6.0}, label);
    };

    painter.save();
    drawAxis(coordinateSystem.xAxis(axisLengthMm), QColor(235, 90, 90), "X");
    drawAxis(coordinateSystem.yAxis(axisLengthMm), QColor(90, 220, 140), "Y");
    drawAxis(coordinateSystem.zAxis(axisLengthMm), QColor(95, 155, 255), "Z");

    const auto origin = worldToScreen(project(coordinateSystem.origin()));
    painter.setBrush(QColor(235, 238, 242));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(origin, 3.5, 3.5);
    painter.restore();
#else
    (void)painter;
#endif
}

toolpath::sketch::LineSegment2D ToolpathView::previewLine() const
{
    return toolpath::sketch::LineSegment2D{dragStart_, dragCurrent_};
}

toolpath::sketch::Circle2D ToolpathView::previewCircle() const
{
    return toolpath::sketch::Circle2D{dragStart_, (dragCurrent_ - dragStart_).norm()};
}

toolpath::sketch::Arc2D ToolpathView::previewArc() const
{
    const auto radiusVector = dragCurrent_ - dragStart_;
    const double endAngleRad = std::atan2(radiusVector.y(), radiusVector.x());

    return toolpath::sketch::Arc2D{
        dragStart_,
        radiusVector.norm(),
        0.0,
        endAngleRad,
        endAngleRad < 0.0
    };
}

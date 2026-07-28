#pragma once

#include "toolpath/core/Geometry.hpp"

#include <QPoint>
#include <QPointF>
#include <QSize>

class ViewportCamera2D {
public:
    [[nodiscard]] toolpath::core::Point2D screenToWorld(const QPoint& point, const QSize& viewportSize) const;
    [[nodiscard]] QPointF worldToScreen(const toolpath::core::Point2D& point, const QSize& viewportSize) const;

    void panByPixels(const QPointF& deltaPx);
    void zoomByFactor(double factor);

    [[nodiscard]] double scalePxPerMm() const;

private:
    double scalePxPerMm_{8.0};
    toolpath::core::Point2D centerMm_{0.0, 0.0};
};

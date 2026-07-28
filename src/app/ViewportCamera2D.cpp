#include "ViewportCamera2D.hpp"

#include <algorithm>

toolpath::core::Point2D ViewportCamera2D::screenToWorld(const QPoint& point, const QSize& viewportSize) const
{
    const double x = centerMm_.x() + (point.x() - viewportSize.width() * 0.5) / scalePxPerMm_;
    const double y = centerMm_.y() + (viewportSize.height() * 0.5 - point.y()) / scalePxPerMm_;
    return toolpath::core::Point2D{x, y};
}

QPointF ViewportCamera2D::worldToScreen(const toolpath::core::Point2D& point, const QSize& viewportSize) const
{
    return QPointF{
        viewportSize.width() * 0.5 + (point.x() - centerMm_.x()) * scalePxPerMm_,
        viewportSize.height() * 0.5 - (point.y() - centerMm_.y()) * scalePxPerMm_
    };
}

void ViewportCamera2D::panByPixels(const QPointF& deltaPx)
{
    centerMm_.x() -= deltaPx.x() / scalePxPerMm_;
    centerMm_.y() += deltaPx.y() / scalePxPerMm_;
}

void ViewportCamera2D::zoomByFactor(double factor)
{
    constexpr double minScalePxPerMm = 1.0;
    constexpr double maxScalePxPerMm = 80.0;

    scalePxPerMm_ = std::clamp(scalePxPerMm_ * factor, minScalePxPerMm, maxScalePxPerMm);
}

double ViewportCamera2D::scalePxPerMm() const
{
    return scalePxPerMm_;
}

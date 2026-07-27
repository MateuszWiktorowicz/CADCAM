#include "toolpath/sketch/Arc2D.hpp"
#include "toolpath/sketch/Circle2D.hpp"
#include "toolpath/sketch/LineSegment2D.hpp"
#include "toolpath/sketch/Sketch2D.hpp"

#include <gtest/gtest.h>

#include <cmath>

using namespace toolpath::sketch;

namespace {

constexpr double kPi = 3.14159265358979323846;

} // namespace

TEST(SketchGeometryTests, LineSegmentReportsLength)
{
    const LineSegment2D line{
        toolpath::core::Point2D{0.0, 0.0},
        toolpath::core::Point2D{3.0, 4.0}
    };

    EXPECT_DOUBLE_EQ(line.lengthMm(), 5.0);
    EXPECT_FALSE(line.isDegenerate());
}

TEST(SketchGeometryTests, CircleReportsBasicMeasurements)
{
    const Circle2D circle{
        toolpath::core::Point2D{10.0, 20.0},
        2.0
    };

    EXPECT_DOUBLE_EQ(circle.diameterMm(), 4.0);
    EXPECT_DOUBLE_EQ(circle.circumferenceMm(), 4.0 * kPi);
    EXPECT_DOUBLE_EQ(circle.areaMm2(), 4.0 * kPi);
    EXPECT_FALSE(circle.isDegenerate());
}

TEST(SketchGeometryTests, ArcReportsEndpointsAndLength)
{
    const Arc2D arc{
        toolpath::core::Point2D{0.0, 0.0},
        10.0,
        0.0,
        kPi * 0.5,
        false
    };

    EXPECT_NEAR(arc.startPoint().x(), 10.0, 1e-9);
    EXPECT_NEAR(arc.startPoint().y(), 0.0, 1e-9);
    EXPECT_NEAR(arc.endPoint().x(), 0.0, 1e-9);
    EXPECT_NEAR(arc.endPoint().y(), 10.0, 1e-9);
    EXPECT_DOUBLE_EQ(arc.sweepAngleRad(), kPi * 0.5);
    EXPECT_DOUBLE_EQ(arc.lengthMm(), 10.0 * kPi * 0.5);
}

TEST(SketchGeometryTests, SketchStoresMixedEntities)
{
    Sketch2D sketch;
    sketch.add(LineSegment2D{});
    sketch.add(Circle2D{});
    sketch.add(Arc2D{});

    EXPECT_EQ(sketch.size(), 3);
    EXPECT_FALSE(sketch.empty());

    sketch.clear();

    EXPECT_TRUE(sketch.empty());
}

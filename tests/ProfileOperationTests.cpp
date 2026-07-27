#include "toolpath/core/ProfileOperation.hpp"

#include <gtest/gtest.h>

using namespace toolpath::core;

TEST(ProfileOperationTests, GeneratesMultipleDepthPasses)
{
    RectangleProfileInput input;
    input.widthMm = 40.0;
    input.heightMm = 20.0;
    input.cutting.cutDepthMm = -2.0;
    input.cutting.stepDownMm = 0.5;

    const auto path = ProfileOperation{}.generateRectangleProfile(input);

    EXPECT_EQ(path.moves.size(), 22);
    EXPECT_DOUBLE_EQ(path.moves.front().position.z(), 5.0);
    EXPECT_DOUBLE_EQ(path.moves[path.moves.size() - 2].position.z(), -2.0);
}

TEST(ProfileOperationTests, RejectsTooLargeToolForInsideProfile)
{
    RectangleProfileInput input;
    input.widthMm = 2.0;
    input.heightMm = 2.0;
    input.tool.diameterMm = 3.0;
    input.compensation = CompensationSide::Inside;

    EXPECT_THROW({
        const auto ignored = ProfileOperation{}.generateRectangleProfile(input);
        (void)ignored;
    }, std::invalid_argument);
}

TEST(ProfileOperationTests, GeneratesProfileFromClosedPolyline)
{
    ProfileInput input;
    input.profile = Polyline2D{
        {
            Point2D{-10.0, -5.0},
            Point2D{10.0, -5.0},
            Point2D{10.0, 5.0},
            Point2D{-10.0, 5.0},
            Point2D{-10.0, -5.0},
        },
        true
    };
    input.cutting.cutDepthMm = -1.0;
    input.cutting.stepDownMm = 0.5;

    const auto path = ProfileOperation{}.generateProfile(input);

    EXPECT_EQ(path.moves.size(), 12);
    EXPECT_DOUBLE_EQ(path.moves.front().position.z(), 5.0);
    EXPECT_DOUBLE_EQ(path.moves[path.moves.size() - 2].position.z(), -1.0);
}

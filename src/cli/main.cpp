#include "toolpath/core/GCodeWriter.hpp"
#include "toolpath/core/ProfileOperation.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

int main()
{
    using namespace toolpath::core;

    RectangleProfileInput input;
    input.widthMm = 40.0;
    input.heightMm = 20.0;
    input.tool.diameterMm = 3.175;
    input.tool.spindleRpm = 12000.0;
    input.cutting.cutDepthMm = -2.0;
    input.cutting.stepDownMm = 0.5;
    input.cutting.feedRateMmMin = 300.0;
    input.cutting.plungeRateMmMin = 120.0;
    input.cutting.safeZMm = 5.0;

    const auto toolpath = ProfileOperation{}.generateRectangleProfile(input);
    const auto gcode = GCodeWriter{}.writeGrblProgram(toolpath, input.tool);

    const auto output = std::filesystem::path{"samples/rectangle_profile.nc"};
    std::ofstream file(output);
    file << gcode;

    std::cout << "Generated " << output << " with " << toolpath.moves.size() << " moves.\n";
    return 0;
}


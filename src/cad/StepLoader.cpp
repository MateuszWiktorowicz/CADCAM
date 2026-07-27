#include "toolpath/cad/StepLoader.hpp"

#include <IFSelect_ReturnStatus.hxx>
#include <STEPControl_Reader.hxx>

#include <stdexcept>
#include <string>

namespace toolpath::cad {

TopoDS_Shape StepLoader::loadShape(const std::filesystem::path& path) const
{
    STEPControl_Reader reader;
    const auto status = reader.ReadFile(path.string().c_str());
    if (status != IFSelect_RetDone) {
        throw std::runtime_error("Failed to read STEP file: " + path.string());
    }

    reader.TransferRoots();
    return reader.OneShape();
}

} // namespace toolpath::cad


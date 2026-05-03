#include "MBDynDriveMeter.h"
#include "SimulationStoppingError.h"

using namespace MbD;

std::shared_ptr<MBDynDriveMeter> MBDynDriveMeter::With()
{
    auto inst = std::make_shared<MBDynDriveMeter>();
    inst->initialize();
    return inst;
}

void MBDynDriveMeter::readFunction(std::vector<std::string>& args)
{
    {auto _hdr = readStringNoSpacesOffTop(args); (void)_hdr; assert(_hdr == "meter");}
    auto startOffset = readDoubleOffTop(args);
    assert(startOffset == 0.0);
    {auto _hdr = readStringNoSpacesOffTop(args); (void)_hdr; assert(_hdr == "forever");}
    {auto _hdr = readStringNoSpacesOffTop(args); (void)_hdr; assert(_hdr == "steps");}
    auto steps = readIntOffTop(args);
    auto ss = std::stringstream();
    ss << steps;
    formula = ss.str();

}

void MBDynDriveMeter::createASMT()
{
    throw SimulationStoppingError("To be implemented.");
}


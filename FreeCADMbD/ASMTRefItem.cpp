/***************************************************************************
 *   Copyright (c) 2023 Ondsel, Inc.                                       *
 *                                                                         *
 *   This file is part of OndselSolver.                                    *
 *                                                                         *
 *   See LICENSE file for details about copyright.                         *
 ***************************************************************************/
 
#include "ASMTRefItem.h"
#include <algorithm>

using namespace MbD;

std::shared_ptr<ASMTRefItem> ASMTRefItem::With()
{
    auto inst = std::make_shared<ASMTRefItem>();
    inst->initialize();
    return inst;
}

void ASMTRefItem::addMarker(std::shared_ptr<ASMTMarker> marker)
{
    markers->push_back(marker);
    marker->owner = this;
}

void ASMTRefItem::readMarkers(std::vector<std::string>& lines)
{
    {auto _hdr = readStringNoSpacesOffTop(lines); (void)_hdr; assert(_hdr == "Markers");}
    markers->clear();
    auto it = std::find_if(lines.begin(), lines.end(), [](const std::string& s) {
        return s.find("RefPoint") != std::string::npos;
        });
    std::vector<std::string> markersLines(lines.begin(), it);
    // Phase-0 diagnostic: report if markersLines is surprisingly short
    if (std::getenv("MBD_DEBUG_PARSE")) {
        std::cerr << "[readMarkers] markersLines.size()=" << markersLines.size()
                  << " first=" << (markersLines.empty() ? "(empty)" : markersLines.front().substr(0,40))
                  << std::endl;
    }
    while (!markersLines.empty()) {
        readMarker(markersLines);
    }
    lines.erase(lines.begin(), it);
}

void ASMTRefItem::readMarker(std::vector<std::string>& lines)
{
    {auto _hdr = readStringNoSpacesOffTop(lines); (void)_hdr; assert(_hdr == "Marker");}
    auto marker = ASMTMarker::With();
    marker->owner = this;
    marker->parseASMT(lines);
    markers->push_back(marker);
}

void ASMTRefItem::storeOnLevel(std::ofstream& os, size_t level)
{
    storeOnLevelString(os, level, "RefPoints");
    ASMTSpatialItem::storeOnLevel(os, level+1);
    for (auto& marker : *markers) {
        marker->storeOnLevel(os, level);
    }
}

void ASMTRefItem::updateFromInputState()
{
    for (auto& marker : *markers) {
        marker->updateFromInputState();
    }
}

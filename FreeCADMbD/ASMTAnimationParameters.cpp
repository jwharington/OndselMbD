/***************************************************************************
 *   Copyright (c) 2023 Ondsel, Inc.                                       *
 *                                                                         *
 *   This file is part of OndselSolver.                                    *
 *                                                                         *
 *   See LICENSE file for details about copyright.                         *
 ***************************************************************************/

#include "ASMTAnimationParameters.h"

using namespace MbD;

std::shared_ptr<ASMTAnimationParameters> ASMTAnimationParameters::With()
{
    auto inst = std::make_shared<ASMTAnimationParameters>();
    inst->initialize();
    return inst;
}

void ASMTAnimationParameters::initialize()
{
    nframe = 1000000;
    icurrent = 1;
    istart = 1;
    iend = 1000000;
    isForward = true;
    framesPerSecond = 30;
}

void ASMTAnimationParameters::parseASMT(std::vector<std::string>& lines)
{
    //size_t nframe, icurrent, istart, iend, framesPerSecond;
    //bool isForward;
    auto pos = lines[0].find_first_not_of("\t");
    auto leadingTabs = lines[0].substr(0, pos);
    assert(lines[0] == (leadingTabs + "nframe"));
    safePopFront(lines);
    nframe = readSize_t(lines[0]);
    safePopFront(lines);
    assert(lines[0] == (leadingTabs + "icurrent"));
    safePopFront(lines);
    icurrent = readSize_t(lines[0]);
    safePopFront(lines);
    assert(lines[0] == (leadingTabs + "istart"));
    safePopFront(lines);
    istart = readSize_t(lines[0]);
    safePopFront(lines);
    assert(lines[0] == (leadingTabs + "iend"));
    safePopFront(lines);
    iend = readSize_t(lines[0]);
    safePopFront(lines);
    assert(lines[0] == (leadingTabs + "isForward"));
    safePopFront(lines);
    isForward = readBool(lines[0]);
    safePopFront(lines);
    assert(lines[0] == (leadingTabs + "framesPerSecond"));
    safePopFront(lines);
    framesPerSecond = readSize_t(lines[0]);
    safePopFront(lines);

}

void ASMTAnimationParameters::storeOnLevel(std::ofstream& os, size_t level)
{
    storeOnLevelString(os, level, "AnimationParameters");
    storeOnLevelString(os, level + 1, "nframe");
    storeOnLevelSize_t(os, level + 2, nframe);
    storeOnLevelString(os, level + 1, "icurrent");
    storeOnLevelSize_t(os, level + 2, icurrent);
    storeOnLevelString(os, level + 1, "istart");
    storeOnLevelSize_t(os, level + 2, istart);
    storeOnLevelString(os, level + 1, "iend");
    storeOnLevelSize_t(os, level + 2, iend);
    storeOnLevelString(os, level + 1, "isForward");
    storeOnLevelBool(os, level + 2, isForward);
    storeOnLevelString(os, level + 1, "framesPerSecond");
    storeOnLevelSize_t(os, level + 2, framesPerSecond);
}

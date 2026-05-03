/***************************************************************************
 *   Copyright (c) 2023 Ondsel, Inc.                                       *
 *                                                                         *
 *   This file is part of OndselSolver.                                    *
 *                                                                         *
 *   See LICENSE file for details about copyright.                         *
 ***************************************************************************/

#include "ASMTSimulationParameters.h"

using namespace MbD;

std::shared_ptr<ASMTSimulationParameters> ASMTSimulationParameters::With()
{
    auto inst = std::make_shared<ASMTSimulationParameters>();
    inst->initialize();
    return inst;
}

void ASMTSimulationParameters::initialize()
{
    //Do nothing.
}

void ASMTSimulationParameters::parseASMT(std::vector<std::string>& lines)
{
    //tstart, tend, hmin, hmax, hout, errorTol;

    size_t pos = lines[0].find_first_not_of("\t");
    auto leadingTabs = lines[0].substr(0, pos);
    assert(lines[0] == (leadingTabs + "tstart"));
    safePopFront(lines);
    tstart = readDouble(lines[0]);
    safePopFront(lines);
    assert(lines[0] == (leadingTabs + "tend"));
    safePopFront(lines);
    tend = readDouble(lines[0]);
    safePopFront(lines);
    assert(lines[0] == (leadingTabs + "hmin"));
    safePopFront(lines);
    hmin = readDouble(lines[0]);
    safePopFront(lines);
    assert(lines[0] == (leadingTabs + "hmax"));
    safePopFront(lines);
    hmax = readDouble(lines[0]);
    safePopFront(lines);
    assert(lines[0] == (leadingTabs + "hout"));
    safePopFront(lines);
    hout = readDouble(lines[0]);
    safePopFront(lines);
    assert(lines[0] == (leadingTabs + "errorTol"));
    safePopFront(lines);
    seterrorTol(readDouble(lines[0]));
    safePopFront(lines);

}

void ASMTSimulationParameters::settstart(double t)
{
    tstart = t;
}

void ASMTSimulationParameters::settend(double t)
{
    tend = t;
}

void ASMTSimulationParameters::sethmin(double h)
{
    hmin = h;
}

void ASMTSimulationParameters::sethmax(double h)
{
    hmax = h;
}

void ASMTSimulationParameters::sethout(double h)
{
    hout = h;
}

void ASMTSimulationParameters::seterrorTol(double tol)
{
    errorTol = tol;
    errorTolPosKine = tol;
    errorTolAccKine = tol;
    corAbsTol = tol;
    corRelTol = tol;
    intAbsTol = tol;
    intRelTol = tol;
}

void ASMTSimulationParameters::setmaxIter(size_t maxIter)
{
    iterMaxPosKine = maxIter;
    iterMaxAccKine = maxIter;
}

void ASMTSimulationParameters::storeOnLevel(std::ofstream& os, size_t level)
{
    storeOnLevelString(os, level, "SimulationParameters");
    storeOnLevelString(os, level + 1, "tstart");
    storeOnLevelDouble(os, level + 2, tstart);
    storeOnLevelString(os, level + 1, "tend");
    storeOnLevelDouble(os, level + 2, tend);
    storeOnLevelString(os, level + 1, "hmin");
    storeOnLevelDouble(os, level + 2, hmin);
    storeOnLevelString(os, level + 1, "hmax");
    storeOnLevelDouble(os, level + 2, hmax);
    storeOnLevelString(os, level + 1, "hout");
    storeOnLevelDouble(os, level + 2, hout);
    storeOnLevelString(os, level + 1, "errorTol");
    storeOnLevelDouble(os, level + 2, errorTol);
}

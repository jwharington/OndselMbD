/***************************************************************************
 *   Copyright (c) 2023 Ondsel, Inc.                                       *
 *                                                                         *
 *   This file is part of OndselSolver.                                    *
 *                                                                         *
 *   See LICENSE file for details about copyright.                         *
 ***************************************************************************/
 
#include "ASMTPrincipalMassMarker.h"
#include <cassert>
#include "FullMatrix.h"

using namespace MbD;

std::shared_ptr<ASMTPrincipalMassMarker> ASMTPrincipalMassMarker::With()
{
    auto inst = std::make_shared<ASMTPrincipalMassMarker>();
    inst->initialize();
    return inst;
}

void ASMTPrincipalMassMarker::initialize()
{
    name = "MassMarker";
}

void ASMTPrincipalMassMarker::parseASMT(std::vector<std::string>& lines)
{
    size_t pos = lines[0].find_first_not_of("\t");
    auto leadingTabs = lines[0].substr(0, pos);
    assert(lines[0] == (leadingTabs + "Name"));
    safePopFront(lines);
    name = readStringNoSpacesOffTop(lines);
    assert(lines[0] == (leadingTabs + "Position3D"));
    safePopFront(lines);
    position3D = readColumnOfDoubles(lines[0]);
    safePopFront(lines);
    assert(lines[0] == (leadingTabs + "RotationMatrix"));
    safePopFront(lines);
    rotationMatrix = FullMatrix<double>::With(3);
    for (size_t i = 0; i < 3; i++)
    {
        auto row = readRowOfDoubles(lines[0]);
        rotationMatrix->atiput(i, row);
        safePopFront(lines);
    }
    assert(lines[0] == (leadingTabs + "Mass"));
    safePopFront(lines);
    mass = readDouble(lines[0]);
    safePopFront(lines);
    assert(lines[0] == (leadingTabs + "MomentOfInertias"));
    safePopFront(lines);
    momentOfInertias = DiagonalMatrix<double>::With(3);
    auto row = readRowOfDoubles(lines[0]);
    safePopFront(lines);
    for (size_t i = 0; i < 3; i++)
    {
        momentOfInertias->atiput(i, row->at(i));
    }
    assert(lines[0] == (leadingTabs + "Density"));
    safePopFront(lines);
    density = readDouble(lines[0]);
    safePopFront(lines);
}

void ASMTPrincipalMassMarker::setMass(double m)
{
    mass = m;
}

void ASMTPrincipalMassMarker::setDensity(double rho)
{
    density = rho;
}

void ASMTPrincipalMassMarker::setMomentOfInertias(DiagMatDsptr mat)
{
    momentOfInertias = mat;
}

// Overloads to simplify syntax.
void ASMTPrincipalMassMarker::setMomentOfInertias(double a, double b, double c)
{
    momentOfInertias = DiagonalMatrix<double>::With(ListD{ a, b, c });
}

void ASMTPrincipalMassMarker::storeOnLevel(std::ofstream& os, size_t level)
{
    storeOnLevelString(os, level, "PrincipalMassMarker");
    storeOnLevelString(os, level + 1, "Name");
    storeOnLevelString(os, level + 2, name);
    ASMTSpatialItem::storeOnLevel(os, level);
    storeOnLevelString(os, level + 1, "Mass");
    storeOnLevelDouble(os, level + 2, mass);
    storeOnLevelString(os, level + 1, "MomentOfInertias");
    storeOnLevelArray(os, level + 2, *momentOfInertias);
    storeOnLevelString(os, level + 1, "Density");
    storeOnLevelDouble(os, level + 2, density);
}

void ASMTPrincipalMassMarker::zeroMass()
{
    mass = 0.0;
    density = 0.0;
    momentOfInertias = DiagonalMatrix<double>::With(ListD{ 0.0, 0.0, 0.0 });
}

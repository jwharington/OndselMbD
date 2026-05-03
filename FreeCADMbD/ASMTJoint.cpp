/***************************************************************************
 *   Copyright (c) 2023 Ondsel, Inc.                                       *
 *                                                                         *
 *   This file is part of OndselSolver.                                    *
 *                                                                         *
 *   See LICENSE file for details about copyright.                         *
 ***************************************************************************/
#include <fstream>    

#include "ASMTJoint.h"
#include "JointIJ.h"
#include "System.h"

using namespace MbD;

namespace {
std::string jointTypeToken(const std::type_info& type)
{
    std::string token = type.name();

    // Handles both demangled names (e.g. "MbD::ASMTGearJoint") and
    // Itanium-mangled names (e.g. "N3MbD12ASMTGearJointE").
    auto asmtPos = token.find("ASMT");
    if (asmtPos != std::string::npos) {
        token = token.substr(asmtPos + 4); // drop ASMT prefix
    }

    auto scopePos = token.rfind("::");
    if (scopePos != std::string::npos) {
        token = token.substr(scopePos + 2);
    }

    // Itanium mangling can leave a trailing 'E' after the class token.
    if (!token.empty() && token.back() == 'E') {
        token.pop_back();
    }

    return token;
}
}

std::shared_ptr<ASMTJoint> ASMTJoint::With()
{
    auto inst = std::make_shared<ASMTJoint>();
    inst->initialize();
    return inst;
}

void ASMTJoint::readJointSeries(std::vector<std::string>& lines)
{
    std::string str = lines[0];
    std::string substr = "JointSeries";
    auto pos = str.find(substr);
    assert(pos != std::string::npos);
    str.erase(0, pos + substr.length());
    auto seriesName = readString(str);
    assert(fullName("") == seriesName);
    safePopFront(lines);
    readFXonIs(lines);
    readFYonIs(lines);
    readFZonIs(lines);
    readTXonIs(lines);
    readTYonIs(lines);
    readTZonIs(lines);
}

void ASMTJoint::storeOnLevel(std::ofstream& os, size_t level)
{
    auto jointType = jointTypeToken(typeid(*this));
    storeOnLevelString(os, level, jointType);
    storeOnLevelString(os, level + 1, "Name");
    storeOnLevelString(os, level + 2, name);
    ASMTItemIJ::storeOnLevel(os, level);
}

void ASMTJoint::storeOnTimeSeries(std::ofstream& os)
{
    os << jointTypeToken(typeid(*this)) << "Series\t" << fullName("") << std::endl;
    ASMTItemIJ::storeOnTimeSeries(os);
}

bool ASMTJoint::isJoint()
{
    return true;
}

void ASMTJoint::createMbD()
{
    ASMTConstraintSet::createMbD();
    auto mbdJt = std::static_pointer_cast<JointIJ>(mbdObject);
    mbdSys()->addJoint(mbdJt);
}

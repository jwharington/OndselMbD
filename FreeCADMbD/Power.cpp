/***************************************************************************
 *   Copyright (c) 2023 Ondsel, Inc.                                       *
 *                                                                         *
 *   This file is part of OndselSolver.                                    *
 *                                                                         *
 *   See LICENSE file for details about copyright.                         *
 ***************************************************************************/
 
#include "Power.h"
#include "Constant.h"
#include "Ln.h"
#include <cassert>
#include <algorithm>
#include "SimulationStoppingError.h"

using namespace MbD;

Power::Power(Symsptr bse, Symsptr ex) : FunctionXY(bse, ex)
{
}

std::shared_ptr<Power> Power::With()
{
    auto inst = std::make_shared<Power>();
    inst->initialize();
    return inst;
}

std::shared_ptr<Power> Power::With(Symsptr base, Symsptr exp)
{
    auto inst = std::make_shared<Power>(base, exp);
    inst->initialize();
    return inst;
}

Symsptr Power::differentiateWRTx()
{
    auto yminus1 = Symbolic::sum(y, sptrConstant(-1.0));
    auto power = Symbolic::raisedTo(x, yminus1);
    auto deriv = Symbolic::times(y, power);
    return deriv->simplified(deriv);
}

Symsptr Power::differentiateWRTy()
{
    auto lnterm = std::make_shared<Ln>(x);
    auto deriv = Symbolic::times(clonesptr(), lnterm);
    return deriv->simplified();
}

Symsptr Power::simplifyUntil(Symsptr sptr, std::shared_ptr<std::unordered_set<Symsptr>> set)
{
    auto itr = std::find_if(set->begin(), set->end(), [sptr](Symsptr sym) { return sptr.get() == sym.get(); });
    if (itr != set->end()) return sptr;

    auto newBase = x->simplifyUntil(x, set);
    auto newExp = y->simplifyUntil(y, set);

    if (newExp->isZero()) return sptrConstant(1.0);
    if (newExp->isOne()) return newBase;

    if (newBase->isConstant() && newExp->isConstant()) {
        return sptrConstant(std::pow(newBase->getValue(), newExp->getValue()));
    }

    return std::make_shared<Power>(newBase, newExp);
}

double Power::getValue()
{
    return std::pow(x->getValue(), y->getValue());
}

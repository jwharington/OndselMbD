/***************************************************************************
 *   Copyright (c) 2023 Ondsel, Inc.                                       *
 *                                                                         *
 *   This file is part of OndselSolver.                                    *
 *                                                                         *
 *   See LICENSE file for details about copyright.                         *
 ***************************************************************************/
 
#pragma once

#include <stdexcept>
#include <string>

namespace MbD {

    /**
     * Phase-0 diagnostic hook: when MBD_TRACE_STUBS is defined (or env
     * MBD_TRACE_STUBS=1), every "To be implemented." throw records its
     * call site and emits a report on process exit.
     *
     * Build with  -DMBD_TRACE_STUBS  or set env var at runtime.
     */
    class SimulationStoppingError : virtual public std::runtime_error
    {
    public:
        explicit SimulationStoppingError(const std::string& msg);

        /** Dump accumulated stub-hit table to stdout (or path in MBD_STUB_LOG). */
        static void dumpStubReport();
    };
}

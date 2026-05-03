/***************************************************************************
 *   Copyright (c) 2023 Ondsel, Inc.                                       *
 *                                                                         *
 *   This file is part of OndselSolver.                                    *
 *                                                                         *
 *   See LICENSE file for details about copyright.                         *
 ***************************************************************************/
/**
 * Phase-0 instrumentation: track every SimulationStoppingError("To be
 * implemented.") throw site.
 *
 * Activation:
 *   - Compile with -DMBD_TRACE_STUBS, or
 *   - Set environment variable MBD_TRACE_STUBS=1 at runtime.
 *
 * Output:
 *   - Emitted to stdout (or to the file named in env MBD_STUB_LOG).
 *   - Written on process exit via atexit handler, or by calling
 *     SimulationStoppingError::dumpStubReport() explicitly.
 *
 * Format (tab-separated):
 *   count  <TAB>  symbol  <TAB>  raw_address
 */

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <map>
#include <mutex>
#include <vector>
#include <sstream>
#include <string>

#if defined(__linux__) || defined(__APPLE__)
#  include <execinfo.h>
#  include <cxxabi.h>
#  define MBD_HAVE_BACKTRACE 1
#endif

#include "SimulationStoppingError.h"

using namespace MbD;

// ─── internal state ──────────────────────────────────────────────────────────

namespace {

constexpr int kMaxFrames = 32;
constexpr int kSkipFrames = 3; // skip: dumpStubReport / ctor / throw glue

struct StubHit {
    std::string symbol; // demangled or raw
    std::string address;
    int count = 0;
};

std::mutex g_mtx;
// key: raw symbol string (unique per call site)
std::map<std::string, StubHit> g_hits;
bool g_tracing = false;
bool g_initialized = false;

bool tracingEnabled()
{
    if (!g_initialized) {
        g_initialized = true;
#if defined(MBD_TRACE_STUBS)
        g_tracing = true;
#else
        const char* env = std::getenv("MBD_TRACE_STUBS");
        g_tracing = (env && (env[0] == '1' || env[0] == 'y' || env[0] == 'Y'));
#endif
    }
    return g_tracing;
}

std::string demangle(const char* sym)
{
#ifdef MBD_HAVE_BACKTRACE
    // sym looks like: ".../libFoo.so(mangled_name+0x42) [0xaddr]"
    // Extract mangled name between '(' and '+'
    std::string s(sym);
    auto lp = s.find('(');
    auto plus = s.find('+', lp);
    if (lp != std::string::npos && plus != std::string::npos && plus > lp + 1) {
        std::string mangled = s.substr(lp + 1, plus - lp - 1);
        int status = -1;
        char* demangled = abi::__cxa_demangle(mangled.c_str(), nullptr, nullptr, &status);
        if (status == 0 && demangled) {
            std::string result(demangled);
            free(demangled);
            return result;
        }
    }
#endif
    return sym;
}

void recordHit(const std::string& msg)
{
    if (!tracingEnabled()) return;
    if (msg != "To be implemented.") return;

#ifdef MBD_HAVE_BACKTRACE
    void* frames[kMaxFrames] = {};
    int n = backtrace(frames, kMaxFrames);
    char** syms = backtrace_symbols(frames, n);

    // Pick the first non-internal frame after the skip window
    int target = (n > kSkipFrames) ? kSkipFrames : (n > 0 ? n - 1 : 0);
    std::string key   = (syms && target < n) ? syms[target] : "unknown";
    std::string addr  = (syms && target < n) ? syms[target] : "unknown";
    std::string demangled = demangle(key.c_str());

    // Also capture one extra frame for context
    std::string ctx;
    if (syms && target + 1 < n) {
        ctx = demangle(syms[target + 1]);
    }

    if (syms) free(syms);

    std::lock_guard<std::mutex> lk(g_mtx);
    auto& hit = g_hits[demangled];
    hit.symbol  = demangled;
    hit.address = addr;
    hit.count++;
#else
    std::lock_guard<std::mutex> lk(g_mtx);
    auto& hit = g_hits["(no backtrace)"];
    hit.symbol = "(no backtrace)";
    hit.count++;
#endif
}

void dumpAtExit()
{
    SimulationStoppingError::dumpStubReport();
}

} // anonymous namespace

// ─── public API ──────────────────────────────────────────────────────────────

SimulationStoppingError::SimulationStoppingError(const std::string& msg)
    : std::runtime_error(msg)
{
    static bool atexitRegistered = false;
    if (!atexitRegistered) {
        atexitRegistered = true;
        std::atexit(dumpAtExit);
    }
    recordHit(msg);
}

void SimulationStoppingError::dumpStubReport()
{
    if (!tracingEnabled()) return;

    std::lock_guard<std::mutex> lk(g_mtx);
    if (g_hits.empty()) return;

    // Sort by count descending
    std::vector<StubHit> sorted;
    sorted.reserve(g_hits.size());
    for (auto& [k, v] : g_hits) sorted.push_back(v);
    std::sort(sorted.begin(), sorted.end(),
              [](const StubHit& a, const StubHit& b) { return a.count > b.count; });

    // Determine output stream
    std::ostream* out = &std::cout;
    std::ofstream fout;
    const char* logPath = std::getenv("MBD_STUB_LOG");
    if (logPath && logPath[0]) {
        fout.open(logPath, std::ios::app);
        if (fout.is_open()) out = &fout;
    }

    *out << "\n===== MbD STUB HIT REPORT =====\n";
    *out << "count\tsymbol\n";
    *out << "-----\t------\n";
    for (const auto& h : sorted) {
        *out << h.count << "\t" << h.symbol << "\n";
    }
    *out << "===== END STUB REPORT =====\n";
    out->flush();
}

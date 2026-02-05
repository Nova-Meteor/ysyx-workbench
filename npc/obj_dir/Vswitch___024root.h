// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vswitch.h for the primary calling header

#ifndef VERILATED_VSWITCH___024ROOT_H_
#define VERILATED_VSWITCH___024ROOT_H_  // guard

#include "verilated.h"


class Vswitch__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vswitch___024root final {
  public:

    // DESIGN SPECIFIC STATE
    VL_IN8(a,0,0);
    VL_IN8(b,0,0);
    VL_OUT8(f,0,0);
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __VstlPhaseResult;
    CData/*0:0*/ __VicoFirstIteration;
    CData/*0:0*/ __VicoPhaseResult;
    VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VicoTriggered;

    // INTERNAL VARIABLES
    Vswitch__Syms* vlSymsp;
    const char* vlNamep;

    // CONSTRUCTORS
    Vswitch___024root(Vswitch__Syms* symsp, const char* namep);
    ~Vswitch___024root();
    VL_UNCOPYABLE(Vswitch___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard

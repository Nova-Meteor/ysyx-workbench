// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vswitch.h for the primary calling header

#include "Vswitch__pch.h"

void Vswitch___024root___ctor_var_reset(Vswitch___024root* vlSelf);

Vswitch___024root::Vswitch___024root(Vswitch__Syms* symsp, const char* namep)
 {
    vlSymsp = symsp;
    vlNamep = strdup(namep);
    // Reset structure values
    Vswitch___024root___ctor_var_reset(this);
}

void Vswitch___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vswitch___024root::~Vswitch___024root() {
    VL_DO_DANGLING(std::free(const_cast<char*>(vlNamep)), vlNamep);
}

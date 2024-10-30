#include "Vtop.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "vbuddy.cpp"  // Insert Vbuddy code here
#include <iostream>
#include <chrono>
#include <thread>

int main(int argc, char **argv, char **env) {
    int i;
    int clk;

    Verilated::commandArgs(argc, argv);

    // Init top verilog instance
    Vtop* top = new Vtop;

    // Init trace dump
    Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC;
    top->trace(tfp, 99);
    tfp->open("top.vcd");

    // init Vbuddy
    if (vbdOpen()!=1) return(-1);
    vbdHeader("Lab 1: Counter");

    // Initialize simulation inputs
    top->clk = 1;
    top->rst = 1;
    top->en = 0;
    vbdSetMode(1);
    top->v = vbdValue();

    // Run simulation for many clock cycles
    for (i = 0; i < 300; i++) {

        // Dump variables into VCD file and toggle clock
        for (clk = 0; clk < 2; clk++) {
            tfp->dump(2 * i + clk);
            top->clk = !top->clk;
            top->eval();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Send count value to Vbuddy
        vbdHex(4, (int(top->bcd) >> 16) & 0xF);
        vbdHex(3, (int(top->bcd) >> 8) & 0xF);
        vbdHex(2, (int(top->bcd) >> 4) & 0xF);
        vbdHex(1, (int(top->bcd) & 0xF));
        vbdCycle(i + 1);

        // vbdPlot(int(top->count), 0, 255);


        // Change input stimuli
        top->rst = (i < 2) | (i == 15);
        top->en = vbdFlag();
        if (Verilated::gotFinish()) exit(0);
    }

    vbdClose();  // Housekeeping
    tfp->close();
    exit(0);
}

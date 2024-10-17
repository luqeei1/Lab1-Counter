#include "Vcounter.h"       // Include generated header for Vcounter module
#include "verilated.h"       // Verilator's main library
#include "verilated_vcd_c.h" // Verilator's VCD tracing library
#include "vbuddy.cpp"


int main(int argc, char **argv, char **env) {
    int i;
    int clk;

    Verilated::commandArgs(argc, argv); // Parse Verilator command-line arguments

    // Instantiate the counter module as Vcounter
    Vcounter* top = new Vcounter;

    // Enable trace dump
    Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC;
    top->trace(tfp, 99);  // Depth of tracing
    tfp->open("counter.vcd");  // Name of the VCD file

    if (vbdOpen() != 1) return(-1);
    vbdHeader("Lab1: Counter");

    // Initialize signals
    top->clk = 1;
    top->rst = 1;
    top->en = 0;

    // Run simulation for 300 clock cycles
    for (i = 0; i < 300; i++) {

        // Toggle clock and dump trace for each half cycle
        for (clk = 0; clk < 2; clk++) {
            tfp->dump(2 * i + clk);  // Dump variables into VCD file
            top->clk = !top->clk;    // Toggle clock
            top->eval();             // Evaluate model
        }

        vbdHex(4, (int(top->count) >> 16) & 0xF);
        vbdHex(3, (int(top->count) >> 8) & 0xF);
        vbdHex(2, (int(top->count) >> 4) & 0xF);
        vbdHex(1, (int(top->count) & 0xF));
        vbdCycle(i+1);

        // Modify reset and enable signals during the simulation
        top->rst = ( (i < 2) | (i==15));         // Reset for the first two cycles
        top->en = (i > 4);          // Enable after the fourth cycle

        if (Verilated::gotFinish()) exit(0); // Exit if simulation finishes
    }

    // Close the VCD file
    vbdClose();
    tfp->close();
    exit(0);
}

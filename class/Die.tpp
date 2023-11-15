#ifndef _DIE_TPP_
#define _DIE_TPP_

#include "FPGA.tpp"

class FPGA;

class Die {
private:
public:
    int DieId;
    FPGA* fpga;
    Die() : DieId(), fpga() {}
};

#endif

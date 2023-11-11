#ifndef _FPGA_TPP_
#define _FPGA_TPP_

#include <vector>
#include "Die.tpp"

class FPGA {
private:
public:
    int FPGAId;
    std::vector<Die*> Dies;
    FPGA() : FPGAId(), Dies() {}
};

#endif

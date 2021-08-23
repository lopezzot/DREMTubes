#include <stdint.h>

extern "C" {
    // argv -- unused
    // run_nr -- run number
    int32_t dreammon_init(char** argv, uint32_t run_nr, bool drs, int32_t drs_setup, bool phys_h);

    // doSingle -- flag on whether to do low-frequency updates
    // events   -- call counter
    // buf      -- event data
    int32_t dreammon_event(uint32_t doSingle, uint32_t events, uint32_t* buf, bool drs, int32_t drs_setup);

    // i -- unused
    int32_t dreammon_sync(uint32_t i, bool drs, int32_t drs_setup);
    int32_t dreammon_exit(uint32_t i, bool drs, int32_t drs_setup);
}

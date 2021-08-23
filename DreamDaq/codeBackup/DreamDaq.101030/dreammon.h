
extern "C" {
    // argv -- unused
    // run_nr -- run number
    int dreammon_init(char * * argv, unsigned int run_nr, bool drs, int drs_setup, bool phys_h);

    // doSingle -- flag on whether to do low-frequency updates
    // events   -- call counter
    // buf      -- event data
    int dreammon_event(unsigned int doSingle, unsigned int events, unsigned int * buf, bool drs, int drs_setup);

    // i -- unused
    int dreammon_exit(unsigned int i, bool drs, int drs_setup);
}

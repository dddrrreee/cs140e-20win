int main(int argc, char *argv[]) {
#if 0
    // make sure you can look these up using whatever method you have!
    assert(ent_lookup(root, "/console"));
    assert(ent_lookup(root, "/echo.cmd"));
    assert(ent_lookup(root, "/reboot.cmd"));
    assert(ent_lookup(root, "/run.cmd"));
    console = ent_lookup(root, "/console");
#endif

    // can comment this out to devlop without the pi initially.
    if(!fd_is_open(HANDOFF_FD))
        panic("pi-fs: must run with <HANDOFF_FD> as an open file descriptor\n");
    // if you want to toggle if using pi or not should have this as a global variable
    else
        using_pi_p = 1;

    // ping the pi til it echos something back: otherwise we could
    // start shoving stuff into its UART before its booted.
    output("about to check ready\n");
    while(1) {
        put_uint8(PI_READY);
        if(can_read_timeout(pi_fd, 10000)) {
            expect_op(PI_READY);
            break;
        }
    }
    output("about to call not main\n");
    return fuse_main(argc, argv, &pi_oper, 0);
}


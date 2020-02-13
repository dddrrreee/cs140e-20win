/*
 * engler, cs140e: low-level routines to support lexing.
 *
 * mainly, input, output, checkpoint/restore:
 *  1. code to read into circular buffer (lex_readin)
 *  2. get a character (lex_getc)
 *  3. take and restore checkpoints in input stream (lex_get_ckpt, lex_restore_ckpt)
 *  4. basic debugging by dumping all input (lex_dump).
 *
 * hopefully you should not have to modify this.  please let me know if 
 * broken.
 */
#include "libesp.h"

// read in more data.
int lex_readin(lex_t *l) {
    assert(lex_empty(l));
    assert(lex_nstream(l) == 0);

    // i think to get around hold and wait, have to allow timeout.
    for(int i = 0; !esp_has_data_timeout(l, 1000*10); i++)  {
        if(i % 100  == 0)
            output("%d: waiting on input from esp\n", i);
    }

    unsigned nbytes = 0; 
    do { 
        uint8_t buf[1024];
        int n;

        // returns number of bytes or -1.
        if((n = esp_read(l, buf, sizeof buf, 0)) <= 0)
            panic("server_read failed: %d\n", n);
//        cq_push_n(&l->stream, buf, n);
        lex_push_n(l,buf,n);
        nbytes += n;
        assert(lex_nstream(l) == nbytes);
    } while(esp_has_data_timeout(l, 0));

    return nbytes;
}

int lex_has_data(lex_t *l) {
    return lex_nstream(l) || esp_has_data_timeout(l, 0);
}

void lex_dump(lex_t *l) {
    output("all unconsumed input currently buffered from ESP:<\n");
    lex_dump_str(l);
    output(">\n");
}

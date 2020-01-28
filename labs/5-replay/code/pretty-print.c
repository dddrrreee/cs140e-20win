/*
 * engler, cs140e: code to parse a log produced by our interpose logging program.
 *
 * i'd say one of the laws of program checking is that the code to explain an error 
 * is often > 80% of the code, and the actual check itself is < 20%.  e.g., for 
 * us, our error check is roughly a trace + strcmp of output.
 *
 * this code is merely to make it easier to figure out what happened.
 *
 * all this does is read in a log.  there's more code than what its actual complexity.
 *
 * if we built more semantics into 'interpose'  or went with a straight-up binary
 * representation then this would be much much simpler.
 *
 * but, it's useful to have a general purpose tracing program, and after decades of
 * dealing w/ input/output i'm a big believer in human-readable.
 *
 * but, of course, tastes differ.
 * 
 * use this to just read in the different records.  then on top of this can parse
 * the program.
 */
#include <assert.h>
#include <ctype.h>
#include "shared-code/simple-boot.h"
#include "libunix.h"

typedef struct {
    uint8_t *log;
    unsigned nbytes;
    unsigned off;
    unsigned lineno;
} log_t;

enum { PI_OUTPUT = 'p', UNIX_OUTPUT='u' };

#define err(_l, args...) do { output("parse error at line=%d:", (_l)->lineno); die(args); } while(0)


/*
   recall from lab 2-bootloader:

    send a string <msg> to the unix side to print.  
    message format:
        [PRINT_STRING, strlen(msg), <msg>]

     =======================================================
             pi side             |               unix side
     -------------------------------------------------------
      put_uint(GET_PROG_INFO)+ ----->

                                      put_uint(PUT_PROG_INFO);
                                      put_uint(ARMBASE);
                                      put_uint(nbytes);
                             <------- put_uint(crc32(code));

      put_uint32(GET_CODE)
      put_uint32(crc32)      ------->
                                      <check crc = the crc value sent>
                                      put_uint(PUT_CODE);
                                      foreach b in code
                                           put_byte(b);
                              <-------
     <copy code to addr>
     <check code crc32>
     put_uint(BOOT_SUCCESS)
                              ------->
                                       <done!>
                                       start echoing any pi output to
                                       the terminal.
     =======================================================
*/


/*
  check if op is legal, returns string if so or fails if not.

  from simple-boot.h:

    // the weird numbers are to try to help with debugging
    // when you drop a byte, flip them, corrupt one, etc.
    BOOT_START      = 0xFFFF0000,

    GET_PROG_INFO   = 0x11112222,       // pi sends
    PUT_PROG_INFO   = 0x33334444,       // unix sends

    GET_CODE        = 0x55556666,       // pi sends
    PUT_CODE        = 0x77778888,       // unix sends

    BOOT_SUCCESS    = 0x9999AAAA,       // pi sends on success
    BOOT_ERROR      = 0xBBBBCCCC,       // pi sends on failure.

    PRINT_STRING    = 0xDDDDEEEE,       // pi sends to print a string.

    // error codes from the pi to unix
    BAD_CODE_ADDR   = 0xdeadbeef,
    BAD_CODE_CKSUM  = 0xfeedface,
 */
const char * op_check(uint32_t op) {
    switch(op) {
    case BOOT_START:    return "BOOT_START";
    case GET_PROG_INFO: return "GET_PROG_INFO";
    case PUT_PROG_INFO: return "PUT_PROG_INFO";
    case GET_CODE:      return "GET_CODE";
    case PUT_CODE:      return "PUT_CODE";
    case BOOT_SUCCESS:  return "BOOT_SUCCESS";
    case BOOT_ERROR:    return "BOOT_ERROR";
    case PRINT_STRING:  return "PRINT_STRING";
    case BAD_CODE_ADDR: return "BAD_CODE_ADDR";
    case BAD_CODE_CKSUM:return "BAD_CODE_CKSUM";
    default: 
        panic("invalid op=<%d>\n", op); 
        return 0;
    }
}

log_t log_readin(const char *name) {
    log_t l = {0};
    if(!(l.log = read_file(&l.nbytes, name)))
        die("cannot open <%s>\n", name);
    l.lineno = 1;
    return l;
}

// end of log
static int log_is_eol(log_t *l) {
    return (l->off == l->nbytes);
}
// get a single character.
uint8_t log_getc(log_t *l) {
    assert(l->off < l->nbytes); 
    uint8_t c = l->log[l->off++];
    return c;
}
static uint8_t log_peek(log_t *l) {
    assert(!log_is_eol(l));
    return l->log[l->off];
}





// checking if a single char matches what we expect.
static void exp_tok(log_t *l, uint8_t exp) {
    uint8_t got = log_getc(l);
    if(exp != got)
        err(l,"value mismatch, expected '%c', have '%c' (%d)\n", exp, got, got);
}

// convert a hex character to int value.
static int hex2int(log_t *l, char ch) {
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    err(l,"invalid char '%x', expected hex!\n", ch);
}

// read in a hex number.
uint8_t log_get_hex(log_t *l) {
    exp_tok(l,'0');
    exp_tok(l,'x');
    unsigned d1 = log_getc(l);
    unsigned d0 = log_getc(l);
    unsigned v = hex2int(l,d1) * 16 + hex2int(l,d0);
    assert(v <= 255);
    return v;
}

// eat the rest of the line up to a newline.
static void eat_line(log_t *l) {
    uint8_t c;

    // can have either a new-line terminated comment
    int in_comment_p = 0;
    while((c = log_getc(l)) != '\n') {
        if(c == '#')  
            in_comment_p = 1;
        if(!in_comment_p && !isspace(c))
            err(l,"invalid character '%c', expected space or '#'\n", c);
    } while(c != '\n');
    l->lineno++;
}

int log_read_ent(log_t *l, uint8_t *op, uint8_t *v) {
    if(log_is_eol(l))
        return 0;

    *op = log_getc(l);
    exp_tok(l, ',');
    *v = log_get_hex(l);
    eat_line(l);
    return 1;
}

int log_readline(log_t *l, uint8_t *op, uint8_t *v) {
    return log_read_ent(l,op,v);
}

typedef struct {
    uint8_t sender;
    uint8_t v;
} log_ent_t;

log_ent_t *log_parse_tuples(const char *name, unsigned *n) {
    log_t l = log_readin(name);

    // overallocate
    log_ent_t *e = calloc(l.nbytes+1, sizeof *e);
    int i;
    for(i = 0; log_readline(&l,&e[i].sender, &e[i].v); i++)
        assert(i <= l.nbytes);
    *n = i;
    return e;
}


// expect op
static uint8_t read_ent_op(log_t *l, uint8_t op_exp) {
    uint8_t op, v;
    log_read_ent(l, &op, &v);
    if(op_exp != op)
        err(l,"unexpected op: exp '%c' have '%c'\n", op_exp, op);
    return v;
}

// read in 32 bits with a known op.
int log_read_ent32(log_t *l, uint8_t op, uint32_t *v) {
    if(log_is_eol(l))
        return 0;
    uint8_t v0,v1,v2,v3;
    v0 = read_ent_op(l, op);
    v1 = read_ent_op(l, op);
    v2 = read_ent_op(l, op);
    v3 = read_ent_op(l, op);
    *v = v3 << 24 | v2 << 16 | v1 << 8 | v0;
    return 1;
}

uint32_t log_ent32_exp(log_t *l, uint8_t op) {
    uint32_t v;
    if(!log_read_ent32(l,op, &v))
        err(l,"unexpected end of file\n");
    return v;
}

uint32_t log_peek32(log_t *l, uint8_t op) {
    log_t ckpt = *l;
    uint32_t v =  log_ent32_exp(l,op);
    *l = ckpt;
    return v;
}

// read in 32 bits with an unknown op
int log_read32(log_t *l, uint8_t *op, uint32_t *v) {
    if(log_is_eol(l))
        return 0;
    *op = log_peek(l);
    return log_read_ent32(l, *op, v);
}

// read in all entries that have <op> and return buffer containing the values. 
uint8_t * log_readN(log_t *l, uint8_t op, uint32_t n) {
    uint8_t op1;
    uint8_t *p = calloc(n + 1, 1);
    for(int i = 0; i < n; i++) {
        if(!log_read_ent(l, &op1, &p[i]))
            panic("impossible\n");
        if(op != op1)
            panic("impossible\n");
    }
    return p;
}

uint8_t *log_readall(log_t *l, uint8_t op, uint32_t *nbytes) {
    log_t ckpt = *l;
    
    unsigned n = 0;
    uint8_t op1,v;
    for(n = 0; ; n++) {
        if(!log_read_ent(l, &op1, &v))
            break;
        if(op != op1)
            panic("impossible\n");
    }
    if(!log_is_eol(l))
        err(l,"pi output did not consume log??\n");

    *l = ckpt;
    *nbytes = n;
    return log_readN(l, op, n);
}

/*
 * for reference from the lab:

    send a string <msg> to the unix side to print.  
    message format:
        [PRINT_STRING, strlen(msg), <msg>]

     =======================================================
             pi side             |               unix side
     -------------------------------------------------------
      put_uint(GET_PROG_INFO)+ ----->

                                      put_uint(PUT_PROG_INFO);
                                      put_uint(ARMBASE);
                                      put_uint(nbytes);
                             <------- put_uint(crc32(code));

      put_uint32(GET_CODE)
      put_uint32(crc32)      ------->
                                      <check crc = the crc value sent>
                                      put_uint(PUT_CODE);
                                      foreach b in code
                                           put_byte(b);
                              <-------
     <copy code to addr>
     <check code crc32>
     put_uint(BOOT_SUCCESS)
                              ------->
                                       <done!>
                                       start echoing any pi output to
                                       the terminal.
     =======================================================
*/
enum { BAD_DATA = 1} ;
struct msg {
    uint32_t op;
    int sender;

    union msg_types { 
        uint8_t bad_data;

        struct boot_success {
            uint8_t *output;
            uint32_t nbytes;
        } boot_success;
        struct  prog_info {
            uint32_t armbase, nbytes, crc32;
        } prog_info;
    
        struct get_code {
            uint32_t crc32;
        } get_code;
    
        // ugh ugh ugh.  should have included a size!!  this sucks.
        struct put_code {
            uint8_t *code;
            uint32_t nbytes; // should we check?
        } put_code;
    
        struct print_string {
            uint32_t size;
            uint8_t *string;
        } print_string;
    } u;
};

static void pretty_print_op(struct msg *m) {
    const char *op_s =  op_check(m->op);
    
    const char *pre = "";
    if(m->sender == UNIX_OUTPUT)
        pre = "                                     ";
    output("%s%s/%x:\n", pre,op_s, m->op);
    switch(m->op) {
    // empty messages
    case BAD_DATA:
        output("    %x", m->u.bad_data);
        break;
    case GET_PROG_INFO: 
        break;
    case BOOT_ERROR:   
    case BAD_CODE_ADDR:
    case BAD_CODE_CKSUM:
        output("    is an error\n");
        break;
    case GET_CODE:   
        output("    crc32=0x%x\n", m->u.get_code.crc32);
        break;
    case BOOT_SUCCESS: 
        output("output follows:\n%s\n", m->u.boot_success.output);
        break;
    case PUT_PROG_INFO: {
        struct prog_info *p = &m->u.prog_info;
        output("    %sarmbase=0x%x\n", pre,p->armbase);
        output("    %snbytes=%d\n", pre,p->nbytes);
        output("    %scrc32=0x%x\n", pre,p->crc32);
        break;
    }
    case PUT_CODE:      
        output("%s    code= <...%d nbytes...>\n", pre,m->u.put_code.nbytes);
        break;
    case PRINT_STRING: 
        panic("not handling\n");
    default:
        panic("impossible\n");
    }
    if(m->sender == PI_OUTPUT)
        output("                        --------->\n");
    else
        output("                        <---------\n");
}
    
void pretty_print(struct msg *m, unsigned n) {
    output("----------------------------------------------\n");
    output("       PI                            UNIX\n");
    for(int i = 0; i < n; i++) { 
        //output("%d:", i);
        pretty_print_op(&m[i]);
    }
    output("----------------------------------------------\n");
}

// we could have this code raise the log except that the code and any 
// string for print will not be a 32-bit int.
struct msg *parse_trace(log_t *l, unsigned *n) {
    uint8_t op;
    uint32_t v;

    // way over-allocate so we don't have to think.
    struct msg *m = calloc(l->nbytes, sizeof *m);
    

    /*
     * empty messages:
     */

    uint32_t msg_op;

    // first run of bytes can be bad data from previous connection.
    int i = 0;
    while(log_peek32(l, PI_OUTPUT) != GET_PROG_INFO) {
        m[i].op = BAD_DATA;
        m[i].sender = PI_OUTPUT;
        uint8_t x = m[i].u.bad_data = read_ent_op(l, PI_OUTPUT);
        output("%d: bad data: 0x%x\n", l->lineno, x);
        i++;
    }

    uint32_t nbytes = 0;
    for(i = 0; log_read32(l,&op,&msg_op); i++) {
        const char *op_s =  op_check(msg_op);
        output("%d: checking op <%s/%x>\n", i, op_s, msg_op);
        m[i].op = msg_op;

        switch(msg_op) {
        // empty messages
        case GET_PROG_INFO: 
        case BOOT_ERROR:   
        case BAD_CODE_ADDR:
        case BAD_CODE_CKSUM:
            m[i].sender = PI_OUTPUT;
            break;
        case GET_CODE:   
            m[i].sender = PI_OUTPUT;
            m[i].u.get_code.crc32 = log_ent32_exp(l,PI_OUTPUT);
            break;

        case BOOT_SUCCESS: 
        {
            // the rest is just output.
            m[i].sender = PI_OUTPUT;
            struct boot_success *s = &m[i].u.boot_success;

            s->output = log_readall(l, PI_OUTPUT, &s->nbytes);
            break;
        }

        case PUT_PROG_INFO: 
        {
            m[i].sender = UNIX_OUTPUT;
            struct prog_info *p = &m[i].u.prog_info;

            p->armbase = log_ent32_exp(l,UNIX_OUTPUT);
            p->nbytes = log_ent32_exp(l,UNIX_OUTPUT);
            p->crc32 = log_ent32_exp(l,UNIX_OUTPUT);
            if(nbytes)
                assert(nbytes == p->nbytes);
            else
                nbytes = p->nbytes;
            break;
        }
        case PUT_CODE:      
            assert(nbytes);
            m[i].sender = UNIX_OUTPUT;
            m[i].u.put_code.code = log_readN(l, UNIX_OUTPUT, nbytes);
            m[i].u.put_code.nbytes = nbytes;
            break;
        case PRINT_STRING: panic("not handling\n");
        default: panic("impossible\n");
        }
    }
    *n = i;
    return m;
}

// check that what we read in is what we parsed.
static void check_parse(const char *input, log_t *l) {
    log_t ckpt = *l;

    const char *tmp_file = "/tmp/check.log.txt";
    FILE *fp = fopen(tmp_file, "w+");
    demand(fp, "could not create file\n");
    
    uint8_t op,v;
    while(log_read_ent(l, &op, &v)) 
        fprintf(fp, "%c,0x%.2x\n", op,v);
    fclose(fp);

    run_system("diff %s %s", input, tmp_file);
    *l = ckpt;
}

// we are not handling print right now.
int main(int argc, char *argv[]) {
    if(argc != 2)
        die("expected 2 arguments, have %d\n", argc);

    log_t l = log_readin(argv[1]);
    uint8_t op,v;

    check_parse(argv[1], &l);
    check_parse(argv[1], &l);       // make sure we can to 2x.

    unsigned n;
    struct msg *m = parse_trace(&l, &n);
    pretty_print(m,n);
    return 0;
}

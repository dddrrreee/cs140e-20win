typedef struct {
  unsigned read;
  unsigned padding[3];
  unsigned peek;
  unsigned sender;
  unsigned status;
  unsigned configuration;
  unsigned write;
} mailbox_t;

#define MAILBOX_EMPTY  (1<<30)
#define MAILBOX_FULL   (1<<31)

static mailbox_t *mbox =  (void*)0x2000B880;

void write_mailbox(volatile void *data, unsigned channel) {
    while(mbox->status & MAILBOX_FULL)
        ;
    mbox->write = ((unsigned)(data) | channel | 0x40000000);
}

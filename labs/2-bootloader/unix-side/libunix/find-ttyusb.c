// engler, cs140e.
#include <assert.h>
#include <fcntl.h>
#include <string.h>

#include "libunix.h"

#define _SVID_SOURCE
#include <dirent.h>
static const char *ttyusb_prefixes[] = {
	"ttyUSB",	// linux
	"cu.SLAB_USB", // mac os
	0
};

static int filter(const struct dirent *d) {
    // scan through the prefixes, returning 1 when you find a match.
    // 0 if there is no match.
    return 0 == strncmp(ttyusb_prefixes[1], d->d_name, strlen(ttyusb_prefixes[1]));
}

// find the TTY-usb device (if any) by using <scandir> to search for
// a device with a prefix given by <ttyusb_prefixes> in /dev
// returns:
//  - device name.
// panic's if 0 or more than 1.
//
char *find_ttyusb(void) {
    char *p;

    // use <alphasort> in <scandir>
    // return a malloc'd name so doesn't corrupt.
    // CP2102 enumerates in /dev
    const char* dev_path = "/dev/";
    
    // scandir will take care of memory allocation; just need to free
    struct dirent** scan_buf;
    
    // use scandir to find the directory
    int status = scandir(dev_path, &scan_buf, &filter, alphasort);
    
    // check status returned by scandir
    if(status < 0) {
		die("Some other error has occurred");
	} else if(status == 0) {
        die("No CP2102 bridges found");
    } else if (status > 1){
        die("Multiple CP2102 bridges found");
    } else {
		;
	}
    
    // there should only be one directory returned
    //int fd = open((scan_buf[0])->d_name, O_RDWR);
    
    // write full path to serial bridge
	//output("%s\n", scan_buf[0]->d_name);
	// Stack allocate, sprintf, and then strdup	
	//p = (char*) malloc(strlen(dev_path) + 1 + strlen(scan_buf[0]->d_name) + 1);
    //strcpy(p, dev_path);
	//strcat(p, scan_buf[0]->d_name);
    
	char buf [1024];
	snprintf(buf, 1024, "%s%s", dev_path,scan_buf[0]->d_name); 
    return strdup(buf);
}

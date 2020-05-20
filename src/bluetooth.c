#define _DEFAULT_SOURCE

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <threads.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include "bluetooth.h"

mtx_t  mtx_scan_bluetooth;
thrd_t thr_bluetooth;


int scan_bluetooth_thread (void *args)
{
    const int max_rsp = 255;
    const int len = 8;
    int num_rsp;
    int dev_id, sock;
    int i;
    inquiry_info *ii;
    char addr[19] = { 0 };
    char name[248] = { 0 };

    dev_id = hci_get_route(NULL);
    sock = hci_open_dev( dev_id );
    if (dev_id < 0 || sock < 0) {
        perror("opening socket");
        return 0;
    }

  printf("Scanning bluetooth...\n");
  mtx_lock(&mtx_scan_bluetooth);
    ii = (inquiry_info*) malloc( max_rsp * sizeof(inquiry_info));
    num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, IREQ_CACHE_FLUSH);
    if( num_rsp < 0 ) perror("hci_inquiry");

    for (i = 0; i < num_rsp; i++) {
      ba2str(&(ii+i)->bdaddr, addr);
      memset(name, 0, sizeof(name));
      if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), name, 0) < 0)
        strcpy(name, "[unknown]");
      printf("%s  %s\n", addr, name);
    }

    close( sock );
    mtx_unlock(&mtx_scan_bluetooth);
    return num_rsp;
}


void launch_scan_bluetooth()
{
  mtx_init(&mtx_scan_bluetooth, mtx_plain);
  thrd_create( &thr_bluetooth, scan_bluetooth_thread, NULL);
}


bool is_bluetooth_scan_finished()
{
  if ( mtx_trylock(&mtx_scan_bluetooth) == thrd_success ) {
    mtx_unlock(&mtx_scan_bluetooth);
    return true;
  } else {
    return false;
  };  
}


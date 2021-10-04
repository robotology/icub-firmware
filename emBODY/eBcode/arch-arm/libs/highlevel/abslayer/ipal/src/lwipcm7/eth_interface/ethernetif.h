#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__

#include "lwip/netif.h"

err_t ethernetif_init(struct netif *netif);
void  ethernetif_poll(struct netif *netif);
void  ethernetif_check_link (struct netif *netif);

#endif

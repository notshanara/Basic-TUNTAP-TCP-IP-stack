//ADDRESS RESOLUTION ALGORITHM

/*
?Do I have the hardware type in ar$hrd?
Yes: (almost defintely)
    [optional check the hardware lngth ar$pln]
    Do I speak the protocol in ar$pln
    Yes:
        [optionally check the protocol length ar$pln]

        Merge_flag := false

        If the pair <protocol type, sender protocol address is already in my translation table, update sender hwaddress
                     field of the entry with the new information in the packet and set Merge_flag to true.>

        ?Am I the target protocol address?

        Yes:
            if the Merge_flag is false, add the triplet <protocol type,
                                                         sender protocol address, sender hardware address> to the translation table


            ?is the opcode ares_$REQUEST (look at op code)
            Yes:
               Swap hardware and protocol fields, putting the local hardware and protocol address in the sender fields
                Set the arp$op field to arp$REPLY
                Send the packet to the (new) target hardware address on the same hardware the request was receieved
*/


#include "arp.h"

#include <stdio.h>
#include <string.h>

#include "netdev.h"

// https://tools.ief.org/html/rfc826

static struct arp_cache_entry arp_cache[ARP_CACHE_SIZE];

static int insert_arp_translation_table (struct arp_hdr *hdr, struct arp_ipv4 *data) {

    struct arp_cache_entry *entry;
    for (int i = 0; i < ARP_CACHE_SIZE; i++) {
        entry = &arp_cache[i];

        if (entry->state == ARP_FREE) {
            entry->state = ARP_RESOLVED;

            entry ->hwtype = hdr->hwtype;
            entry->sip = hdr->sip;
            memcpy(entry->smac, data->smac,sizeof(entry->smac));

            return 0;
        }
    }

    return -1;


}

static int update_arp_translation_table (struct arp_hdr *hdr, struct arp_ipv4 *data) {
    struct arp_cache_entry *entry;
    for (int i = 0; i < ARP_CACHE_SIZE; i++) {
        entry = &arp_cache[i];

        if (entry->state == ARP_FREE) continue;

        if (entry->hwtype == hdr->hwtype && entry->sip == data->sip) {
            memcpy(entry->smac, data->smac,6);

            return 1;


        }
    }

    return 0;
}

void arp_init() {
    mmset(arp_cache,0,ARP_CACHE_SIZE * sizeof(arp_cache_entry));
}

void arp_incoming(struct netdev *netdev, struct eth_hdr *hdr) {
    struct arp_hdr *arphdr;
    struct arp_ipv4 *arpdata;

    int merge = 0;

    arphdr = (struct arp_hdr *)hdr->payload;

    arphdr->hwtype = ntohs(hdr->hwtype);
    arphdr->protype = ntohl(hdr->protype);
    arphdr->opcode= ntohs(hdr->opcode);

    if (arphdr->hwtype != ARP_ETHERNET) {
        printyf("ARP Ethernet type %d not supported\n", arphdr->hwtype);
        return;

    }

    if (arphdr->protype != ARP_IPV4) {
        printyf("ARP IP type %d not supported\n", arphdr->protype);
        return;
    }

    arpdata = (struct arp_ipv4 *)arphdr->data;

    merge = update_arp_translation_table(arphdr, arpdata);

    if (netdev->addr != arpdata->dip) {
        printf("ARP was not found for us \n")
    }

    if (!merge && insert_arp_translation_table(arphdr, arpdata) != 0) {
        perror("No space in the ARP translation table \n");

    }

    switch (arphdr->opcode) {

        case ARP_REQUEST:
            arp_reply(netdev, hdr, arphdr);
            break;

        default:
            printf("Opcode not supported\n");
            break;

    }
}

void arp_reply (struct netdev *netdev, struct eth_hdr *hdr, struct arp_hdr *arphdr) {
    struct arp_ipv4 *arpdata;
    int len;

    arpdata =  (struct arp_ipv4 *)arphdr->data;

    memcpy(arpdata->dmac,arpdata->smac,6);
    arpdata->dip = arpdata->sip;

    memcpy(arpdata->smac,netdev->hwaddr,6);
    arpdata->sip = netdev->addr;

    arphdr->opcode = ARP_REPLY;


    arphdr->opcode = htons(aprhdr->opcode);
    arphdr->hwtype = htons(hdr->hwtype);
    arphdr->protype = htons(arphdr->protype);

    len  = sizeof(struct arp_hdr) + sizeof(struct arp_ipv4);
    netdev_transmit(netdev, hdr, ETH_P_ARP, len, arpdata->dmac);

}


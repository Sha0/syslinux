/*
 * TFTP data output backend
 */

#include <string.h>
#include <syslinux/pxe.h>
#include <syslinux/config.h>
#include <netinet/in.h>
#include <sys/times.h>

#include "backend.h"

enum tftp_opcode {
    TFTP_RRQ	= 1,
    TFTP_WRQ	= 2,
    TFTP_DATA	= 3,
    TFTP_ACK	= 4,
    TFTP_ERROR	= 5,
};

struct tftp_state {
    uint32_t my_ip;
    uint32_t srv_ip;
    uint16_t my_port;
    uint16_t srv_port;
    uint16_t seq;
};

static int send_ack_packet(struct tftp_state *tftp,
			   const void *pkt, size_t len)
{
    com32sys_t ireg, oreg;
    t_PXENV_UDP_WRITE *uw = __com32.cs_bounce;
    t_PXENV_UDP_READ  *ur = __com32.cs_bounce;
    clock_t start;
    static const clock_t timeouts[] = {
	2, 2, 3, 3, 4, 5, 6, 7, 9, 10, 12, 15, 18, 21, 26, 31,
	37, 44, 53, 64, 77, 92, 110, 132, 159, 191, 229, 0
    };
    const clock_t *timeout;

    memset(&ireg, 0, sizeof ireg);
    ireg.eax.w[0] = 0x0009;

    for (timeout = timeouts ; *timeout ; timeout++) {
	memset(uw, 0, sizeof uw);
	memcpy(uw+1, pkt, len);
	uw->ip = tftp->srv_ip;
	uw->src_port = tftp->my_port;
	uw->dst_port = tftp->srv_port ? tftp->srv_port : htons(69);
	uw->buffer_size = len;
	uw->buffer = FAR_PTR(uw+1);

	ireg.ebx.w[0] = PXENV_UDP_WRITE;
	ireg.es = SEG(uw);
	ireg.edi.w[0] = OFFS(uw);

	__intcall(0x22, &ireg, &oreg);

	start = times(NULL);

	do {
	    memset(ur, 0, sizeof ur);
	    ur->src_ip = tftp->srv_ip;
	    ur->dest_ip = tftp->my_ip;
	    ur->s_port = tftp->srv_port;
	    ur->d_port = tftp->my_port;
	    ur->buffer_size = __com32.cs_bounce_size - sizeof *ur;
	    ur->buffer = FAR_PTR(ur+1);

	    ireg.ebx.w[0] = PXENV_UDP_READ;
	    ireg.es = SEG(ur);
	    ireg.edi.w[0] = OFFS(ur);
	    __intcall(0x22, &ireg, &oreg);

	    if (!(oreg.eflags.l & EFLAGS_CF) &&
		ur->status == PXENV_STATUS_SUCCESS &&
		tftp->srv_ip == ur->src_ip &&
		(tftp->srv_port == 0 ||
		 tftp->srv_port == ur->s_port)) {
		uint16_t *xb = (uint16_t *)(ur+1);
		if (ntohs(xb[0]) == TFTP_ACK &&
		    ntohs(xb[1]) == tftp->seq) {
		    tftp->srv_port = ur->s_port;
		    return 0;		/* All good! */
		} else if (ntohs(xb[1]) == TFTP_ERROR) {
		    return -1;		/* All bad! */
		}
	    }
	} while ((clock_t)(times(NULL) - start) < *timeout);
    }

    return -1;			/* No success... */
}

static int be_tftp_write(struct backend *be)
{
    static uint16_t local_port = 0x4000;
    struct tftp_state tftp;
    char buffer[512+4+6];
    int nlen;
    const union syslinux_derivative_info *sdi =
	syslinux_derivative_info();
    const char *data = be->outbuf;
    size_t len = be->zbytes;
    size_t chunk;

    tftp.my_ip    = sdi->pxe.myip;
    tftp.my_port  = htons(local_port++);
    tftp.srv_ip   = pxe_dns(be->argv[1]);
    tftp.srv_port = 0;
    tftp.seq      = 0;

    buffer[0] = 0;
    buffer[1] = TFTP_WRQ;
    nlen = strlcpy(buffer+2, be->argv[0], 512);
    memcpy(buffer+3+nlen, "octet", 6);

    if (send_ack_packet(&tftp, buffer, 2+nlen+1+6))
	return -1;

    do {
	chunk = len >= 512 ? 512 : len;

	buffer[1] = TFTP_DATA;
	*((uint16_t *)(buffer+2)) = htons(++tftp.seq);
	memcpy(buffer+4, data, chunk);
	data += chunk;
	len -= chunk;

	if (send_ack_packet(&tftp, buffer, chunk+4))
	    return -1;
    } while (chunk == 512);

    return 0;
}

struct backend be_tftp = {
    .name       = "tftp",
    .helpmsg    = "filename tftp_server",
    .minargs    = 2,
    .write      = be_tftp_write,
};

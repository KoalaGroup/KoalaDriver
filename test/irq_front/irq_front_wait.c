#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <sis1100_var.h>

/****************************************************************************/
int
main(int argc, char* argv[])
{
    int p, count=10;
    struct sis1100_irq_ctl irqctl;
    struct sis1100_irq_get irqget;
    struct sis1100_irq_ack irqack;

    if (argc!=2) {
        printf("usage: %s path\n", argv[0]);
        return 1;
    }

    if ((p=open(argv[1], O_RDWR, 0))<0) {
        printf("open %s: %s\n", argv[1], strerror(errno));
        return 2;
    }

    irqctl.irq_mask=SIS3100_FRONT_IRQS;
    irqctl.signal=-1; /* activated, but no signal */
    if (ioctl(p, SIS1100_IRQ_CTL, &irqctl)<0) {
        printf("ioctl(SIS1100_IRQ_CTL): %s\n", strerror(errno));
        return 3;    
    }

    while (count--) {
        irqget.irq_mask=SIS3100_FRONT_IRQS; /* or just 0xffffffff */
        if (ioctl(p, SIS1100_IRQ_WAIT, &irqget)<0) {
            printf("ioctl(SIS1100_IRQ_GET): %s\n", strerror(errno));
            return 1;
        }
        if (irqget.remote_status) {
            if (irqget.remote_status<0)
                printf("Link down\n");
            else
                printf("Link up\n");
        }
        printf("changed front inputs: 0x%08x\n", irqget.irqs);

        irqack.irq_mask=irqget.irqs;
        if (ioctl(p, SIS1100_IRQ_ACK, &irqack)<0) {
            printf("ioctl(SIS1100_IRQ_ACK): %s\n", strerror(errno));
            return 1;
        }
    }

    /* disable IRQs */
    irqctl.irq_mask=SIS3100_FRONT_IRQS; /* or 0xffffffff */
    irqctl.signal=0;
    if (ioctl(p, SIS1100_IRQ_CTL, &irqctl)<0) {
        printf("ioctl(SIS1100_IRQ_CTL): %s\n", strerror(errno));
        return 3;    
    }

    close(p);
    return 0;    
}
/****************************************************************************/
/****************************************************************************/

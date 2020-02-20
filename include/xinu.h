/* xinu.h - include all system header files */
#ifndef _XINU_H
#define _XINU_H
#include <stdarg.h>
#include <kernel.h>
#include <stddef.h>
#include <conf.h>
#include <process.h>
#include <queue.h>
#include <resched.h>
#include <semaphore.h>
#include <future.h>
#include <myqueue.h>
#include <memory.h>
#include <bufpool.h>
#include <clock.h>
#include <mark.h>
#include <ports.h>
#include <uart.h>
#include <tty.h>
#include <device.h>
#include <interrupt.h>
#include <file.h>
#include <rfilesys.h>
#include <rdisksys.h>
#include <lfilesys.h>
#include <ether.h>
#include <net.h>
#include <ip.h>
#include <arp.h>
#include <udp.h>
#include <dhcp.h>
#include <icmp.h>
#include <tftp.h>
#include <name.h>
#include <shell.h>
#include <date.h>
#include <prototypes.h>
#include <delay.h>
#include <stdio.h>
#include <string.h>

#ifdef GPIO
#include <gpio.h>
#endif

#if defined(ARM_QEMU) || defined(ARM_BBB)

#include <armv7a.h>

#endif

#ifdef ARM_QEMU

#include <platform.h>
struct platform platform;

#endif

#ifdef ARM_BBB

#include <am335x_control.h>
#include <am335x_eth.h>

#endif

#if defined(X86_QEMU) || defined(X86_GALILEO)

#include <pci.h>
#include <multiboot.h>

#endif 

#ifdef X86_QEMU

#include <i386.h>
#include <io.h>
#include <e1000e.h>

#endif

#ifdef X86_GALILEO

#include <quark_eth.h>
#include <quark_pdat.h>
#include <quark_irq.h>

#endif

#endif

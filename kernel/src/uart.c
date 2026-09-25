#include "minemu/platform.h"
#include "minemu/trap.h"
#include "minemu/irq.h"
#include <stdint.h>

#define UART_BUF_SIZE 64

static char uart_buf[UART_BUF_SIZE];
static uint32_t head = 0;
static uint32_t tail = 0;

static void uart_buf_push(char c)
{
    uint32_t next = (head + 1) % UART_BUF_SIZE;
    if (next != tail) {
        uart_buf[head] = c;
        head = next;
    }
}

int uart_buf_pop(char *out)
{
    if (head == tail) return 0;
    *out = uart_buf[tail];
    tail = (tail + 1) % UART_BUF_SIZE;
    return 1;
}

void uart_irq_handler(struct minemu_trap_frame *frame)
{
    (void)frame;

    while (MINEMU_UART0->status & MINEMU_UART_STATUS_RX_READY) {
        uint32_t v = MINEMU_UART0->rx_data;
        uart_buf_push((char)(v & 0xff));
    }
}

int msh_read_char(char *out)
{
    int ok;

    minemu_irq_disable();
    ok = uart_buf_pop(out);
    minemu_irq_enable();

    return ok;
}


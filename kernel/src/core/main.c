#include "minemu/boot.h"
#include "minemu/trap.h"
#include "minemu/trace.h"
#include "minemu/platform.h"
#include "minemu/irq.h"
#include <stdint.h>

static int str_eq(const char *a, const char *b)
{
    while (*a && *b) {
        if (*a != *b)
            return 0;
        a++;
        b++;
    }
    return (*a == '\0' && *b == '\0');
}

static void uart_putc(char c)
{
    MINEMU_UART0->tx_data = (uint32_t)c;
}

static void uart_puts(const char *s)
{
    while (*s) {
        uart_putc(*s++);
    }
}

/* Provided by uart.c */
int msh_read_char(char *out);

static void msh_prompt(void)
{
    uart_puts("msh> ");
}

static void msh_run(void)
{
    char line[21];      // 20 chars + null terminator

    for (;;) {
        msh_prompt();

        uint32_t len = 0;

        // Read a full line
        for (;;) {
            char c;

            // Block until a character arrives
            while (!msh_read_char(&c)) {
                // spin
            }

            if (c == '\r')
                continue;

            if (c == '\n') {
                break;
            }

            // Backspace (0x08 or 0x7f)
            if (c == 0x08 || c == 0x7f) {
                if (len > 0) {
                    len--;
                }
                continue;
            }

            // Normal character
            if (len < 20) {
                line[len++] = c;
            } else {
                // Overlong input: ignore until newline
            }
        }

        line[len] = '\0';

        // Trim leading spaces
        uint32_t i = 0;
        while (i < len && line[i] == ' ') {
            i++;
        }

        // Empty or all-space line → new prompt
        if (i == len) {
            continue;
        }

        // Extract first word (command)
        char *cmd = &line[i];
        uint32_t cmd_end = i;
        while (cmd_end < len && line[cmd_end] != ' ') {
            cmd_end++;
        }
        line[cmd_end] = '\0';

        // Handle "echo"
        if (str_eq(cmd, "echo")) {
            uint32_t j = cmd_end + 1;

            // Skip spaces after echo
            while (j < len && line[j] == ' ') {
                j++;
            }

            if (j >= len) {
                uart_putc('\n');
            } else {
                uart_puts(&line[j]);
                uart_putc('\n');
            }
            continue;
        }

        // Unknown command
        uart_puts("command not found: ");
        uart_puts(cmd);
        uart_putc('\n');
    }
}

void minemu_kernel_main(const struct minemu_boot_info *boot_info) {
    if ((uintptr_t)boot_info != MINEMU_BOOT_INFO_VADDR ||
        boot_info->magic != MINEMU_BOOT_INFO_MAGIC ||
        boot_info->version != MINEMU_ABI_VERSION ||
        boot_info->size != sizeof(*boot_info) ||
        boot_info->system_rom_base != UINT32_C(0x08000000) ||
        boot_info->direct_map_vaddr != UINT32_C(0xc0000000) ||
        boot_info->direct_map_paddr != UINT32_C(0x40000000) ||
        boot_info->direct_map_size != UINT32_C(0x04000000)) {
        minemu_trace_event(UINT32_C(0xb007bad0));
        minemu_fail_stop();
    }

    minemu_trace_event(1);

    // Enable UART0 RX interrupt
    MINEMU_UART0->control |= UINT32_C(1);  // bit 0: RX interrupt enable

    // Enable UART0 in the interrupt controller
    MINEMU_INTERRUPT->enable |= (UINT32_C(1) << MINEMU_IRQ_UART0);

    // Start shell
    msh_run();
}

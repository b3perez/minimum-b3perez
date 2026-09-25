#include "minemu/boot.h"
#include "minemu/trap.h"
#include "minemu/trace.h"
#include "minemu/platform.h"

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
    MINEMU_UART0->tx_data = (uint32_t)'h';
    MINEMU_UART0->tx_data = (uint32_t)'e';
    MINEMU_UART0->tx_data = (uint32_t)'l';
    MINEMU_UART0->tx_data = (uint32_t)'l';
    MINEMU_UART0->tx_data = (uint32_t)'o';
    MINEMU_UART0->tx_data = (uint32_t)' ';
    MINEMU_UART0->tx_data = (uint32_t)'w';
    MINEMU_UART0->tx_data = (uint32_t)'o';
    MINEMU_UART0->tx_data = (uint32_t)'r';
    MINEMU_UART0->tx_data = (uint32_t)'l';
    MINEMU_UART0->tx_data = (uint32_t)'d';
    MINEMU_UART0->tx_data = (uint32_t)'\n';
    minemu_fail_stop();
}

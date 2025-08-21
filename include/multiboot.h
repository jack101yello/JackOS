#include <common/types.h>

#define MULTIBOOT_FLAG_MEM     0x001
#define MULTIBOOT_FLAG_DEVICE  0x002
#define MULTIBOOT_FLAG_CMDLINE 0x004
#define MULTIBOOT_FLAG_MODS    0x008
#define MULTIBOOT_FLAG_AOUT    0x010
#define MULTIBOOT_FLAG_ELF     0x020
#define MULTIBOOT_FLAG_MMAP    0x040
#define MULTIBOOT_FLAG_CONFIG  0x080
#define MULTIBOOT_FLAG_LOADER  0x100
#define MULTIBOOT_FLAG_APM     0x200
#define MULTIBOOT_FLAG_VBE     0x400

struct multiboot
{
   jackos::common::uint32_t flags;
   jackos::common::uint32_t mem_lower;
   jackos::common::uint32_t mem_upper;
   jackos::common::uint32_t boot_device;
   jackos::common::uint32_t cmdline;
   jackos::common::uint32_t mods_count;
   jackos::common::uint32_t mods_addr;
   jackos::common::uint32_t num;
   jackos::common::uint32_t size;
   jackos::common::uint32_t addr;
   jackos::common::uint32_t shndx;
   jackos::common::uint32_t mmap_length;
   jackos::common::uint32_t mmap_addr;
   jackos::common::uint32_t drives_length;
   jackos::common::uint32_t drives_addr;
   jackos::common::uint32_t config_table;
   jackos::common::uint32_t boot_loader_name;
   jackos::common::uint32_t apm_table;
   jackos::common::uint32_t vbe_control_info;
   jackos::common::uint32_t vbe_mode_info;
   jackos::common::uint32_t vbe_mode;
   jackos::common::uint32_t vbe_interface_seg;
   jackos::common::uint32_t vbe_interface_off;
   jackos::common::uint32_t vbe_interface_len;
}  __attribute__((packed));

typedef struct multiboot_header multiboot_header_t;

struct multiboot_mod_list {
   jackos::common::uint32_t mod_start;
   jackos::common::uint32_t mod_end;
   jackos::common::uint32_t string;
   jackos::common::uint32_t reserved;
};

typedef struct multiboot_mod_list multiboot_module_t;

#ifndef __jackos_gdt_H
#define __jackos_gdt_H

#include <common/types.h>

namespace jackos {
    struct tss_entry_struct {
        jackos::common::uint32_t prev_tss;
        jackos::common::uint32_t esp0;
        jackos::common::uint32_t ss0;
        jackos::common::uint32_t esp1;
        jackos::common::uint32_t ss1;
        jackos::common::uint32_t esp2;
        jackos::common::uint32_t ss2;
        jackos::common::uint32_t cr3;
        jackos::common::uint32_t eip;
        jackos::common::uint32_t eflags;
        jackos::common::uint32_t eax;
        jackos::common::uint32_t ecx;
        jackos::common::uint32_t edx;
        jackos::common::uint32_t ebx;
        jackos::common::uint32_t esp;
        jackos::common::uint32_t ebp;
        jackos::common::uint32_t esi;
        jackos::common::uint32_t edi;
        jackos::common::uint32_t es;
        jackos::common::uint32_t cs;
        jackos::common::uint32_t ss;
        jackos::common::uint32_t ds;
        jackos::common::uint32_t fs;
        jackos::common::uint32_t gs;
        jackos::common::uint32_t ldt;
        jackos::common::uint16_t trap;
        jackos::common::uint16_t iomap_base;
    } __attribute__((packed));
    typedef struct tss_entry_struct tss_entry_t;

    class GlobalDescriptorTable {
        public:
            class SegmentDescriptor {
                private:
                    jackos::common::uint16_t limit_lo;
                    jackos::common::uint16_t base_lo;
                    jackos::common::uint8_t base_hi;
                    jackos::common::uint8_t type;
                    jackos::common::uint8_t flags_limit_hi;
                    jackos::common::uint8_t base_vhi;
                public:
                    SegmentDescriptor(jackos::common::uint32_t base, jackos::common::uint32_t limit, jackos::common::uint8_t type);
                    jackos::common::uint32_t Base();
                    jackos::common::uint32_t Limit();
            } __attribute__((packed));

            struct TaskStateSegment {
                jackos::common::uint32_t prevTss;
                jackos::common::uint32_t esp0;  // Kernel stack pointer
                jackos::common::uint32_t ss0;   // Kernel stack segment
                jackos::common::uint32_t esp1;
                jackos::common::uint32_t ss1;
                jackos::common::uint32_t esp2;
                jackos::common::uint32_t ss2;
                jackos::common::uint32_t cr3;
                jackos::common::uint32_t eip;
                jackos::common::uint32_t eflags;
                jackos::common::uint32_t eax;
                jackos::common::uint32_t ecx;
                jackos::common::uint32_t edx;
                jackos::common::uint32_t ebx;
                jackos::common::uint32_t esp;
                jackos::common::uint32_t ebp;
                jackos::common::uint32_t esi;
                jackos::common::uint32_t edi;
                jackos::common::uint32_t es;
                jackos::common::uint32_t cs;
                jackos::common::uint32_t ss;
                jackos::common::uint32_t ds;
                jackos::common::uint32_t fs;
                jackos::common::uint32_t gs;
                jackos::common::uint32_t ldt;
                jackos::common::uint16_t trap;
                jackos::common::uint16_t iomap;
            } __attribute__((packed));

            SegmentDescriptor nullSegmentSelector;
            SegmentDescriptor unusedSegmentSelector;
            SegmentDescriptor codeSegmentSelector;
            SegmentDescriptor dataSegmentSelector;
            SegmentDescriptor userCodeSegmentSelector;
            SegmentDescriptor userDataSegmentSelector;
            SegmentDescriptor tssSegmentSelector;

            TaskStateSegment tss;

            GlobalDescriptorTable();
            ~GlobalDescriptorTable();

            jackos::common::uint16_t CodeSegmentSelector();
            jackos::common::uint16_t DataSegmentSelector();
            jackos::common::uint16_t UserCodeSegmentSelector();
            jackos::common::uint16_t UserDataSegmentSelector();
            jackos::common::uint16_t TSSSegmentSelector();
    };
}

#endif
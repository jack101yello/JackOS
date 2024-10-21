#ifndef __jackos_gdt_H
#define __jackos_gdt_H

#include <common/types.h>

namespace jackos {
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

            SegmentDescriptor nullSegmentSelector;
            SegmentDescriptor unusedSegmentSelector;
            SegmentDescriptor codeSegmentSelector;
            SegmentDescriptor dataSegmentSelector;

            GlobalDescriptorTable();
            ~GlobalDescriptorTable();

            jackos::common::uint16_t CodeSegmentSelector();
            jackos::common::uint16_t DataSegmentSelector();
    };
}

#endif
#ifndef __jackos_filesystem_iso9660_H
#define __jackos_filesystem_iso9660_H

#include <common/types.h>

#define CDROM_DATA_SECTOR_START 0x10

namespace jackos {
    namespace filesystem {
		/* Volule descriptor types:
		 * Boot Record | 0x00
		 * Primary Volume | 0x01
		 * Supplementary Volume | 0x02
		 * Volume Partition | 0x03
		 * Volume Set Terminator | 0xFF
		 */

		struct __attribute__((packed)) date_time {
			jackos::common::int32_t year;
			jackos::common::int16_t month;
			jackos::common::int16_t day;
			jackos::common::int16_t hour;
			jackos::common::int16_t minute;
			jackos::common::int16_t second;
			jackos::common::int16_t centisecond;
			jackos::common::int8_t timezone_offset; // In 15 minute intervals from -48 (west) to 52 (east). E.g. NYC time is either 28 or 32, due to daylight savings.
		};

		struct __attribute__((packed)) directory_date_time {
			jackos::common::uint8_t year; // Years since 1900
			jackos::common::uint8_t month; // Months 1-12
			jackos::common::uint8_t day; // Day of the month
			jackos::common::uint8_t hour; // 0-23
			jackos::common::uint8_t minute; // 0-59
			jackos::common::uint8_t second;
			jackos::common::uint8_t timezone_offset;
		};

		struct __attribute__((packed)) VolumeDescriptor {
			jackos::common::int8_t type; // See above
			char ident[5]; // Should be 'CD001'
			jackos::common::int8_t version; // The volume descriptor version (0x01)
			char data[2041];
		};

		struct __attribute__((packed)) BootRecord {
			jackos::common::int8_t type; // 0x0 indicates a boot record
			char ident[5];
			jackos::common::int8_t version;
			char boot_system_identifier[32];
			char boot_identifier[32];
			char boot_system_use[1977];
		};

		struct __attribute__((packed)) PrimaryVolumeDescriptor {
			jackos::common::int8_t type;
			char ident[5];
			jackos::common::int8_t version;
			char unused_block1; // Always 0x00
			char system_identifier[32];
			char volume_identifier[32];
			char unused_block2[8]; // All 0x00
			jackos::common::uint32_t volume_space_size_lsb; // Number of logical blocks
			jackos::common::uint32_t volume_space_size_msb;
			char unused_block3[32];
			jackos::common::uint16_t volume_set_size; // Number of disks
			jackos::common::uint16_t volume_set_size_msb;
			jackos::common::uint16_t volume_sequence_number; // This disk's number in the volume set
			jackos::common::uint16_t volume_sequence_number_msb;
			jackos::common::uint16_t logical_block_size; // Usually 2 KiB
			jackos::common::uint16_t logical_block_size_msb;
			jackos::common::uint32_t path_table_size;
			jackos::common::uint32_t path_table_size_msb;
			jackos::common::uint32_t lpath_table_location; // The LBA location of the type-l path table
			jackos::common::int32_t loptional_path_table_location; // The LBA location of the optional type-l path table
			jackos::common::int32_t mpath_table_location; // The LBA location of the type-m path table
			jackos::common::int32_t m_optional_path_table_location; // The LBA location of the optional type-m path table
			char root_dir_entry[34]; // The directory record for the root directory
			char volume_set_identifier[128];
			char publisher_identifier[128];
			char data_preparer_identifier[128];
			char application_identifier[128];
			char copyright_file_identifier[37]; // Filename of a file in root that contains copyright information
			char abstract_file_identifier[37]; // Filename of a file in root that contains abstract information
			char bibliographic_file_identifier[37]; // Filename of a file in root that contains bibliographic information
			date_time volume_creation_dt;
			date_time volume_modification_dt;
			date_time volume_expiration_dt;
			date_time volume_effective_dt;
			jackos::common::int8_t volume_structure_version; // Always 0x01
			char unused_block4;
			char application_used[512]; // This is not defined by the ISO9660 standard.
			char reserved[653]; // Reserved by ISO
		};

		struct __attribute__((packed)) VolumeDescriptorSetTerminator {
			jackos::common::int8_t type;
			char ident[5];
			jackos::common::int8_t version;
		};

		struct __attribute__((packed)) PathTableEntry {
			jackos::common::int8_t directory_identifier_length;
			jackos::common::int8_t extended_attribute_record_length;
			jackos::common::uint32_t location; // This is in a different format in the l-type and m-type tables
			jackos::common::uint16_t parent_directory_number;
			char identifier[8];
		};

		struct __attribute__((packed)) DirectoryEntry {
			jackos::common::int8_t directory_record_length;
			jackos::common::int8_t extended_attribute_record_length;
			jackos::common::uint32_t extent_location;
			jackos::common::uint32_t extent_location_msb;
			jackos::common::uint32_t data_length;
			jackos::common::uint32_t data_length_msb;
			directory_date_time dt;
			jackos::common::int8_t flags;
			jackos::common::int8_t file_unit_size; // Zero if not in interleaved mode
			jackos::common::int8_t interleave_gap_size; // Zero if not in interleaved mode
			jackos::common::uint16_t volume_sequence_number;
			jackos::common::uint16_t volume_sequence_number_msb;
			jackos::common::int8_t file_identifier_length; // Length of the filename
			char file_identifier[8]; // Filename
		};
    }
}

#endif

fat16_jump_instruction:
db 0xeb, 0x3c, 0x90, ; JMP SHORT 3C NOP
fat16_oem_identifier:
db 0x6d, 0x6b, 0x66, 0x73, 0x2e, 0x66, 0x61, 0x74, ; OEM identifier
fat16_bytes_per_sector:
db 0x00, 0x02, ; bytes per sector
fat16_sectors_per_cluster:
db 0x04, ; sectors per cluster
fat16_number_of_reserved_sectors:
db 0x04, 0x00, ; number of reserved sectors
fat16_number_of_fats:
db 0x02, ; number of FATs
fat16_number_of_root_directory_entries:
db 0x00, 0x02, ; number of root directory entries
fat16_total_sectors_of_logical_volume:
db 0x00, 0x00, ; total sectors of logical volume
fat16_media_descriptor_type:
db 0xf8, ; media descriptor type
fat16_sectors_per_fat: 
db 0x64, 0x00, ; sectors per fat
fat16_sectors_per_track:
db 0x20, 0x00, ; sectors per track
fat16_number_of_heads:
db 0x08, 0x00, ; number of heads 
fat16_number_of_hidden_sectors:
db 0x00, 0x00, 0x00, 0x00, ; number of hidden sectors
fat16_large_sector_count:
db 0x00, 0x90, 0x01, 0x00, ; large sector count
fat16_drive_number:
db 0x80, ; drive number
fat16_flags_windows_NT:
db 0x00, ; flags in Windows NT
fat16_signature:
db 0x29, ; signature
fat16_volume_serial:
db 0x78, 0x40, 0xa3, 0x4e, ; volume serial
fat16_volume_label_string:
db 0x4e, 0x4f, 0x20, 0x4e, 0x41, 0x4d, 0x45, 0x20, 0x20, 0x20, 0x20, ; volume label string
fat16_system_identifier_string:
db 0x46, 0x41, 0x54, 0x31, 0x36, 0x20, 0x20, 0x20, ; system identiefier string
/* fs.h - FAT16 filesystem */
#pragma once
#include <kernel/types.h>

#define FAT16_SECTOR_SIZE       512
#define FAT16_MAX_FILENAME      12
#define FAT16_MAX_PATH          256
#define FAT16_MAX_FILES         64
#define FAT16_MAX_OPEN          16
#define FAT16_ROOT_ENTRIES      512

/* FAT16 Boot Sector (BPB) */
typedef struct {
    u8  jmp[3];
    u8  oem[8];
    u16 bytes_per_sector;
    u8  sectors_per_cluster;
    u16 reserved_sectors;
    u8  fat_count;
    u16 root_entries;
    u16 total_sectors_16;
    u8  media_type;
    u16 sectors_per_fat;
    u16 sectors_per_track;
    u16 heads;
    u32 hidden_sectors;
    u32 total_sectors_32;
    u8  drive_num;
    u8  reserved1;
    u8  boot_sig;
    u32 volume_id;
    u8  volume_label[11];
    u8  fs_type[8];
} PACKED fat16_bpb_t;

/* Directory entry */
typedef struct {
    u8  name[11];
    u8  attr;
    u8  ntres;
    u8  crt_time_tenth;
    u16 crt_time;
    u16 crt_date;
    u16 acc_date;
    u16 cluster_high;
    u16 wrt_time;
    u16 wrt_date;
    u16 cluster_low;
    u32 file_size;
} PACKED fat16_dir_entry_t;

/* File attributes */
#define FAT_ATTR_READ_ONLY      0x01
#define FAT_ATTR_HIDDEN         0x02
#define FAT_ATTR_SYSTEM         0x04
#define FAT_ATTR_VOLUME_ID      0x08
#define FAT_ATTR_DIRECTORY      0x10
#define FAT_ATTR_ARCHIVE        0x20
#define FAT_ATTR_LONG_NAME      0x0F

/* Open file handle */
typedef struct {
    bool used;
    char name[FAT16_MAX_FILENAME];
    u32  cluster;
    u32  size;
    u32  position;
    u8   attr;
} fat16_file_t;

/* Filesystem state */
typedef struct {
    bool mounted;
    fat16_bpb_t bpb;
    u32 fat_start;
    u32 root_start;
    u32 data_start;
    u32 root_sectors;
    u32 total_clusters;
    u16* fat_cache;
    fat16_file_t open_files[FAT16_MAX_OPEN];
} fat16_fs_t;

bool fs_init(void);
bool fs_mount(void);
bool fs_open(const char* path, fat16_file_t** out_handle);
bool fs_create(const char* path);
bool fs_read(fat16_file_t* file, void* buffer, u32 size, u32* out_read);
bool fs_write(fat16_file_t* file, const void* buffer, u32 size);
bool fs_close(fat16_file_t* file);
bool fs_list(const char* path, void (*callback)(const char* name, u32 size, u8 attr));
bool fs_exists(const char* path);


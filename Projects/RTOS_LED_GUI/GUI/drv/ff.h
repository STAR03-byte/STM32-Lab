/**
  *********************************************************************
  * @file    ff.h
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   FATFS 文件系统头文件 (存根，不使用文件系统时)
  **********************************************************************
  */
#ifndef _FATFS
#define _FATFS

#include <stdint.h>

/* 文件系统类型 */
typedef int FRESULT;

/* 返回值定义 */
#define FR_OK                  0
#define FR_DISK_ERR             1
#define FR_INT_ERR              2
#define FR_NOT_READY            3
#define FR_NO_FILE              4
#define FR_NO_PATH              5
#define FR_INVALID_NAME         6
#define FR_DENIED               7
#define FR_EXIST                8
#define FR_INVALID_OBJECT       9
#define FR_WRITE_PROTECTED      10
#define FR_INVALID_DRIVE        11
#define FR_NOT_ENABLED          12
#define FR_NO_FILESYSTEM        13
#define FR_MKFS_ABORTED         14
#define FR_TIMEOUT              15
#define FR_LOCKED               16
#define FR_NOT_ENOUGH_CORE      17
#define FR_TOO_MANY_OPEN_FILES  18

/* 文件访问模式 */
#define FA_READ                 0x01
#define FA_WRITE                0x02
#define FA_OPEN_EXISTING        0x00
#define FA_CREATE_NEW           0x04
#define FA_CREATE_ALWAYS        0x08
#define FA_OPEN_ALWAYS          0x10
#define FA_OPEN_APPEND          0x30

/* 文件属性 */
#define AM_RDO  0x01
#define AM_HID  0x02
#define AM_SYS  0x04
#define AM_DIR  0x10
#define AM_ARC  0x20

/* 文件信息结构体 */
typedef struct {
    uint32_t fsize;
    uint16_t fdate;
    uint16_t ftime;
    uint8_t  fattrib;
    char     fname[13];
} FILINFO;

/* 文件对象结构体 */
typedef struct {
    uint8_t  flag;
    uint8_t  err;
    uint32_t fptr;
    uint32_t fsize;
    uint32_t sclust;
    uint32_t clust;
    uint32_t sect;
    uint32_t dir_sect;
    uint8_t* dir_ptr;
} FIL;

/* 目录对象结构体 */
typedef struct {
    uint8_t  fs_type;
    uint8_t  drv;
    uint16_t id;
    uint16_t n_rootdir;
    uint32_t n_fatent;
    uint32_t fatbase;
    uint32_t dirbase;
    uint32_t database;
    uint32_t last_clust;
    uint32_t free_clust;
    uint32_t fsi_flag;
    uint32_t fsi_sector;
    uint8_t* win;
} FATFS;

/* 文件系统函数存根 */
static inline FRESULT f_mount(FATFS* fs, const char* path, uint8_t opt) { (void)fs; (void)path; (void)opt; return FR_NOT_ENABLED; }
static inline FRESULT f_open(FIL* fp, const char* path, uint8_t mode) { (void)fp; (void)path; (void)mode; return FR_NOT_ENABLED; }
static inline FRESULT f_close(FIL* fp) { (void)fp; return FR_NOT_ENABLED; }
static inline FRESULT f_read(FIL* fp, void* buff, uint32_t btr, uint32_t* br) { (void)fp; (void)buff; (void)btr; (void)br; return FR_NOT_ENABLED; }
static inline FRESULT f_write(FIL* fp, const void* buff, uint32_t btw, uint32_t* bw) { (void)fp; (void)buff; (void)btw; (void)bw; return FR_NOT_ENABLED; }
static inline FRESULT f_lseek(FIL* fp, uint32_t ofs) { (void)fp; (void)ofs; return FR_NOT_ENABLED; }
static inline FRESULT f_opendir(FATFS* dp, const char* path) { (void)dp; (void)path; return FR_NOT_ENABLED; }
static inline FRESULT f_readdir(FATFS* dp, FILINFO* fno) { (void)dp; (void)fno; return FR_NOT_ENABLED; }
static inline FRESULT f_stat(const char* path, FILINFO* fno) { (void)path; (void)fno; return FR_NOT_ENABLED; }
static inline FRESULT f_getfree(const char* path, uint32_t* nclst, FATFS** fatfs) { (void)path; (void)nclst; (void)fatfs; return FR_NOT_ENABLED; }

#endif /* _FATFS */

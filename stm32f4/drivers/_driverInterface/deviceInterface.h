#ifndef __DEVICEINTERFACE_H__
#define __DEVICEINTERFACE_H__

#include "list.h"
#include "stdint.h"
#include "stdio.h"
#include "ysizet.h"

#define USING_DEVICE              //使用RT 设备驱动驱动
#define NAME_MAX	              32

//#define NULL                      (0)

/* RT-Thread basic data type definitions */
//typedef signed   char                   int8_t;      /**<  8bit integer type */
//typedef signed   short                  int16_t;     /**< 16bit integer type */
//typedef signed   long                   int32_t;     /**< 32bit integer type */
//typedef unsigned char                   uint8_t;     /**<  8bit unsigned integer type */
//typedef unsigned short                  uint16_t;    /**< 16bit unsigned integer type */
//typedef unsigned long                   uint32_t;    /**< 32bit unsigned integer type */
typedef int                             bool_t;      /**< boolean type */

/* 32bit CPU */
typedef long                            base_t;      /**< Nbit CPU related date type */
typedef unsigned long                   ubase_t;     /**< Nbit unsigned CPU related data type */

typedef base_t                        err_t;       /**< Type for error number */
typedef uint32_t                      time_t;      /**< Type for time stamp */
typedef uint32_t                      tick_t;      /**< Type for tick count */
typedef base_t                        flag_t;      /**< Type for flags */
//typedef ubase_t                       size_t;      /**< Type for size number */
typedef ubase_t                       dev_t;       /**< Type for device */
typedef base_t                        off_t;       /**< Type for offset */



/**
 * Base structure of Kernel object
 */
struct object
{
    char       name[NAME_MAX];                       /**< name of kernel object */
    uint8_t type;                                    /**< type of kernel object */
    uint8_t flag;                                    /**< flag of kernel object */

    list_t  list;                                    /**< list node of kernel object */
};
typedef struct object *object_t;                  /**< Type for kernel objects. */


#ifdef USING_DEVICE
/**
 * @addtogroup Device
 */

/*@{*/

/**
 * device (I/O) class type
 */
enum device_class_type
{
    Device_Class_Char = 0,                           /**< character device */
    Device_Class_Block,                              /**< block device */
    Device_Class_NetIf,                              /**< net interface */
    Device_Class_MTD,                                /**< memory device */
    Device_Class_CAN,                                /**< CAN device */
    Device_Class_RTC,                                /**< RTC device */
    Device_Class_Sound,                              /**< Sound device */
    Device_Class_Graphic,                            /**< Graphic device */
    Device_Class_I2CBUS,                             /**< I2C bus device */
    Device_Class_USBDevice,                          /**< USB slave device */
    Device_Class_USBHost,                            /**< USB host bus */
    Device_Class_SPIBUS,                             /**< SPI bus device */
    Device_Class_SPIDevice,                          /**< SPI device */
    Device_Class_SDIO,                               /**< SDIO bus device */
    Device_Class_PM,                                 /**< PM pseudo device */
    Device_Class_Pipe,                               /**< Pipe device */
    Device_Class_Portal,                             /**< Portal device */
    Device_Class_Timer,                              /**< Timer device */
	Device_Class_Miscellaneous,                      /**< Miscellaneous device */
	Device_Class_Unknown                             /**< unknown device */
};

/**
 * device flags defitions
 */
#define DEVICE_FLAG_DEACTIVATE       0x000           /**< device is not not initialized */

#define DEVICE_FLAG_RDONLY           0x001           /**< read only */
#define DEVICE_FLAG_WRONLY           0x002           /**< write only */
#define DEVICE_FLAG_RDWR             0x003           /**< read and write */

#define DEVICE_FLAG_REMOVABLE        0x004           /**< removable device */
#define DEVICE_FLAG_STANDALONE       0x008           /**< standalone device */
#define DEVICE_FLAG_ACTIVATED        0x010           /**< device is activated */
#define DEVICE_FLAG_SUSPENDED        0x020           /**< device is suspended */
#define DEVICE_FLAG_STREAM           0x040           /**< stream mode */

#define DEVICE_CTRL_CONFIG           0x03    	/* configure device */
#define DEVICE_CTRL_SET_INT          0x10    	/* enable receive irq */
#define DEVICE_CTRL_CLR_INT          0x11    	/* disable receive irq */
#define DEVICE_CTRL_GET_INT          0x12

#define DEVICE_FLAG_INT_RX           0x100           /**< INT mode on Rx */
#define DEVICE_FLAG_DMA_RX           0x200           /**< DMA mode on Rx */
#define DEVICE_FLAG_INT_TX           0x400           /**< INT mode on Tx */
#define DEVICE_FLAG_DMA_TX           0x800           /**< DMA mode on Tx */

#define DEVICE_OFLAG_CLOSE           0x000           /**< device is closed */
#define DEVICE_OFLAG_RDONLY          0x001           /**< read only access */
#define DEVICE_OFLAG_WRONLY          0x002           /**< write only access */
#define DEVICE_OFLAG_RDWR            0x003           /**< read and write */
#define DEVICE_OFLAG_OPEN            0x008           /**< device is opened */

/**
 * general device commands
 */
#define DEVICE_CTRL_RESUME           0x01            /**< resume device */
#define DEVICE_CTRL_SUSPEND          0x02            /**< suspend device */

/**
 * special device commands
 */
#define DEVICE_CTRL_CHAR_STREAM      0x10            /**< stream mode on char device */
#define DEVICE_CTRL_BLK_GETGEOME     0x10            /**< get geometry information   */
#define DEVICE_CTRL_BLK_SYNC         0x11            /**< flush data to block device */
#define DEVICE_CTRL_BLK_ERASE        0x12            /**< erase block on block device */
#define DEVICE_CTRL_BLK_AUTOREFRESH  0x13            /**< block device : enter/exit auto refresh mode */
#define DEVICE_CTRL_NETIF_GETMAC     0x10            /**< get mac address */
#define DEVICE_CTRL_MTD_FORMAT       0x10            /**< format a MTD device */
#define DEVICE_CTRL_RTC_GET_TIME     0x10            /**< get time */
#define DEVICE_CTRL_RTC_SET_TIME     0x11            /**< set time */
#define DEVICE_CTRL_RTC_GET_ALARM    0x12            /**< get alarm */
#define DEVICE_CTRL_RTC_SET_ALARM    0x13            /**< set alarm */

typedef struct device *device_t;
/**
 * Device structure
 */
struct device
{
    struct object          parent;                   /**< inherit from object */

    enum device_class_type type;                     /**< device type */
    uint16_t               flag;                     /**< device flag */
    uint16_t               open_flag;                /**< device open flag */

    uint8_t                ref_count;                /**< reference count */
    uint8_t                device_id;                /**< 0 - 255 */

    /* device call back */
    err_t (*rx_indicate)(device_t dev, size_t size);
    err_t (*tx_complete)(device_t dev, void *buffer);

    /* common device interface */
    err_t  (*init)   (device_t dev);
    err_t  (*open)   (device_t dev, uint16_t oflag);
    err_t  (*close)  (device_t dev);
    size_t (*read)   (device_t dev, off_t pos, void *buffer, size_t size);
    size_t (*write)  (device_t dev, off_t pos, const void *buffer, size_t size);
    err_t  (*control)(device_t dev, uint8_t cmd, void *args);

    void                     *user_data;                /**< device private data */
};

/**
 * block device geometry structure
 */
struct device_blk_geometry
{
    uint32_t sector_count;                           /**< count of sectors */
    uint32_t bytes_per_sector;                       /**< number of bytes per sector */
    uint32_t block_size;                             /**< number of bytes to erase one block */
};

/**
 * sector arrange struct on block device
 */
struct device_blk_sectors
{
    uint32_t sector_begin;                           /**< begin sector */
    uint32_t sector_end;                             /**< end sector   */
};

/**
 * cursor control command
 */
#define DEVICE_CTRL_CURSOR_SET_POSITION  0x10
#define DEVICE_CTRL_CURSOR_SET_TYPE      0x11

/**
 * graphic device control command
 */
#define RTGRAPHIC_CTRL_RECT_UPDATE      0
#define RTGRAPHIC_CTRL_POWERON          1
#define RTGRAPHIC_CTRL_POWEROFF         2
#define RTGRAPHIC_CTRL_GET_INFO         3
#define RTGRAPHIC_CTRL_SET_MODE         4
#define RTGRAPHIC_CTRL_GET_EXT          5

/* graphic deice */
enum
{
    RTGRAPHIC_PIXEL_FORMAT_MONO = 0,
    RTGRAPHIC_PIXEL_FORMAT_GRAY4,
    RTGRAPHIC_PIXEL_FORMAT_GRAY16,
    RTGRAPHIC_PIXEL_FORMAT_RGB332,
    RTGRAPHIC_PIXEL_FORMAT_RGB444,
    RTGRAPHIC_PIXEL_FORMAT_RGB565,
    RTGRAPHIC_PIXEL_FORMAT_RGB565P,
    RTGRAPHIC_PIXEL_FORMAT_BGR565 = RTGRAPHIC_PIXEL_FORMAT_RGB565P,
    RTGRAPHIC_PIXEL_FORMAT_RGB666,
    RTGRAPHIC_PIXEL_FORMAT_RGB888,
    RTGRAPHIC_PIXEL_FORMAT_ARGB888,
    RTGRAPHIC_PIXEL_FORMAT_ABGR888,
};

/**
 * build a pixel position according to (x, y) coordinates.
 */
#define RTGRAPHIC_PIXEL_POSITION(x, y)  ((x << 16) | y)

/**
 * graphic device information structure
 */
struct device_graphic_info
{
    uint8_t  pixel_format;                           /**< graphic format */
    uint8_t  bits_per_pixel;                         /**< bits per pixel */
    uint16_t reserved;                               /**< reserved field */

    uint16_t width;                                  /**< width of graphic device */
    uint16_t height;                                 /**< height of graphic device */

    uint8_t *framebuffer;                            /**< frame buffer */
};

/**
 * rectangle information structure
 */
struct device_rect_info
{
    uint16_t x;                                      /**< x coordinate */
    uint16_t y;                                      /**< y coordinate */
    uint16_t width;                                  /**< width */
    uint16_t height;                                 /**< height */
};

/**
 * graphic operations
 */
struct device_graphic_ops
{
    void (*set_pixel) (const char *pixel, int x, int y);
    void (*get_pixel) (char *pixel, int x, int y);

    void (*draw_hline)(const char *pixel, int x1, int x2, int y);
    void (*draw_vline)(const char *pixel, int x, int y1, int y2);

    void (*blit_line) (const char *pixel, int x, int y, size_t size);
};
#define graphix_ops(device)          ((struct device_graphic_ops *)(device->user_data))

/*@}*/
#endif

#define DT_UNKNOWN		0x00
#define DT_REG			0x01
#define DT_DIR			0x02

/**
 * Base structure of IPC object
 */
struct ipc_object
{
    struct object parent;                            /**< inherit from object */

    list_t        suspend_thread;                    /**< threads pended on this resource */
};


/**
 * Semaphore structure
 */
struct semaphore
{
    struct ipc_object parent;                        /**< inherit from ipc_object */

    uint16_t          value;                         /**< value of semaphore. */
};
typedef struct semaphore *sem_t;


/* RT-Thread error code definitions */
#define EOK                          0               /**< There is no error */
#define ERROR                        1               /**< A generic error happens */
#define ETIMEOUT                     2               /**< Timed out */
#define EFULL                        3               /**< The resource is full */
#define EEMPTY                       4               /**< The resource is empty */
#define ENOMEM                       5               /**< No memory */
#define ENOSYS                       6               /**< No system */
#define EBUSY                        7               /**< Busy */
#define EIO                          8               /**< IO error */


//函数接口
err_t    device_register(device_t dev,
                            const char *name,
                            uint16_t flags);
err_t    device_unregister(device_t dev);
err_t    device_open(device_t dev, uint16_t oflag);
err_t    device_close(device_t dev);
size_t   device_read(device_t dev,
                         off_t    pos,
                         void       *buffer,
                         size_t   size);
size_t   device_write(device_t dev,
                          off_t    pos,
                          const void *buffer,
                          size_t   size);
err_t    device_control(device_t dev, uint8_t cmd, void *arg);
device_t device_find(const char *name);
err_t    device_init(device_t dev);
void     device_listInit(void);









#endif





/*
 * Copyright (C) 2007 Google, Inc.
 * Copyright (c) 2008-2010, Code Aurora Forum. All rights reserved.
 * Author: Brian Swetland <swetland@google.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/bootmem.h>
//#include <linux/usb/mass_storage_function.h>
#include <linux/power_supply.h>
#include <linux/gpio_event.h>
#include <linux/i2c-gpio.h>


#include <mach/hardware.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/flash.h>
#include <asm/setup.h>
#ifdef CONFIG_CACHE_L2X0
#include <asm/hardware/cache-l2x0.h>
#endif

#include <asm/mach/mmc.h>
#include <mach/vreg.h>
#include <mach/mpp.h>
#include <mach/gpio.h>
#include <mach/board.h>
#include <mach/pmic.h>
#include <mach/msm_iomap.h>
#include <mach/msm_rpcrouter.h>
#include <mach/msm_hsusb.h>
#include <mach/rpc_hsusb.h>
#include <mach/rpc_pmapp.h>
#include <mach/msm_serial_hs.h>
#include <mach/memory.h>
#include <mach/msm_battery.h>
#include <mach/rpc_server_handset.h>
#include <mach/msm_tsif.h>
#include <mach/socinfo.h>

#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/i2c.h>
#include <linux/android_pmem.h>
#include <mach/camera.h>
#include "devices.h"
#include "clock.h"
#include "msm-keypad-devices.h"
#ifdef CONFIG_SAMSUNG_JACK
#include <linux/sec_jack.h>
#include "proc_comm.h"
#endif
#include "pm.h"
#ifdef CONFIG_ARCH_MSM7X27
#include <linux/msm_kgsl.h>
#endif

#ifdef CONFIG_USB_ANDROID
#include <linux/usb/android_composite.h>
#include <linux/usb/f_accessory.h>
#endif

#ifdef CONFIG_SENSORS_BMA_ACCEL
#include <linux/i2c/bma023_dev.h>
#endif
#ifdef CONFIG_SENSORS_MMC31XX
#include <linux/i2c/mmc31xx.h>
#endif
#ifdef CONFIG_MACH_COOPER_BASE_KOR
#include <linux/skbuff.h>	// 2011.01.21 static wifi skb
#include  <mach/media.h> 
#endif


#ifdef CONFIG_SENSORS_KR3D_ACCEL
#define KR3DM_I2C_ADDR    0x12	//0x10
#endif

#if defined(CONFIG_MACH_JUNO_SKT) || defined(CONFIG_MACH_JUNO_KT)
#if defined (CONFIG_TOUCHSCREEN_QT602240)
/*
*/
#elif defined (CONFIG_TOUCHSCREEN_MELFAS)
#include <linux/input/melfas_ts.h>
#endif
#endif
#include <linux/i2c/tsp_gpio.h>

#ifdef CONFIG_ARCH_MSM7X25
#define MSM_PMEM_MDP_SIZE	0xb21000
#define MSM_PMEM_ADSP_SIZE	0x97b000
#define MSM_FB_SIZE		0x200000
#define PMEM_KERNEL_EBI1_SIZE	0x64000
#endif

#ifdef CONFIG_ARCH_MSM7X27
#if defined(CONFIG_MACH_COOPER)
#define MSM_PMEM_MDP_SIZE 	0x1B76000 // size = 23<<20; in gralloc.cpp
#if defined(CONFIG_MACH_GIO_SKT) || defined(CONFIG_MACH_GIO_KT) || defined(CONFIG_MACH_JUNO_SKT) || defined(CONFIG_MACH_JUNO_KT)
#define MSM_PMEM_ADSP_SIZE 	0x9DE000  // 0x8DE000		// 3M :0x86E000, 2M : 0x77F000 
#else
#define MSM_PMEM_ADSP_SIZE 	0xB71000    // 5M: 0xB71000		// 3M :0x86E000, 2M : 0x77F000 
#endif
#define MSM_PMEM_AUDIO_SIZE 	0x5B000 
#ifdef CONFIG_FB_MSM_LCDC_S6E63M0_WVGA_PANEL
#define MSM_FB_SIZE 		0x2EE000	// ARGB8888 double duffrting // 0x238000 // 0x500000//0x11C000	//0x5DC00 

#else
#define MSM_FB_SIZE 		0x238000 // 0x500000//0x11C000	//0x5DC00 
#endif
#define PMEM_KERNEL_EBI1_SIZE 	0x1C000 
#endif	// CONFIG_MACH_CALLISTO
#endif	// CONFIG_ARCH_MSM7X27

//#if defined(CONFIG_MACH_EUROPA)
//#endif

extern int board_hw_revision;

#if defined(CONFIG_MACH_JUNO_SKT) || defined(CONFIG_MACH_JUNO_KT)
#define FEATURE_CAMERA_GPIO_I2C
#endif

/*
#ifdef	CONFIG_MACH_COOPER	
#define	WLAN_RESET_N	78
#else
#define	WLAN_RESET_N	82
#endif
*/
//#define	BT_PWR		88

#if defined(CONFIG_MACH_JUNO_SKT) || defined(CONFIG_MACH_JUNO_KT)
//#define VDD_WLAN_EN	77
#define WLAN_RST                        82   //WLAN_RESET_N
#define WLAN_BT_EN                    106   //WLAN_BT_REG_EN  
#define GPIO_WLAN_HOST_WAKE  17   //WLAN_HOST_WAKE
static int gpio_wlan_reset_n = 82; //wlan _setup_pascal
static int bt_power_on_enter = 0 ;//bt power on
#define	BT_RESET		75		// BT_RESET
#define BT_WAKE_PIN     57     // BT_WAKE

#else
#define VDD_WLAN_EN	77
#define WLAN_RST                        82   //WLAN_RESET_N
#define WLAN_BT_EN                    88   //WLAN_BT_REG_EN  
#define GPIO_WLAN_HOST_WAKE  17   //WLAN_HOST_WAKE
static int gpio_wlan_reset_n = 82; //wlan _setup_pascal
static int bt_power_on_enter = 0 ;//bt power on
#define	BT_RESET		75		// BT_RESET
#define BT_WAKE_PIN     57     // BT_WAKE
#endif


#ifdef CONFIG_MACH_COOPER_BASE_KOR
/*
#define  S5PV210_VIDEO_SAMSUNG_MEMSIZE_FIMC0 (8192 * SZ_1K)
#define  S5PV210_VIDEO_SAMSUNG_MEMSIZE_FIMC1 (9900 * SZ_1K)
#define  S5PV210_VIDEO_SAMSUNG_MEMSIZE_FIMC2 (8192 * SZ_1K)
#define  S5PV210_VIDEO_SAMSUNG_MEMSIZE_MFC0 (36864 * SZ_1K)
#define  S5PV210_VIDEO_SAMSUNG_MEMSIZE_MFC1 (36864 * SZ_1K)
#define  S5PV210_VIDEO_SAMSUNG_MEMSIZE_FIMD (4800 * SZ_1K)
#define  S5PV210_VIDEO_SAMSUNG_MEMSIZE_JPEG (14100 * SZ_1K)
#define  S5PV210_VIDEO_SAMSUNG_MEMSIZE_PMEM (8192 * SZ_1K)
#define  S5PV210_VIDEO_SAMSUNG_MEMSIZE_PMEM_GPU1 (4200 * SZ_1K)
#define  S5PV210_VIDEO_SAMSUNG_MEMSIZE_TEXSTREAM (4800 * SZ_1K)
*/
static struct s5p_media_device crespo_media_devs[] = {
/*
	[0] = {
		.id = S5P_MDEV_MFC,
		.name = "mfc",
		.bank = 0,
		.memsize = S5PV210_VIDEO_SAMSUNG_MEMSIZE_MFC0,
		.paddr = 0,
	},
	[1] = {
		.id = S5P_MDEV_MFC,
		.name = "mfc",
		.bank = 1,
		.memsize = S5PV210_VIDEO_SAMSUNG_MEMSIZE_MFC1,
		.paddr = 0,
	},
	[2] = {
		.id = S5P_MDEV_FIMC0,
		.name = "fimc0",
		.bank = 1,
		.memsize = S5PV210_VIDEO_SAMSUNG_MEMSIZE_FIMC0,
		.paddr = 0,
	},
	[3] = {
		.id = S5P_MDEV_FIMC1,
		.name = "fimc1",
		.bank = 1,
		.memsize = S5PV210_VIDEO_SAMSUNG_MEMSIZE_FIMC1,
		.paddr = 0,
	},
	[4] = {
		.id = S5P_MDEV_FIMC2,
		.name = "fimc2",
		.bank = 1,
		.memsize = S5PV210_VIDEO_SAMSUNG_MEMSIZE_FIMC2,
		.paddr = 0,
	},
	[5] = {
		.id = S5P_MDEV_JPEG,
		.name = "jpeg",
		.bank = 0,
		.memsize = S5PV210_VIDEO_SAMSUNG_MEMSIZE_JPEG,
		.paddr = 0,
	},
	[6] = {
		.id = S5P_MDEV_FIMD,
		.name = "fimd",
		.bank = 1,
		.memsize = S5PV210_VIDEO_SAMSUNG_MEMSIZE_FIMD,
		.paddr = 0,
	},
	[7] = {
		.id = S5P_MDEV_PMEM,
		.name = "pmem",
		.bank = 0,
		.memsize = S5PV210_VIDEO_SAMSUNG_MEMSIZE_PMEM,
		.paddr = 0,
	},
	[8] = {
		.id = S5P_MDEV_PMEM_GPU1,
		.name = "pmem_gpu1",
		.bank = 0,
		.memsize = S5PV210_VIDEO_SAMSUNG_MEMSIZE_PMEM_GPU1,
		.paddr = 0,
	},
	[9] = {
		.id = S5P_MDEV_TEXSTREAM,
		.name = "texstream",
		.bank = 0,
		.memsize = S5PV210_VIDEO_SAMSUNG_MEMSIZE_TEXSTREAM,
		.paddr = 0,
	},
*/	
#ifdef CONFIG_WLAN_SAMSUNG_MEMSIZE_BCM
	[0] = {  //[10] = {
		.id = S3C_MDEV_WIFI,
		.name = "wifi",
		.bank = 0,
		.memsize = CONFIG_WLAN_SAMSUNG_MEMSIZE_BCM * SZ_1K,
		.paddr = 0,
	},
#endif	
};

//extern void s5p_reserve_bootmem(struct s5p_media_device *mdevs, int nr_mdevs);
#else  

#if 1  //defined(SDIO_ISR_THREAD)
#define WLAN_HOST_WAKE   
#endif  // CONFIG_MACH_COOPER

#endif

#ifdef CONFIG_SAMSUNG_JACK

#define GPIO_JACK_S_35	28
#define GPIO_SEND_END	29

static struct sec_jack_zone jack_zones[] = {
	[0] = {
		.adc_high	= 3,
		.delay_ms	= 10,
		.check_count	= 5,
		.jack_type	= SEC_HEADSET_3POLE,
	},
	[1] = {
		.adc_high	= 20,
		.delay_ms	= 10,
		.check_count	= 10,
		.jack_type	= SEC_HEADSET_3POLE,
	},
	[2] = {
		.adc_high	= 9999,
		.delay_ms	= 10,
		.check_count	= 5,
		.jack_type	= SEC_HEADSET_4POLE,
	},
};

static int get_stealth_det_jack_state(void)
{
#if defined(CONFIG_MACH_JUNO_SKT) || defined(CONFIG_MACH_JUNO_KT)
	/* Active High */
	return(gpio_get_value(GPIO_JACK_S_35));
#else
	/* Active Low */
	return(gpio_get_value(GPIO_JACK_S_35)) ^ 1;
#endif
}

static int get_stealth_send_key_state(void)
{
	/* Active High */
	return(gpio_get_value(GPIO_SEND_END));
}

#define SMEM_PROC_COMM_MICBIAS_ONOFF		PCOM_OEM_MICBIAS_ONOFF
#define SMEM_PROC_COMM_MICBIAS_ONOFF_REG5	PCOM_OEM_MICBIAS_ONOFF_REG5
#define SMEM_PROC_COMM_GET_ADC				PCOM_OEM_SAMSUNG_GET_ADC

enum {
	SMEM_PROC_COMM_GET_ADC_BATTERY = 0x0,
	SMEM_PROC_COMM_GET_ADC_TEMP,
	SMEM_PROC_COMM_GET_ADC_VF,
	SMEM_PROC_COMM_GET_ADC_ALL, // data1 : VF(MSB 2 bytes) vbatt_adc(LSB 2bytes), data2 : temp_adc
	SMEM_PROC_COMM_GET_ADC_EAR_ADC,		// 3PI_ADC
	SMEM_PROC_COMM_GET_ADC_MAX,
};

enum {
	SMEM_PROC_COMM_MICBIAS_CONTROL_OFF = 0x0,
	SMEM_PROC_COMM_MICBIAS_CONTROL_ON,
	SMEM_PROC_COMM_MICBIAS_CONTROL_MAX
};

static void set_stealth_micbias_state_reg5(bool state)
{
	int res = 0;
	int data1 = 0;
	int data2 = 0;
	if (!state)
	{
		data1 = SMEM_PROC_COMM_MICBIAS_CONTROL_OFF;
		res = msm_proc_comm(SMEM_PROC_COMM_MICBIAS_ONOFF_REG5, &data1, &data2);
		if(res < 0)
		{
			pr_err("sec_jack: micbias_reg5 %s  fail \n",state?"on":"off");
		}
	}
}


static void set_stealth_micbias_state(bool state)
{
	int res = 0;
	int data1 = 0;
	int data2 = 0;
	if (state)
	{
		data1 = SMEM_PROC_COMM_MICBIAS_CONTROL_ON;
	}
	else
	{
		data1 = SMEM_PROC_COMM_MICBIAS_CONTROL_OFF;
	}
	res = msm_proc_comm(SMEM_PROC_COMM_MICBIAS_ONOFF, &data1, &data2);
	if(res < 0)
	{
		pr_err("sec_jack: micbias %s  fail \n",state?"on":"off");
	}
}

static int sec_jack_get_adc_value(void)
{
	int res = 0;
	int data1 = SMEM_PROC_COMM_GET_ADC_EAR_ADC;
	int data2 = 0;

	res = msm_proc_comm(SMEM_PROC_COMM_GET_ADC, &data1, &data2);
	if(res < 0)
	{
		pr_err("sec_jack: get_adc fail \n");
		return res;
	}

	return data1;
}

void sec_jack_gpio_init(void)
{
	if(gpio_request(GPIO_JACK_S_35, "h2w_detect")<0)
		pr_err("sec_jack:gpio_request fail\n");
	if(gpio_direction_input(GPIO_JACK_S_35)<0)
		pr_err("sec_jack:gpio_direction fail\n");
	if(gpio_request(GPIO_SEND_END , "h2w_button")<0)
		pr_err("sec_jack:gpio_request fail\n");
	if(gpio_direction_input(GPIO_SEND_END )<0)
		pr_err("sec_jack:gpio_direction fail\n");
}

static struct sec_jack_platform_data sec_jack_data = {
	.get_det_jack_state	= get_stealth_det_jack_state,
	.get_send_key_state	= get_stealth_send_key_state,
	.set_micbias_state	= set_stealth_micbias_state,
	.set_micbias_state_reg5	= set_stealth_micbias_state_reg5,
	.get_adc_value	= sec_jack_get_adc_value,
	.zones		= jack_zones,
	.num_zones	= ARRAY_SIZE(jack_zones),
	.det_int	= MSM_GPIO_TO_INT(GPIO_JACK_S_35),
	.send_int	= MSM_GPIO_TO_INT(GPIO_SEND_END),
};

static struct platform_device sec_device_jack = {
	.name           = "sec_jack",
	.id             = -1,
	.dev            = {
		.platform_data  = &sec_jack_data,
	},
};
#endif

static struct resource smc91x_resources[] = {
	[0] = {
		.start	= 0x9C004300,
		.end	= 0x9C0043ff,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= MSM_GPIO_TO_INT(132),
		.end	= MSM_GPIO_TO_INT(132),
		.flags	= IORESOURCE_IRQ,
	},
};

#ifdef CONFIG_USB_FUNCTION
static struct usb_mass_storage_platform_data usb_mass_storage_pdata = {
	.nluns          = 0x02,
	.buf_size       = 16384,
	.vendor         = "GOOGLE",
	.product        = "Mass storage",
	.release        = 0xffff,
};

static struct platform_device mass_storage_device = {
	.name           = "usb_mass_storage",
	.id             = -1,
	.dev            = {
		.platform_data          = &usb_mass_storage_pdata,
	},
};
#endif
#ifdef CONFIG_USB_ANDROID
static char *usb_functions_default[] = {
	"usb_mass_storage",
};

static char *usb_functions_default_adb[] = {
	"adb",
	"acm",
	"diag",
	"usb_mass_storage",
};

static char *usb_functions_rndis[] = {
	"rndis",
};

#ifdef CONFIG_USB_ANDROID_ACCESSORY
/* accessory mode */
static char *usb_functions_accessory[] = {
	"accessory",
};

static char *usb_functions_accessory_adb[] = {
	"accessory",
	"adb",
};
#endif

static char *usb_functions_all[] = {
#ifdef CONFIG_USB_ANDROID_ACM
	"acm",
#endif
#ifdef CONFIG_USB_ANDROID_DIAG
	"diag",
#endif
	"usb_mass_storage",
	"adb",
#ifdef CONFIG_USB_F_SERIAL
	"modem",
	"nmea",
#endif
#ifdef CONFIG_USB_ANDROID_RMNET
	"rmnet",
#endif
#ifdef CONFIG_USB_ANDROID_ACCESSORY
	"accessory",
#endif
#ifdef CONFIG_USB_ANDROID_RNDIS
	"rndis",
#endif
};

static struct android_usb_product usb_products[] = {
	{
		.vendor_id	= 0x04e8,
		.product_id	= 0x6881,
		.num_functions	= ARRAY_SIZE(usb_functions_rndis),
		.functions	= usb_functions_rndis,
	},
	{
		.vendor_id	= 0x04e8,
		.product_id	= 0x689e,
		.num_functions	= ARRAY_SIZE(usb_functions_default_adb),
		.functions	= usb_functions_default_adb,
	},
	{
        .vendor_id  = 0x04e8,
		.product_id	= 0x681d,
		.num_functions	= ARRAY_SIZE(usb_functions_default),
		.functions	= usb_functions_default,
	},
#ifdef CONFIG_USB_ANDROID_ACCESSORY
	{
		.vendor_id	= USB_ACCESSORY_VENDOR_ID,
		.product_id     = USB_ACCESSORY_PRODUCT_ID,
		.num_functions  = ARRAY_SIZE(usb_functions_accessory),
		.functions      = usb_functions_accessory,
	},
#endif
};

#ifdef	CONFIG_MACH_COOPER_BASE_KOR
static struct usb_mass_storage_platform_data mass_storage_pdata = {
#if defined(CONFIG_MACH_JUNO_SKT) || defined(CONFIG_MACH_JUNO_KT)
	.nluns		= 2,
#else
	.nluns		= 1,
#endif	
	.vendor		= "SAMSUNG ",
#if defined(CONFIG_MACH_COOPER_SKT)
	.product	= "SHW-M240S Card",
#elif defined(CONFIG_MACH_COOPER_KT)
	.product	= "SHW-M240K Card",
#elif defined(CONFIG_MACH_GIO_SKT)
	.product	= "SHW-M290S Card",
#elif defined(CONFIG_MACH_GIO_KT)
	.product	= "SHW-M290K Card",
#elif defined(CONFIG_MACH_JUNO_SKT)
	.product	= "SHW-M340S",
#elif defined(CONFIG_MACH_JUNO_KT)
	.product	= "SHW-M340K",
#endif
	.release	= 0x0100,
};
#else
static struct usb_mass_storage_platform_data mass_storage_pdata = {
	.nluns		= 1,
	.vendor		= "SAMSUNG ",
	.product        = "GT-S5830 Card",
	.release	= 0x0100,
};
#endif

static struct platform_device usb_mass_storage_device = {
	.name           = "usb_mass_storage",
	.id             = -1,
	.dev            = {
		.platform_data          = &mass_storage_pdata,
	},
};

static struct usb_ether_platform_data rndis_pdata = {
	/* ethaddr is filled by board_serialno_setup */
	.vendorID	= 0x04e8,
	.vendorDescr	= "Qualcomm Incorporated",
};

static struct platform_device rndis_device = {
	.name	= "rndis",
	.id	= -1,
	.dev	= {
		.platform_data = &rndis_pdata,
	},
};

static struct android_usb_platform_data android_usb_pdata = {
	.vendor_id	= 0x04e8,
//	.product_id	= 0x681d,
	.product_id = 0x689e,
	.version	= 0x0100,
	.product_name	= "Samsung Android USB Device",
	.manufacturer_name = "SAMSUNG Electronics Co., Ltd.",
	.num_products = ARRAY_SIZE(usb_products),
	.products = usb_products,
	.num_functions = ARRAY_SIZE(usb_functions_all),
	.functions = usb_functions_all,
	.serial_number = "1234567890ABCDEF",
};

static struct platform_device android_usb_device = {
	.name	= "android_usb",
	.id		= -1,
	.dev		= {
		.platform_data = &android_usb_pdata,
	},
};

static int __init board_serialno_setup(char *serialno)
{
	int i;
	char *src = serialno;

	/* create a fake MAC address from our serial number.
	 * first byte is 0x02 to signify locally administered.
	 */
	rndis_pdata.ethaddr[0] = 0x02;
	for (i = 0; *src; i++) {
		/* XOR the USB serial across the remaining bytes */
		rndis_pdata.ethaddr[i % (ETH_ALEN - 1) + 1] ^= *src++;
	}

	android_usb_pdata.serial_number = serialno;
	return 1;
}
__setup("androidboot.serialno=", board_serialno_setup);
#endif

static struct platform_device smc91x_device = {
	.name		= "smc91x",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(smc91x_resources),
	.resource	= smc91x_resources,
};

#ifdef CONFIG_USB_FUNCTION
static struct usb_function_map usb_functions_map[] = {
	{"diag", 0},
	{"adb", 1},
	{"modem", 2},
	{"nmea", 3},
	{"mass_storage", 4},
	{"ethernet", 5},
	{"rmnet", 6},
};

/* dynamic composition */
static struct usb_composition usb_func_composition[] = {
	{
		.product_id         = 0x9012,
		.functions	    = 0x5, /* 0101 */
	},

	{
		.product_id         = 0x9013,
		.functions	    = 0x15, /* 10101 */
	},

	{
		.product_id         = 0x9014,
		.functions	    = 0x30, /* 110000 */
	},

	{
		.product_id         = 0x9016,
		.functions	    = 0xD, /* 01101 */
	},

	{
		.product_id         = 0x9017,
		.functions	    = 0x1D, /* 11101 */
	},

	{
		.product_id         = 0xF000,
		.functions	    = 0x10, /* 10000 */
	},

	{
		.product_id         = 0xF009,
		.functions	    = 0x20, /* 100000 */
	},

	{
		.product_id         = 0x9018,
		.functions	    = 0x1F, /* 011111 */
	},
#ifdef CONFIG_USB_FUNCTION_RMNET
	{
		.product_id         = 0x9021,
		/* DIAG + RMNET */
		.functions	    = 0x41,
	},
	{
		.product_id         = 0x9022,
		/* DIAG + ADB + RMNET */
		.functions	    = 0x43,
	},
#endif

};

static struct msm_hsusb_platform_data msm_hsusb_pdata = {
	.version	= 0x0100,
	.phy_info	= (USB_PHY_INTEGRATED | USB_PHY_MODEL_65NM),
	.vendor_id          = 0x5c6,
	.product_name       = "Qualcomm HSUSB Device",
	.serial_number      = "1234567890ABCDEF",
	.manufacturer_name  = "Qualcomm Incorporated",
	.compositions	= usb_func_composition,
	.num_compositions = ARRAY_SIZE(usb_func_composition),
	.function_map   = usb_functions_map,
	.num_functions	= ARRAY_SIZE(usb_functions_map),
	.config_gpio    = NULL,
};
#endif

#ifdef CONFIG_USB_EHCI_MSM
static void msm_hsusb_vbus_power(unsigned phy_info, int on)
{
	if (on)
		msm_hsusb_vbus_powerup();
	else
		msm_hsusb_vbus_shutdown();
}

static struct msm_usb_host_platform_data msm_usb_host_pdata = {
	.phy_info       = (USB_PHY_INTEGRATED | USB_PHY_MODEL_65NM),
};

static void __init msm7x2x_init_host(void)
{
	if (machine_is_msm7x25_ffa() || machine_is_msm7x27_ffa())
		return;

	msm_add_host(0, &msm_usb_host_pdata);
}
#endif

#ifdef CONFIG_USB_MSM_OTG_72K
static int hsusb_rpc_connect(int connect)
{
	if (connect)
		return msm_hsusb_rpc_connect();
	else
		return msm_hsusb_rpc_close();
}
#endif

#ifdef CONFIG_USB_MSM_OTG_72K
struct vreg *vreg_3p3;
static int msm_hsusb_ldo_init(int init)
{
	if (init) {
		/*
		 * PHY 3.3V analog domain(VDDA33) is powered up by
		 * an always enabled power supply (LP5900TL-3.3).
		 * USB VREG default source is VBUS line. Turning
		 * on USB VREG has a side effect on the USB suspend
		 * current. Hence USB VREG is explicitly turned
		 * off here.
		 */
		vreg_3p3 = vreg_get(NULL, "usb");
		if (IS_ERR(vreg_3p3))
			return PTR_ERR(vreg_3p3);
		vreg_enable(vreg_3p3);
		vreg_disable(vreg_3p3);
		vreg_put(vreg_3p3);
}

		return 0;
}

static int msm_hsusb_pmic_notif_init(void (*callback)(int online), int init)
{
	int ret;

	if (init) {
		ret = msm_pm_app_rpc_init(callback);
	} else {
		msm_pm_app_rpc_deinit(callback);
		ret = 0;
	}
	return ret;
}

static int msm_otg_rpc_phy_reset(void __iomem *regs)
{
	return msm_hsusb_phy_reset();
}

static struct msm_otg_platform_data msm_otg_pdata = {
	.rpc_connect	= hsusb_rpc_connect,
	.pmic_vbus_notif_init         = msm_hsusb_pmic_notif_init,
	.chg_vbus_draw		 = hsusb_chg_vbus_draw,
	.chg_connected		 = hsusb_chg_connected,
	.chg_init		 = hsusb_chg_init,
#ifdef CONFIG_USB_EHCI_MSM
	.vbus_power = msm_hsusb_vbus_power,
#endif
	.ldo_init		= msm_hsusb_ldo_init,
	.pclk_required_during_lpm = 1,
	.pclk_src_name		= "ebi1_usb_clk",
};

#ifdef CONFIG_USB_GADGET
static struct msm_hsusb_gadget_platform_data msm_gadget_pdata;
#endif
#endif

#define SND(desc, num) { .name = #desc, .id = num }
static struct snd_endpoint snd_endpoints_list[] = {
	SND(HANDSET, 0),
	SND(MONO_HEADSET, 2),
	SND(HEADSET, 3),
	SND(SPEAKER, 6),
	SND(TTY_HEADSET, 8),
	SND(TTY_VCO, 9),
	SND(TTY_HCO, 10),
	SND(BT, 12),
	SND(STEREO_USB, 15),  // recording
	SND(IN_S_SADC_OUT_HANDSET, 16),
	SND(IN_S_SADC_OUT_HEADSET, 17),
	SND(BT_CONFERENCE, 24), // voice recognition
	SND(IN_S_SADC_OUT_SPEAKER_PHONE, 25),
	SND(FORCE_SPEAKER, 26),
	SND(FM_SPEAKER, 27),
	SND(NO_MIC_HEADSET, 29),
	SND(MEDIA_SPEAKER, 30),
	SND(MEDIA_STEREO_HEADSET, 31),
	SND(BT_NSEC_OFF, 32),	
	SND(CURRENT, 34),
};
#undef SND

static struct msm_snd_endpoints msm_device_snd_endpoints = {
	.endpoints = snd_endpoints_list,
	.num = sizeof(snd_endpoints_list) / sizeof(struct snd_endpoint)
};

static struct platform_device msm_device_snd = {
	.name = "msm_snd",
	.id = -1,
	.dev    = {
		.platform_data = &msm_device_snd_endpoints
	},
};

#define DEC0_FORMAT ((1<<MSM_ADSP_CODEC_MP3)| \
	(1<<MSM_ADSP_CODEC_AAC)|(1<<MSM_ADSP_CODEC_WMA)| \
	(1<<MSM_ADSP_CODEC_WMAPRO)|(1<<MSM_ADSP_CODEC_AMRWB)| \
	(1<<MSM_ADSP_CODEC_AMRNB)|(1<<MSM_ADSP_CODEC_WAV)| \
	(1<<MSM_ADSP_CODEC_ADPCM)|(1<<MSM_ADSP_CODEC_YADPCM)| \
	(1<<MSM_ADSP_CODEC_EVRC)|(1<<MSM_ADSP_CODEC_QCELP))
#ifdef CONFIG_ARCH_MSM7X25
#define DEC1_FORMAT ((1<<MSM_ADSP_CODEC_WAV)|(1<<MSM_ADSP_CODEC_ADPCM)| \
	(1<<MSM_ADSP_CODEC_YADPCM)|(1<<MSM_ADSP_CODEC_QCELP)| \
	(1<<MSM_ADSP_CODEC_MP3))
#define DEC2_FORMAT ((1<<MSM_ADSP_CODEC_WAV)|(1<<MSM_ADSP_CODEC_ADPCM)| \
	(1<<MSM_ADSP_CODEC_YADPCM)|(1<<MSM_ADSP_CODEC_QCELP)| \
	(1<<MSM_ADSP_CODEC_MP3))
#define DEC3_FORMAT 0
#define DEC4_FORMAT 0
#else
#define DEC1_FORMAT ((1<<MSM_ADSP_CODEC_MP3)| \
	(1<<MSM_ADSP_CODEC_AAC)|(1<<MSM_ADSP_CODEC_WMA)| \
	(1<<MSM_ADSP_CODEC_WMAPRO)|(1<<MSM_ADSP_CODEC_AMRWB)| \
	(1<<MSM_ADSP_CODEC_AMRNB)|(1<<MSM_ADSP_CODEC_WAV)| \
	(1<<MSM_ADSP_CODEC_ADPCM)|(1<<MSM_ADSP_CODEC_YADPCM)| \
	(1<<MSM_ADSP_CODEC_EVRC)|(1<<MSM_ADSP_CODEC_QCELP))
#define DEC2_FORMAT ((1<<MSM_ADSP_CODEC_MP3)| \
	(1<<MSM_ADSP_CODEC_AAC)|(1<<MSM_ADSP_CODEC_WMA)| \
	(1<<MSM_ADSP_CODEC_WMAPRO)|(1<<MSM_ADSP_CODEC_AMRWB)| \
	(1<<MSM_ADSP_CODEC_AMRNB)|(1<<MSM_ADSP_CODEC_WAV)| \
	(1<<MSM_ADSP_CODEC_ADPCM)|(1<<MSM_ADSP_CODEC_YADPCM)| \
	(1<<MSM_ADSP_CODEC_EVRC)|(1<<MSM_ADSP_CODEC_QCELP))
#define DEC3_FORMAT ((1<<MSM_ADSP_CODEC_MP3)| \
	(1<<MSM_ADSP_CODEC_AAC)|(1<<MSM_ADSP_CODEC_WMA)| \
	(1<<MSM_ADSP_CODEC_WMAPRO)|(1<<MSM_ADSP_CODEC_AMRWB)| \
	(1<<MSM_ADSP_CODEC_AMRNB)|(1<<MSM_ADSP_CODEC_WAV)| \
	(1<<MSM_ADSP_CODEC_ADPCM)|(1<<MSM_ADSP_CODEC_YADPCM)| \
	(1<<MSM_ADSP_CODEC_EVRC)|(1<<MSM_ADSP_CODEC_QCELP))
#define DEC4_FORMAT (1<<MSM_ADSP_CODEC_MIDI)
#endif

static unsigned int dec_concurrency_table[] = {
	/* Audio LP */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DMA)), 0,
	0, 0, 0,

	/* Concurrency 1 */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC1_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC2_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC3_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC4_FORMAT),

	 /* Concurrency 2 */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC1_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC2_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC3_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC4_FORMAT),

	/* Concurrency 3 */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC1_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC2_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC3_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC4_FORMAT),

	/* Concurrency 4 */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC1_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC2_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC3_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC4_FORMAT),

	/* Concurrency 5 */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC1_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC2_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC3_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC4_FORMAT),

	/* Concurrency 6 */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	0, 0, 0, 0,

	/* Concurrency 7 */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC1_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC2_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC3_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC4_FORMAT),
};

#define DEC_INFO(name, queueid, decid, nr_codec) { .module_name = name, \
	.module_queueid = queueid, .module_decid = decid, \
	.nr_codec_support = nr_codec}

static struct msm_adspdec_info dec_info_list[] = {
	DEC_INFO("AUDPLAY0TASK", 13, 0, 11), /* AudPlay0BitStreamCtrlQueue */
#ifdef CONFIG_ARCH_MSM7X25
	DEC_INFO("AUDPLAY1TASK", 14, 1, 5),  /* AudPlay1BitStreamCtrlQueue */
	DEC_INFO("AUDPLAY2TASK", 15, 2, 5),  /* AudPlay2BitStreamCtrlQueue */
	DEC_INFO("AUDPLAY3TASK", 16, 3, 0),  /* AudPlay3BitStreamCtrlQueue */
	DEC_INFO("AUDPLAY4TASK", 17, 4, 0),  /* AudPlay4BitStreamCtrlQueue */
#else
	DEC_INFO("AUDPLAY1TASK", 14, 1, 11),  /* AudPlay1BitStreamCtrlQueue */
	DEC_INFO("AUDPLAY2TASK", 15, 2, 11),  /* AudPlay2BitStreamCtrlQueue */
	DEC_INFO("AUDPLAY3TASK", 16, 3, 11),  /* AudPlay3BitStreamCtrlQueue */
	DEC_INFO("AUDPLAY4TASK", 17, 4, 1),  /* AudPlay4BitStreamCtrlQueue */
#endif
};

static struct msm_adspdec_database msm_device_adspdec_database = {
	.num_dec = ARRAY_SIZE(dec_info_list),
	.num_concurrency_support = (ARRAY_SIZE(dec_concurrency_table) / \
					ARRAY_SIZE(dec_info_list)),
	.dec_concurrency_table = dec_concurrency_table,
	.dec_info_list = dec_info_list,
};

static struct platform_device msm_device_adspdec = {
	.name = "msm_adspdec",
	.id = -1,
	.dev    = {
		.platform_data = &msm_device_adspdec_database
	},
};

static struct android_pmem_platform_data android_pmem_kernel_ebi1_pdata = {
	.name = PMEM_KERNEL_EBI1_DATA_NAME,
	/* if no allocator_type, defaults to PMEM_ALLOCATORTYPE_BITMAP,
	 * the only valid choice at this time. The board structure is
	 * set to all zeros by the C runtime initialization and that is now
	 * the enum value of PMEM_ALLOCATORTYPE_BITMAP, now forced to 0 in
	 * include/linux/android_pmem.h.
	 */
	.cached = 0,
};

static struct android_pmem_platform_data android_pmem_pdata = {
	.name = "pmem",
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 1,
};

static struct android_pmem_platform_data android_pmem_adsp_pdata = {
	.name = "pmem_adsp",
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 0,
};

static struct platform_device android_pmem_device = {
	.name = "android_pmem",
	.id = 0,
	.dev = { .platform_data = &android_pmem_pdata },
};

static struct platform_device android_pmem_adsp_device = {
	.name = "android_pmem",
	.id = 1,
	.dev = { .platform_data = &android_pmem_adsp_pdata },
};

static struct platform_device android_pmem_kernel_ebi1_device = {
	.name = "android_pmem",
	.id = 4,
	.dev = { .platform_data = &android_pmem_kernel_ebi1_pdata },
};

static struct msm_handset_platform_data hs_platform_data = {
	.hs_name = "7k_handset",
	.pwr_key_delay_ms = 500, /* 0 will disable end key */
};

static struct platform_device hs_device = {
	.name   = "msm-handset",
	.id     = -1,
	.dev    = {
		.platform_data = &hs_platform_data,
	},
};


#define LCDC_CONFIG_PROC          21
#define LCDC_UN_CONFIG_PROC       22
#define LCDC_API_PROG             0x30000066
#define LCDC_API_VERS             0x00010001

#define GPIO_OUT_132    132
#define GPIO_OUT_131    131
#if defined(CONFIG_FB_MSM_LCDC_TA8566_WQVGA)
#define GPIO_OUT_103    102
#else
#define GPIO_OUT_103    103  
#define GPIO_OUT_102    102
#endif
//#define GPIO_OUT_129    129
#define GPIO_OUT_101    101

static struct msm_rpc_endpoint *lcdc_ep;

static int msm_fb_lcdc_config(int on)
{
	int rc = 0;
	struct rpc_request_hdr hdr;

	// for avoiding potential rpc error
	return 0;

	if (on)
		pr_info("lcdc config\n");
	else
		pr_info("lcdc un-config\n");

	lcdc_ep = msm_rpc_connect_compatible(LCDC_API_PROG, LCDC_API_VERS, 0);
	if (IS_ERR(lcdc_ep)) {
		printk(KERN_ERR "%s: msm_rpc_connect failed! rc = %ld\n",
			__func__, PTR_ERR(lcdc_ep));
		return -EINVAL;
	}

	rc = msm_rpc_call(lcdc_ep,
				(on) ? LCDC_CONFIG_PROC : LCDC_UN_CONFIG_PROC,
				&hdr, sizeof(hdr),
				5 * HZ);
	if (rc)
		printk(KERN_ERR
			"%s: msm_rpc_call failed! rc = %d\n", __func__, rc);

	msm_rpc_close(lcdc_ep);
	return rc;
}

static int gpio_array_num[] = {
				GPIO_OUT_132, /* spi_clk */
				GPIO_OUT_131, /* spi_cs  */
				GPIO_OUT_103, /* spi_sdi */
				GPIO_OUT_102, /* spi_sdo */
//				GPIO_OUT_129, /* lcd_en */
				GPIO_OUT_101,  /* lcd_reset */
				};

static void lcdc_gordon_gpio_init(void)
{
	if (gpio_request(GPIO_OUT_132, "spi_clk"))
		pr_err("failed to request gpio spi_clk\n");
	if (gpio_request(GPIO_OUT_131, "spi_cs"))
		pr_err("failed to request gpio spi_cs\n");
	if (gpio_request(GPIO_OUT_103, "spi_sdi"))
		pr_err("failed to request gpio spi_sdi\n");
	if (gpio_request(GPIO_OUT_102, "spi_sdo"))
		pr_err("failed to request gpio spi_sdo\n");
//	if (gpio_request(GPIO_OUT_129, "lcd_en"))
//		pr_err("failed to request gpio lcd_en\n");
	if (gpio_request(GPIO_OUT_101, "lcd_reset"))
		pr_err("failed to request gpio lcd_reset\n");
}

static uint32_t lcdc_gpio_table[] = {
	GPIO_CFG(GPIO_OUT_132, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(GPIO_OUT_131, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(GPIO_OUT_103, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(GPIO_OUT_102, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
//	GPIO_CFG(GPIO_OUT_129, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(GPIO_OUT_101,  0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
};

static void config_lcdc_gpio_table(uint32_t *table, int len, unsigned enable)
{
	int n, rc;

/* 20100823 hongkuk.son */
#if 0
	// for avoiding potential rpc error
	return;
#endif
/***/

	for (n = 0; n < len; n++) {
		rc = gpio_tlmm_config(table[n],
			enable ? GPIO_CFG_ENABLE : GPIO_CFG_DISABLE);
		if (rc) {
			printk(KERN_ERR "%s: gpio_tlmm_config(%#x)=%d\n",
				__func__, table[n], rc);
			break;
		}
	}
}

static char *msm_fb_lcdc_vreg[] = {
	"ldo3"
};

static char *msm_fb_lcdc_vreg_rev02[] = {
	"ldo4"
};

#define MSM_FB_LCDC_VREG_OP(name, op) \
do { \
	vreg = vreg_get(0, name); \
	if (vreg_##op(vreg)) \
		printk(KERN_ERR "%s: %s vreg operation failed \n", \
			(vreg_##op == vreg_enable) ? "vreg_enable" \
				: "vreg_disable", name); \
} while (0)

static int msm_fb_lcdc_power_save(int on)
{
/*
	struct vreg *vreg;
	int i;
	int array_size = 0;

	if( board_hw_revision >= 0x3 )
		array_size = ARRAY_SIZE(msm_fb_lcdc_vreg_rev02);
	else
		array_size = ARRAY_SIZE(msm_fb_lcdc_vreg);

	for (i = 0; i < array_size; i++) {
		if (on) {
			if( board_hw_revision >= 0x3 )
				MSM_FB_LCDC_VREG_OP(msm_fb_lcdc_vreg_rev02[i], enable);
			else
			MSM_FB_LCDC_VREG_OP(msm_fb_lcdc_vreg[i], enable);
			vreg_set_level(vreg, OUT3000mV);
			}
		else{
			if( board_hw_revision >= 0x3 )
				MSM_FB_LCDC_VREG_OP(msm_fb_lcdc_vreg_rev02[i], disable);
			else
			MSM_FB_LCDC_VREG_OP(msm_fb_lcdc_vreg[i], disable);
			// for avoiding potential rpc error
			//gpio_tlmm_config(GPIO_CFG(GPIO_OUT_101, 0,
			//GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), GPIO_ENABLE);
// 20100823 hongkuk.son 
			gpio_tlmm_config(GPIO_CFG(GPIO_OUT_101, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), GPIO_ENABLE);
//
			gpio_set_value(101, 0);
			mdelay(15);
			
			gpio_set_value(101, 1);
			mdelay(15);
		}
	}
*/
}
static struct lcdc_platform_data lcdc_pdata = {
	.lcdc_gpio_config = msm_fb_lcdc_config,
	.lcdc_power_save   = msm_fb_lcdc_power_save,
};

#if defined(CONFIG_FB_MSM_LCDC_S6D04M0_QVGA)
static void lcdc_s6d04m0_config_gpios(int enable)
{
	config_lcdc_gpio_table(lcdc_gpio_table,
		ARRAY_SIZE(lcdc_gpio_table), enable);
}

static struct msm_panel_common_pdata lcdc_s6d04m0_panel_data = {
	.panel_config_gpio = lcdc_s6d04m0_config_gpios,
	.gpio_num          = gpio_array_num,
};

static struct platform_device lcdc_s6d04m0_panel_device = {
	.name   = "lcdc_s6d04m0_qvga",
	.id     = 0,
	.dev    = {
		.platform_data = &lcdc_s6d04m0_panel_data,
	}
};
#endif

#if defined(CONFIG_FB_MSM_LCDC_TA8566_WQVGA)
static void lcdc_ta8566_config_gpios(int enable)
{
	config_lcdc_gpio_table(lcdc_gpio_table,
		ARRAY_SIZE(lcdc_gpio_table), enable);
}

static struct msm_panel_common_pdata lcdc_ta8566_panel_data = {
	.panel_config_gpio = lcdc_ta8566_config_gpios,
	.gpio_num          = gpio_array_num,
};

static struct platform_device lcdc_ta8566_panel_device = {
	.name   = "lcdc_ta8566_wqvga",
	.id     = 0,
	.dev    = {
		.platform_data = &lcdc_ta8566_panel_data,
	}
};
#endif


/* 20100823 hongkuk.son */
#if defined(CONFIG_FB_MSM_LCDC_S6D16A0X_HVGA)
static void lcdc_s6d16a0x_config_gpios(int enable)
{
	config_lcdc_gpio_table(lcdc_gpio_table,
		ARRAY_SIZE(lcdc_gpio_table), enable);
}

static struct msm_panel_common_pdata lcdc_s6d16a0x_panel_data = {
	.panel_config_gpio = lcdc_s6d16a0x_config_gpios,
	.gpio_num          = gpio_array_num,
};

static struct platform_device lcdc_s6d16a0x_panel_device = {
	.name   = "lcdc_s6d16a0x_hvga",
	.id     = 0,
	.dev    = {
		.platform_data = &lcdc_s6d16a0x_panel_data,
	}
};
#endif
/***/
#if defined(CONFIG_FB_MSM_LCDC_S6D04D1_WQVGA)
static void lcdc_s6d04d1_config_gpios(int enable)
{
	config_lcdc_gpio_table(lcdc_gpio_table,
		ARRAY_SIZE(lcdc_gpio_table), enable);
}

static struct msm_panel_common_pdata lcdc_s6d04d1_panel_data = {
	.panel_config_gpio = lcdc_s6d04d1_config_gpios,
	.gpio_num          = gpio_array_num,
};

static struct platform_device lcdc_s6d04d1_panel_device = {
	.name   = "lcdc_s6d04d1_wqvga",
	.id     = 0,
	.dev    = {
		.platform_data = &lcdc_s6d04d1_panel_data,
	}
};
#endif

#ifdef CONFIG_FB_MSM_LCDC_S6E63M0_WVGA_PANEL
static int lcdc_gpio_array_num_s6e63m0[] = {
				GPIO_OUT_132, /* spi_clk */
				GPIO_OUT_131, /* spi_cs  */
				GPIO_OUT_103, /* spi_sdi */
				GPIO_OUT_101,  /* lcd_reset */
				};

static uint32_t lcdc_gpio_table_s6e63m0[] = {
	GPIO_CFG(GPIO_OUT_132, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(GPIO_OUT_131, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(GPIO_OUT_103, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(GPIO_OUT_101, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
};

static void lcdc_s6d04m0_config_gpios(int enable)
{
	config_lcdc_gpio_table(lcdc_gpio_table_s6e63m0,
		ARRAY_SIZE(lcdc_gpio_table_s6e63m0), enable);
}

static struct msm_panel_common_pdata lcdc_panel_data = {
	.panel_config_gpio = lcdc_s6d04m0_config_gpios,
	.gpio_num          = lcdc_gpio_array_num_s6e63m0,
};

static struct platform_device lcdc_s6e63m0_panel_device = {
	.name   = "lcdc_s6e63m0_wvga",
	.id     = 0,
	.dev    = {
		.platform_data = &lcdc_panel_data,
	}
};
#endif

static struct resource msm_fb_resources[] = {
	{
		.flags  = IORESOURCE_DMA,
	}
};

static int msm_fb_detect_panel(const char *name)
{
	int ret = -EPERM;

//	if (machine_is_msm7x25_ffa() || machine_is_msm7x27_ffa()) {
//		if (!strcmp(name, "lcdc_gordon_vga"))
#if defined(CONFIG_FB_MSM_LCDC_S6D04M0_QVGA)
		if (!strcmp(name, "lcdc_s6d04m0_qvga"))
			ret = 0;
		else
			ret = -ENODEV;
#endif
#if defined(CONFIG_FB_MSM_LCDC_S6D04D1_WQVGA)
		if (!strcmp(name, "lcdc_s6d04d1_wqvga"))
			ret = 0;
		else
			ret = -ENODEV;
#endif
#if defined(CONFIG_FB_MSM_LCDC_TA8566_WQVGA)
		if (!strcmp(name, "lcdc_ta8566_wqvga"))
			ret = 0;
		else
			ret = -ENODEV;
#endif
/* 20100823 hongkuk.son */
#if defined(CONFIG_FB_MSM_LCDC_S6D16A0X_HVGA)
		if (!strcmp(name, "lcdc_s6d16a0x_hvga"))
			ret = 0;
		else
			ret = -ENODEV;
#endif
/***/
#if defined (CONFIG_FB_MSM_LCDC_S6E63M0_WVGA_PANEL)
		if (!strcmp(name, "lcdc_s6e63m0_wvga"))
			ret = 0;
		else
			ret = -ENODEV;
#endif
//	}

	return ret;
}

static struct msm_fb_platform_data msm_fb_pdata = {
	.detect_client = msm_fb_detect_panel,
	.mddi_prescan = 1,
};

static struct platform_device msm_fb_device = {
	.name   = "msm_fb",
	.id     = 0,
	.num_resources  = ARRAY_SIZE(msm_fb_resources),
	.resource       = msm_fb_resources,
	.dev    = {
		.platform_data = &msm_fb_pdata,
	}
};

//sec: sm.kim
static void keypad_gpio_control(struct platform_device *pdev)
{
	int i, ret;
	int config;
	struct gpio_event_platform_data *p = (struct gpio_event_platform_data *)pdev->dev.platform_data;
        struct gpio_event_info *info = *p->info;
        struct gpio_event_matrix_info *minfo = container_of(info, struct gpio_event_matrix_info, info);

	for(i=0; i<minfo->noutputs; i++) // KBC
		{
		config = GPIO_CFG(minfo->output_gpios[i], 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA);
		ret = gpio_tlmm_config(config, GPIO_CFG_ENABLE);
		if (ret) 
			printk(KERN_ERR "%s: gpio_tlmm_config(%#x)=%d\n", __func__, minfo->output_gpios[i], ret);
		}
	for(i=0; i<minfo->ninputs; i++)	// KBR
		{
//		config = GPIO_CFG(minfo->input_gpios[i], 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA);
		config = GPIO_CFG(minfo->input_gpios[i], 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA);
		ret = gpio_tlmm_config(config, GPIO_CFG_ENABLE);
		if (ret) 
			printk(KERN_ERR "%s: gpio_tlmm_config(%#x)=%d\n", __func__, minfo->output_gpios[i], ret);
		}
	// for COOPER
	config = GPIO_CFG(83, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA);
	ret = gpio_tlmm_config(config, GPIO_CFG_ENABLE);
	if (ret) 
		printk(KERN_ERR "%s: gpio_tlmm_config(%#x)=%d\n", __func__, minfo->output_gpios[i], ret);

}

#ifdef CONFIG_BT
static struct platform_device msm_bt_power_device = {
	.name = "bt_power",
};

enum {
	BT_WAKE,
	BT_RFR,
	BT_CTS,
	BT_RX,
	BT_TX,
	BT_PCM_DOUT,
	BT_PCM_DIN,
	BT_PCM_SYNC,
	BT_PCM_CLK,
	BT_HOST_WAKE,
};

static unsigned bt_config_power_on[] = {
	GPIO_CFG(43, 2, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* RFR */
	GPIO_CFG(44, 2, GPIO_CFG_INPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* CTS */
	GPIO_CFG(45, 2, GPIO_CFG_INPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* Rx */
	GPIO_CFG(46, 3, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* Tx */
	GPIO_CFG(68, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* PCM_DOUT */
	GPIO_CFG(69, 1, GPIO_CFG_INPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* PCM_DIN */
	GPIO_CFG(70, 2, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* PCM_SYNC */
	GPIO_CFG(71, 2, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* PCM_CLK */
};
static unsigned bt_config_power_off[] = {
	GPIO_CFG(43, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* RFR */
	GPIO_CFG(44, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* CTS */
	GPIO_CFG(45, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* Rx */
	GPIO_CFG(46, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* Tx */
	GPIO_CFG(68, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* PCM_DOUT */
	GPIO_CFG(69, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* PCM_DIN */
	GPIO_CFG(70, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* PCM_SYNC */
	GPIO_CFG(71, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* PCM_CLK */
};

void wlan_setup_clock(int on);
extern int bluesleep_start(void);
extern void bluesleep_stop(void);

#if defined(CONFIG_MACH_COOPER) || defined(CONFIG_MACH_COOPER_BASE_KOR)
static void bluetooth_setup_power(int on)
{
    #if 0
	struct vreg *vreg_bt_2_6v;	

	vreg_bt_2_6v = vreg_get(NULL, "ldo14");
	if (IS_ERR(vreg_bt_2_6v)) {
	printk(KERN_ERR "%s: vreg get failed (%ld)\n",
		   __func__, PTR_ERR(vreg_bt_2_6v));
	return;
	}

	printk("%s %s --enter\n", __func__, on ? "on" : "down");

	if (on) {
		vreg_set_level(vreg_bt_2_6v, OUT2600mV);
		vreg_enable(vreg_bt_2_6v);		

		/* additional delay for power on */
		//mdelay(20);
	}
	else
	{
		/* power off for sleep current */
		vreg_disable(vreg_bt_2_6v);
	}
	#else
	return;
	#endif
}
#endif

static int bluetooth_power(int on)
{
	int pin, rc;
	printk("%s %s --enter\n", __func__, on ? "on" : "down");
    
	if (on) {
		//bt set up
		/* If WiFi isn't working, 
		   we should turn on the power for the clock supplied to BT */
		//if (gpio_get_value(gpio_wlan_reset_n) == 0)
		//	wlan_setup_clock(1);
		
		bt_power_on_enter = 1; //bt power on
		
        printk("%s ON --HERE \n", __func__);
		for (pin = 0; pin < ARRAY_SIZE(bt_config_power_on); pin++) {
			rc = gpio_tlmm_config(bt_config_power_on[pin],
					      GPIO_CFG_ENABLE);
			if (rc) {
				printk(KERN_ERR
				       "%s: gpio_tlmm_config(%#x)=%d\n",
				       __func__, bt_config_power_on[pin], rc);
				return -EIO;
			}
		}

#if defined(CONFIG_MACH_COOPER) || defined(CONFIG_MACH_COOPER_BASE_KOR)
		bluetooth_setup_power(1); //jh8181.choi
#endif
		//bt set up
		//gpio_configure(BT_PWR, GPIOF_DRIVE_OUTPUT | GPIOF_OUTPUT_HIGH);	/* BT_VREG_CTL */

		//mdelay(100);
		
        printk("%s %d ON --HERE222 \n", __func__ , BT_RESET);
		gpio_tlmm_config(GPIO_CFG(BT_WAKE_PIN ,0,GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, 
			GPIO_CFG_16MA), GPIO_CFG_ENABLE);
		gpio_set_value(BT_WAKE_PIN, 1);	
		gpio_tlmm_config(GPIO_CFG(WLAN_BT_EN ,0,GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, 
			GPIO_CFG_16MA), GPIO_CFG_ENABLE);
		gpio_set_value(WLAN_BT_EN, 1);	
		mdelay(150);
		gpio_tlmm_config(GPIO_CFG(BT_RESET ,0,GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, 
			GPIO_CFG_16MA), GPIO_CFG_ENABLE);
		gpio_set_value(BT_RESET, 1);
        mdelay(100);  //Delay between turning bluetooth power and starting bluesleep

			
		//gpio_set_value(WLAN_BT_EN, 1);
		//mdelay(100);
		bluesleep_start();
		//mdelay(100);
		//gpio_set_value(BT_RESET, 1);  /* BT_RESET */	
		
		}
	else {
		bluesleep_stop();

		printk("%s DOWN  --HERE \n", __func__);
		
		gpio_set_value(BT_RESET, 0);  /* BT_RESET */

//		gpio_set_value(WLAN_BT_EN,0);
		
		if(gpio_get_value(gpio_wlan_reset_n) == 0)			
		{					 
			gpio_set_value(WLAN_BT_EN,0);  /* WLAN_BT_REG_EN */
			mdelay(150);
		}
		gpio_set_value(BT_WAKE_PIN, 0);  /* BT_WAKE */
        if(bt_power_on_enter == 1){ //bt power on
			for (pin = 0; pin < ARRAY_SIZE(bt_config_power_off); pin++) {		
				rc = gpio_tlmm_config(bt_config_power_off[pin],
						      GPIO_CFG_ENABLE);
				if (rc) {
					printk(KERN_ERR
					       "%s: gpio_tlmm_config(%#x)=%d\n",
					       __func__, bt_config_power_off[pin], rc);
					return -EIO;
				}
			}
        }
#if defined(CONFIG_MACH_COOPER) || defined(CONFIG_MACH_COOPER_BASE_KOR)
		bluetooth_setup_power(0); //jh8181.choi
#endif
	}
	return 0;
}

static void __init bt_power_init(void)
{
	msm_bt_power_device.dev.platform_data = &bluetooth_power;
}
#if defined(CONFIG_MACH_JUNO_SKT) || defined(CONFIG_MACH_JUNO_KT)
static struct resource bluesleep_resources[] = {
	{
		.name	= "gpio_host_wake",
		.start	= 77,
		.end	= 77,
		.flags	= IORESOURCE_IO,
	},
	{
		.name	= "gpio_ext_wake",
		.start	= 57,
		.end	= 57,
		.flags	= IORESOURCE_IO,
	},
	{
		.name	= "host_wake",
		.start	= MSM_GPIO_TO_INT(77),
		.end	= MSM_GPIO_TO_INT(77),
		.flags	= IORESOURCE_IRQ,
	},
};
#else
static struct resource bluesleep_resources[] = {
	{
		.name	= "gpio_host_wake",
		.start	= 20,
		.end	= 20,
		.flags	= IORESOURCE_IO,
	},
	{
		.name	= "gpio_ext_wake",
		.start	= 57,
		.end	= 57,
		.flags	= IORESOURCE_IO,
	},
	{
		.name	= "host_wake",
		.start	= MSM_GPIO_TO_INT(20),
		.end	= MSM_GPIO_TO_INT(20),
		.flags	= IORESOURCE_IRQ,
	},
};

#endif

static struct platform_device msm_bluesleep_device = {
	.name = "bluesleep",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(bluesleep_resources),
	.resource	= bluesleep_resources,
};
#else
#define bt_power_init(x) do {} while (0)
#endif

#ifdef CONFIG_ARCH_MSM7X27

static struct resource kgsl_3d0_resources[] = {
         {
                 .name  = KGSL_3D0_REG_MEMORY,
                 .start = 0xA0000000,
                 .end = 0xA001ffff,
                 .flags = IORESOURCE_MEM,
         },
         {
                 .name = KGSL_3D0_IRQ,
                 .start = INT_GRAPHICS,
                 .end = INT_GRAPHICS,
                 .flags = IORESOURCE_IRQ,
         },
};

static struct kgsl_device_platform_data kgsl_3d0_pdata = {
	.pwr_data = {
		.pwrlevel = {
			{
				.gpu_freq = 128000000,
				.bus_freq = 128000000,
			},
		},
		.init_level = 0,
		.num_levels = 1,
		.set_grp_async = NULL,
		.idle_timeout = HZ/5,
		.nap_allowed = true,
	},
	.clk = {
		.name = {
			.clk = "grp_clk",
			.pclk = "grp_pclk",
		},
	},
	.imem_clk_name = {
		.clk = "imem_clk",
		.pclk = NULL,
	},
};

struct platform_device msm_kgsl_3d0 = {
         .name = "kgsl-3d0",
         .id = 0,
         .num_resources = ARRAY_SIZE(kgsl_3d0_resources),
         .resource = kgsl_3d0_resources,
         .dev = {
                 .platform_data = &kgsl_3d0_pdata,
         },
};
#endif

static struct platform_device msm_device_pmic_leds = {
	.name   = "pmic-leds",
	.id = -1,
};

static struct i2c_board_info i2c_devices[] = {
#ifdef CONFIG_S5K5CA //PGH
	{
		I2C_BOARD_INFO("s5k5ca", 0x78>>1),
	},
#endif
#ifdef CONFIG_SR200PC10 //PGH
	{
		I2C_BOARD_INFO("sr200pc10", 0x40>>1),
	},
#endif
#ifdef CONFIG_S5K4CA //PGH
	{
		I2C_BOARD_INFO("s5k4ca", 0x5A>>1),
	},
#endif
#ifdef CONFIG_MT9D112
	{
		I2C_BOARD_INFO("mt9d112", 0x78 >> 1),
	},
#endif	
#ifdef CONFIG_S5K3E2FX
	{
		I2C_BOARD_INFO("s5k3e2fx", 0x20 >> 1),
	},
#endif
#ifdef CONFIG_MT9P012
	{
		I2C_BOARD_INFO("mt9p012", 0x6C >> 1),
	},
#endif
#ifdef CONFIG_MT9P012_KM
	{
		I2C_BOARD_INFO("mt9p012_km", 0x6C >> 2),
	},
#endif
#if defined(CONFIG_MT9T013) || defined(CONFIG_SENSORS_MT9T013)
	{
		I2C_BOARD_INFO("mt9t013", 0x6C),
	},
#endif
#ifdef CONFIG_VB6801
	{
		I2C_BOARD_INFO("vb6801", 0x20),
	},
#endif
#ifdef CONFIG_SENSORS_MMC31XX
	{
		I2C_BOARD_INFO(MMC31XX_I2C_NAME,  MMC31XX_I2C_ADDR),
	},
#endif
#ifdef CONFIG_SENSORS_BMA_ACCEL
{
		I2C_BOARD_INFO("bma_accel",0x08), // [HSS]  0X18 => 0X08 (2010.09.29)
},
#endif
#ifdef CONFIG_SENSORS_KR3D_ACCEL
{
		I2C_BOARD_INFO("kr3dm_accel",KR3DM_I2C_ADDR >> 1),
},
#endif
#ifdef CONFIG_AMP_MAX97000
{
		I2C_BOARD_INFO("max97000",0x9A>>1),
},
#endif
#ifdef CONFIG_RADIO_SI4709
{
		I2C_BOARD_INFO("Si4709",0x20>>1),
	},
#endif	
};

static struct i2c_board_info touch_i2c_devices[] = {
#if defined(CONFIG_MACH_JUNO_SKT) || defined(CONFIG_MACH_JUNO_KT)
    #if defined (CONFIG_TOUCHSCREEN_QT602240)
	{
		I2C_BOARD_INFO("qt602240-ts", 0x4A),		
	        .irq = MSM_GPIO_TO_INT( TSP_INT ),       
	},
    #elif defined (CONFIG_TOUCHSCREEN_MELFAS)
	{
		I2C_BOARD_INFO(MELFAS_TS_NAME, 0x20),		
	        .irq = MSM_GPIO_TO_INT( TSP_INT ),       
	},
	#endif
#endif	
	#if defined CONFIG_TOUCHSCREEN_SYNAPTICS_I2C_RMI4
	{
		I2C_BOARD_INFO("synaptics-rmi-ts", 0x20),		
	        .irq = MSM_GPIO_TO_INT( TSP_INT ),       
	},
	#endif
	#if defined CONFIG_TOUCHSCREEN_MXT140_I2C_TSP
	{
		I2C_BOARD_INFO("qt602240-ts", 0x4A),		
	        .irq = MSM_GPIO_TO_INT( TSP_INT ),       
	},
	#endif
};

static struct i2c_board_info mus_i2c_devices[] = {
    {
            I2C_BOARD_INFO("fsa9280",0x4A>>1),
    },
#ifdef CONFIG_SENSORS_TAOS
	{
		I2C_BOARD_INFO("taos", 0x39),
	},
#endif
};


static struct i2c_board_info cam_i2c_devices[] = {
#ifdef CONFIG_ISX006
	{
		I2C_BOARD_INFO("isx006", 0x1A),
	},
#endif	
#ifdef CONFIG_S5K4ECGX_COOPER
	{
		I2C_BOARD_INFO("s5k4ecgx", 0x5A>>1),
	},
#endif	
#ifdef CONFIG_S5K5CAGX_GIO
	{
		I2C_BOARD_INFO("s5k5cagx", 0x78>>1),
	},
#endif	
#ifdef CONFIG_S5K5CCAF
	{
		I2C_BOARD_INFO("s5k5ccaf", 0x78>>1),
	},
#endif
#ifdef CONFIG_ISX005
	{
		I2C_BOARD_INFO("isx005", 0x34>>1),
	},
#endif
#ifdef CONFIG_SR030PC30
	{
		I2C_BOARD_INFO("sr030pc30", 0x60>>1),
	},
#endif
#ifdef CONFIG_SR030PC40
	{
		I2C_BOARD_INFO("sr030pc40", 0x60>>1),
	},
#endif
};

static struct i2c_board_info sensor_i2c_devices[] = {
#ifdef CONFIG_SENSORS_MMC31XX
	{
		I2C_BOARD_INFO(MMC31XX_I2C_NAME,  MMC31XX_I2C_ADDR),
	},
#endif
#ifdef CONFIG_SENSORS_KR3D_ACCEL
{
		I2C_BOARD_INFO("kr3dm_accel",KR3DM_I2C_ADDR >> 1),
},
#endif
#ifdef CONFIG_SENSORS_BMA_ACCEL
{
		I2C_BOARD_INFO("bma_accel",0x08), // [HSS]  0X18 => 0X08 (2010.09.29)
},
#endif
#ifdef CONFIG_SENSORS_TAOS
	{
		I2C_BOARD_INFO("taos",0x39),  
	},
#endif
};

#if defined(CONFIG_MACH_JUNO_SKT) || defined(CONFIG_MACH_JUNO_KT)
static struct i2c_board_info proximity_i2c_devices[] = {
#ifdef CONFIG_SENSORS_GP2A
	{
		I2C_BOARD_INFO("gp2a", 0x44),
	},
#endif
};
#endif

static struct i2c_board_info mus_i2c_devices_new[] = {
	{
        	I2C_BOARD_INFO("fsa9280",0x4A>>1),
	},
};

#ifdef CONFIG_MSM_CAMERA
static uint32_t camera_off_gpio_table[] = {
	/* parallel CAMERA interfaces */
//	GPIO_CFG(0,  0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA), /* nCAM_3M_RST */
//	GPIO_CFG(1,  0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), /* CAM_3M_STBY*/
//	GPIO_CFG(2,  0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), /* CAM_CIF_STBY */
//	GPIO_CFG(3,  0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), /* CAM_CIF_nRST */
	GPIO_CFG(4,  0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), /* DAT0 */
	GPIO_CFG(5,  0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), /* DAT1 */
	GPIO_CFG(6,  0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), /* DAT2 */
	GPIO_CFG(7,  0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), /* DAT7 */
	GPIO_CFG(8,  0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), /* DAT8 */
	GPIO_CFG(9,  0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), /* DAT9 */
	GPIO_CFG(10, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), /* DAT10 */
	GPIO_CFG(11, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), /* DAT11 */
	GPIO_CFG(12, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), /* PCLK */
	GPIO_CFG(13, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), /* HSYNC_IN */
	GPIO_CFG(14, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), /* VSYNC_IN */
	GPIO_CFG(15, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /* MCLK */
};

static uint32_t camera_on_gpio_table[] = {
	/* parallel CAMERA interfaces */
//	GPIO_CFG(0,  0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), /* nCAM_3M_RST */
//	GPIO_CFG(1,  0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /* CAM_3M_STBY */
//	GPIO_CFG(2,  1, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), /* CAM_CIF_STBY */
//	GPIO_CFG(3,  0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /* CAM_CIF_nRST */
	GPIO_CFG(4,  1, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), /* DAT0 */
	GPIO_CFG(5,  1, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), /* DAT1 */
	GPIO_CFG(6,  1, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), /* DAT2 */
	GPIO_CFG(7,  1, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), /* DAT7 */
	GPIO_CFG(8,  1, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), /* DAT8 */
	GPIO_CFG(9,  1, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), /* DAT9 */
	GPIO_CFG(10, 1, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), /* DAT10 */
	GPIO_CFG(11, 1, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), /* DAT11 */
	GPIO_CFG(12, 1, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_16MA), /* PCLK */
	GPIO_CFG(13, 1, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), /* HSYNC_IN */
	GPIO_CFG(14, 1, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), /* VSYNC_IN */
	GPIO_CFG(15, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_16MA), /* MCLK */
	};

static void config_gpio_table(uint32_t *table, int len)
{
	int n, rc;
	for (n = 0; n < len; n++) {
		rc = gpio_tlmm_config(table[n], GPIO_CFG_ENABLE);
		if (rc) {
			printk(KERN_ERR "%s: gpio_tlmm_config(%#x)=%d\n",
				__func__, table[n], rc);
			break;
		}
	}
}

static struct vreg *vreg_gp2;
static struct vreg *vreg_gp3;

static void msm_camera_vreg_config(int vreg_en)
{
	int rc;

	if (vreg_gp2 == NULL) {
		vreg_gp2 = vreg_get(NULL, "gp2");
		if (IS_ERR(vreg_gp2)) {
			printk(KERN_ERR "%s: vreg_get(%s) failed (%ld)\n",
				__func__, "gp2", PTR_ERR(vreg_gp2));
			return;
		}

		rc = vreg_set_level(vreg_gp2, 1800);
		if (rc) {
			printk(KERN_ERR "%s: GP2 set_level failed (%d)\n",
				__func__, rc);
		}
	}

	if (vreg_gp3 == NULL) {
		vreg_gp3 = vreg_get(NULL, "gp3");
		if (IS_ERR(vreg_gp3)) {
			printk(KERN_ERR "%s: vreg_get(%s) failed (%ld)\n",
				__func__, "gp3", PTR_ERR(vreg_gp3));
			return;
		}

		rc = vreg_set_level(vreg_gp3, 2850);
		if (rc) {
			printk(KERN_ERR "%s: GP3 set level failed (%d)\n",
				__func__, rc);
		}
	}

	if (vreg_en) {
		rc = vreg_enable(vreg_gp2);
		if (rc) {
			printk(KERN_ERR "%s: GP2 enable failed (%d)\n",
				 __func__, rc);
		}

		rc = vreg_enable(vreg_gp3);
		if (rc) {
			printk(KERN_ERR "%s: GP3 enable failed (%d)\n",
				__func__, rc);
		}
	} else {
		rc = vreg_disable(vreg_gp2);
		if (rc) {
			printk(KERN_ERR "%s: GP2 disable failed (%d)\n",
				 __func__, rc);
		}

		rc = vreg_disable(vreg_gp3);
		if (rc) {
			printk(KERN_ERR "%s: GP3 disable failed (%d)\n",
				__func__, rc);
		}
	}
}

static int config_camera_on_gpios(void)
{
	int vreg_en = 1;

	printk("%s --enter\n", __func__);

	if (machine_is_msm7x25_ffa() ||
	    machine_is_msm7x27_ffa())
		msm_camera_vreg_config(vreg_en);

	config_gpio_table(camera_on_gpio_table,
		ARRAY_SIZE(camera_on_gpio_table));
	return 0;
}

static void config_camera_off_gpios(void)
{
	int vreg_en = 0;

	if (machine_is_msm7x25_ffa() ||
	    machine_is_msm7x27_ffa())
		msm_camera_vreg_config(vreg_en);

	config_gpio_table(camera_off_gpio_table,
		ARRAY_SIZE(camera_off_gpio_table));
}

static struct msm_camera_device_platform_data msm_camera_device_data = {
	.camera_gpio_on  = config_camera_on_gpios,
	.camera_gpio_off = config_camera_off_gpios,
	.ioext.mdcphy = MSM_MDC_PHYS,
	.ioext.mdcsz  = MSM_MDC_SIZE,
	.ioext.appphy = MSM_CLK_CTL_PHYS,
	.ioext.appsz  = MSM_CLK_CTL_SIZE,
};

static struct msm_camera_sensor_flash_src msm_flash_src = {
	.flash_sr_type = MSM_CAMERA_FLASH_SRC_PMIC,
	._fsrc.pmic_src.low_current  = 30,
	._fsrc.pmic_src.high_current = 100,
};


#ifdef CONFIG_S5K5CA //PGH
static struct msm_camera_sensor_flash_data flash_s5k5ca = {
	.flash_type = MSM_CAMERA_FLASH_LED,
	.flash_src  = &msm_flash_src
};
static struct msm_camera_sensor_info msm_camera_sensor_s5k5ca_data = {
        .sensor_name    = "s5k5ca",
        .sensor_reset   = 0, 
//        .sensor_pwd     = 107, 
        .vcm_pwd        = 0, 
        .pdata          = &msm_camera_device_data,
        .flash_data     = &flash_s5k5ca
};
 
static struct platform_device msm_camera_sensor_s5k5ca = {
        .name      = "msm_camera_s5k5ca",
        .dev       = {  
                .platform_data = &msm_camera_sensor_s5k5ca_data,
        },   
};
#endif

#ifdef CONFIG_ISX006
static struct msm_camera_sensor_flash_data flash_isx006 = {
	.flash_type = MSM_CAMERA_FLASH_LED,
	.flash_src  = &msm_flash_src
};
static struct msm_camera_sensor_info msm_camera_sensor_isx006_data = {
        .sensor_name		= "isx006",
        .sensor_reset		= 0, 
        .sensor_pwd		= 107, 
        .vcm_pwd		= 0, 
        .pdata			= &msm_camera_device_data,
        .flash_data		= &flash_isx006
};
 
static struct platform_device msm_camera_sensor_isx006 = {
        .name      = "msm_camera_isx006",
        .dev       = {  
                .platform_data = &msm_camera_sensor_isx006_data,
        },   
};
#endif

#ifdef CONFIG_S5K4ECGX_COOPER
static struct msm_camera_sensor_flash_data flash_s5k4ecgx = {
	.flash_type = MSM_CAMERA_FLASH_LED,
	.flash_src  = &msm_flash_src
};
static struct msm_camera_sensor_info msm_camera_sensor_s5k4ecgx_data = {
        .sensor_name		= "s5k4ecgx",
        .sensor_reset		= 0, 
        .sensor_pwd		= 107, 
        .vcm_pwd		= 0, 
        .pdata			= &msm_camera_device_data,
        .flash_data		= &flash_s5k4ecgx
};
 
static struct platform_device msm_camera_sensor_s5k4ecgx = {
        .name      = "msm_camera_s5k4ecgx",
        .dev       = {  
                .platform_data = &msm_camera_sensor_s5k4ecgx_data,
        },   
};
#endif

#ifdef CONFIG_S5K5CAGX_GIO //PGH
static struct msm_camera_sensor_flash_data flash_s5k5cagx = {
	.flash_type = MSM_CAMERA_FLASH_LED,
	.flash_src  = &msm_flash_src
};
static struct msm_camera_sensor_info msm_camera_sensor_s5k5cagx_data = {
        .sensor_name    = "s5k5cagx",
        .sensor_reset   = 0, 
//        .sensor_pwd     = 107, 
        .vcm_pwd        = 0, 
        .pdata          = &msm_camera_device_data,
        .flash_data     = &flash_s5k5cagx
};
 
static struct platform_device msm_camera_sensor_s5k5cagx = {
        .name      = "msm_camera_s5k5cagx",
        .dev       = {  
                .platform_data = &msm_camera_sensor_s5k5cagx_data,
        },   
};
#endif

#ifdef CONFIG_S5K5CCAF
static struct msm_camera_sensor_flash_data flash_s5k5ccaf = {
	.flash_type = MSM_CAMERA_FLASH_NONE,
	.flash_src  = &msm_flash_src
};
static struct msm_camera_sensor_info msm_camera_sensor_s5k5ccaf_data = {
        .sensor_name    = "s5k5ccaf",
        .sensor_reset   = 0, 
//        .sensor_pwd     = 107, 
        .vcm_pwd        = 0, 
        .pdata          = &msm_camera_device_data,
        .flash_data     = &flash_s5k5ccaf
};
 
static struct platform_device msm_camera_sensor_s5k5ccaf = {
        .name      = "msm_camera_s5k5ccaf",
        .dev       = {  
                .platform_data = &msm_camera_sensor_s5k5ccaf_data,
        },   
};
#endif

#ifdef CONFIG_ISX005
static struct msm_camera_sensor_flash_data flash_isx005 = {
	.flash_type = MSM_CAMERA_FLASH_LED,
	.flash_src  = &msm_flash_src
};
static struct msm_camera_sensor_info msm_camera_sensor_isx005_data = {
        .sensor_name    = "isx005",
        .sensor_reset   = 0, 
//        .sensor_pwd     = 107, 
        .vcm_pwd        = 0, 
        .pdata          = &msm_camera_device_data,
        .flash_data     = &flash_isx005
};
 
static struct platform_device msm_camera_sensor_isx005 = {
        .name      = "msm_camera_isx005",
        .dev       = {  
                .platform_data = &msm_camera_sensor_isx005_data,
        },   
};
#endif

#ifdef CONFIG_SR030PC30
static struct msm_camera_sensor_flash_data flash_sr030pc30 = {
	.flash_type = MSM_CAMERA_FLASH_NONE,
	.flash_src  = &msm_flash_src
};
static struct msm_camera_sensor_info msm_camera_sensor_sr030pc30_data = {
        .sensor_name    = "sr030pc30",
        .sensor_reset   = 0, 
//        .sensor_pwd     = 107, 
        .vcm_pwd        = 0, 
        .pdata          = &msm_camera_device_data,
        .flash_data     = &flash_sr030pc30
};
 
static struct platform_device msm_camera_sensor_sr030pc30 = {
        .name      = "msm_camera_sr030pc30",
        .dev       = {  
                .platform_data = &msm_camera_sensor_sr030pc30_data,
        },   
};
#endif

#ifdef CONFIG_SR030PC40
static struct msm_camera_sensor_flash_data flash_sr030pc40 = {
	.flash_type = MSM_CAMERA_FLASH_NONE,
	.flash_src  = &msm_flash_src
};
static struct msm_camera_sensor_info msm_camera_sensor_sr030pc40_data = {
        .sensor_name    = "sr030pc40",
        .sensor_reset   = 0, 
//        .sensor_pwd     = 107, 
        .vcm_pwd        = 0, 
        .pdata          = &msm_camera_device_data,
        .flash_data     = &flash_sr030pc40
};
 
static struct platform_device msm_camera_sensor_sr030pc40 = {
        .name      = "msm_camera_sr030pc40",
        .dev       = {  
                .platform_data = &msm_camera_sensor_sr030pc40_data,
        },   
};
#endif

#ifdef CONFIG_SR200PC10 //PGH
static struct msm_camera_sensor_flash_data flash_sr200pc10 = {
	.flash_type = MSM_CAMERA_FLASH_LED,
	.flash_src  = &msm_flash_src
};
static struct msm_camera_sensor_info msm_camera_sensor_sr200pc10_data = {
        .sensor_name    = "sr200pc10",
        .sensor_reset   = 0, 
//        .sensor_pwd     = 107, 
        .vcm_pwd        = 0, 
        .pdata          = &msm_camera_device_data,
        .flash_data     = &flash_sr200pc10
};
 
static struct platform_device msm_camera_sensor_sr200pc10 = {
        .name      = "msm_camera_sr200pc10",
        .dev       = {  
                .platform_data = &msm_camera_sensor_sr200pc10_data,
        },   
};
#endif

#ifdef CONFIG_S5K4CA //PGH
static struct msm_camera_sensor_flash_data flash_s5k4ca = {
	.flash_type = MSM_CAMERA_FLASH_LED,
	.flash_src  = &msm_flash_src
};
static struct msm_camera_sensor_info msm_camera_sensor_s5k4ca_data = {
	.sensor_name    = "s5k4ca",
	.sensor_reset   = 0,
	.sensor_pwd     = 107,
	.vcm_pwd        = 0,
	.pdata          = &msm_camera_device_data,
	.flash_data     = &flash_s5k4ca
};

static struct platform_device msm_camera_sensor_s5k4ca = {
	.name      = "msm_camera_s5k4ca",
	.dev       = {
		.platform_data = &msm_camera_sensor_s5k4ca_data,
	},
};
#endif

#ifdef CONFIG_MT9D112
static struct msm_camera_sensor_flash_data flash_mt9d112 = {
	.flash_type = MSM_CAMERA_FLASH_LED,
	.flash_src  = &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_mt9d112_data = {
	.sensor_name    = "mt9d112",
	.sensor_reset   = 89,
	.sensor_pwd     = 85,
	.vcm_pwd        = 0,
	.vcm_enable     = 0,
	.pdata          = &msm_camera_device_data,
	.flash_data     = &flash_mt9d112
};

static struct platform_device msm_camera_sensor_mt9d112 = {
	.name      = "msm_camera_mt9d112",
	.dev       = {
		.platform_data = &msm_camera_sensor_mt9d112_data,
	},
};
#endif

#ifdef CONFIG_S5K3E2FX
static struct msm_camera_sensor_flash_data flash_s5k3e2fx = {
	.flash_type = MSM_CAMERA_FLASH_LED,
	.flash_src  = &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_s5k3e2fx_data = {
	.sensor_name    = "s5k3e2fx",
	.sensor_reset   = 89,
	.sensor_pwd     = 85,
	.vcm_pwd        = 0,
	.vcm_enable     = 0,
	.pdata          = &msm_camera_device_data,
	.flash_data     = &flash_s5k3e2fx
};

static struct platform_device msm_camera_sensor_s5k3e2fx = {
	.name      = "msm_camera_s5k3e2fx",
	.dev       = {
		.platform_data = &msm_camera_sensor_s5k3e2fx_data,
	},
};
#endif

#ifdef CONFIG_MT9P012
static struct msm_camera_sensor_flash_data flash_mt9p012 = {
	.flash_type = MSM_CAMERA_FLASH_LED,
	.flash_src  = &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_mt9p012_data = {
	.sensor_name    = "mt9p012",
	.sensor_reset   = 89,
	.sensor_pwd     = 85,
	.vcm_pwd        = 88,
	.vcm_enable     = 0,
	.pdata          = &msm_camera_device_data,
	.flash_data     = &flash_mt9p012
};

static struct platform_device msm_camera_sensor_mt9p012 = {
	.name      = "msm_camera_mt9p012",
	.dev       = {
		.platform_data = &msm_camera_sensor_mt9p012_data,
	},
};
#endif

#ifdef CONFIG_MT9P012_KM
static struct msm_camera_sensor_flash_data flash_mt9p012_km = {
	.flash_type = MSM_CAMERA_FLASH_LED,
	.flash_src  = &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_mt9p012_km_data = {
	.sensor_name    = "mt9p012_km",
	.sensor_reset   = 89,
	.sensor_pwd     = 85,
	.vcm_pwd        = 88,
	.vcm_enable     = 0,
	.pdata          = &msm_camera_device_data,
	.flash_data     = &flash_mt9p012_km
};

static struct platform_device msm_camera_sensor_mt9p012_km = {
	.name      = "msm_camera_mt9p012_km",
	.dev       = {
		.platform_data = &msm_camera_sensor_mt9p012_km_data,
	},
};
#endif

#ifdef CONFIG_MT9T013
static struct msm_camera_sensor_flash_data flash_mt9t013 = {
	.flash_type = MSM_CAMERA_FLASH_LED,
	.flash_src  = &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_mt9t013_data = {
	.sensor_name    = "mt9t013",
	.sensor_reset   = 89,
	.sensor_pwd     = 85,
	.vcm_pwd        = 0,
	.vcm_enable     = 0,
	.pdata          = &msm_camera_device_data,
	.flash_data     = &flash_mt9t013
};

static struct platform_device msm_camera_sensor_mt9t013 = {
	.name      = "msm_camera_mt9t013",
	.dev       = {
		.platform_data = &msm_camera_sensor_mt9t013_data,
	},
};
#endif

#ifdef CONFIG_VB6801
static struct msm_camera_sensor_flash_data flash_vb6801 = {
	.flash_type = MSM_CAMERA_FLASH_LED,
	.flash_src  = &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_vb6801_data = {
	.sensor_name    = "vb6801",
	.sensor_reset   = 89,
	.sensor_pwd     = 88,
	.vcm_pwd        = 0,
	.vcm_enable     = 0,
	.pdata          = &msm_camera_device_data,
	.flash_data     = &flash_vb6801
};

static struct platform_device msm_camera_sensor_vb6801 = {
	.name      = "msm_camera_vb6801",
	.dev       = {
		.platform_data = &msm_camera_sensor_vb6801_data,
	},
};
#endif
#endif

static u32 msm_calculate_batt_capacity(u32 current_voltage);

static struct msm_psy_batt_pdata msm_psy_batt_data = {
	.voltage_min_design 	= 2800,
	.voltage_max_design	= 4300,
	.avail_chg_sources   	= AC_CHG | USB_CHG ,
	.batt_technology        = POWER_SUPPLY_TECHNOLOGY_LION,
	.calculate_capacity	= &msm_calculate_batt_capacity,
};

static u32 msm_calculate_batt_capacity(u32 current_voltage)
{
	u32 low_voltage   = msm_psy_batt_data.voltage_min_design;
	u32 high_voltage  = msm_psy_batt_data.voltage_max_design;

	return (current_voltage - low_voltage) * 100
		/ (high_voltage - low_voltage);
}

static struct platform_device msm_vibrator_device = {
	.name 		    = "msm_vibrator",
	.id		    = -1,
};

static struct platform_device msm_batt_device = {
	.name 		    = "msm-battery",
	.id		    = -1,
	.dev.platform_data  = &msm_psy_batt_data,
};


static struct i2c_gpio_platform_data touch_i2c_gpio_data = {
	.sda_pin    = TSP_SDA,
	.scl_pin    = TSP_SCL,
#if defined(CONFIG_MACH_COOPER_BASE_KOR)
   .sda_is_open_drain	 = 1,
#endif   
#if defined(CONFIG_MACH_COOPER_SKT)
	.udelay    = 17,
#endif	
};

static struct platform_device touch_i2c_gpio_device = {  
	.name       = "i2c-gpio",
	.id     = 2,
	.dev        = {
		.platform_data  = &touch_i2c_gpio_data,
	},
};

static struct platform_device msm_wlan_pm_device = {
	.name		= "wlan_ar6000_pm",
	.id			= -1,
};

#if defined(CONFIG_MACH_JUNO_SKT) || defined(CONFIG_MACH_JUNO_KT)
static struct i2c_gpio_platform_data fsa9280_i2c_gpio_data = {
	.sda_pin    = 33,
	.scl_pin    = 32,
};
#else
static struct i2c_gpio_platform_data fsa9280_i2c_gpio_data = {
	.sda_pin    = 91,
	.scl_pin    = 90,
};
#endif


static struct platform_device fsa9280_i2c_gpio_device = {  
	.name       = "i2c-gpio",
	.id     = 3,
	.dev        = {
		.platform_data  = &fsa9280_i2c_gpio_data,
	},
};

static struct i2c_gpio_platform_data sensor_i2c_gpio_data = {
	.sda_pin    = 37,
	.scl_pin    = 36,
};

static struct platform_device sensor_i2c_gpio_device = {  
	.name       = "i2c-gpio",
	.id     = 5,
	.dev        = {
		.platform_data  = &sensor_i2c_gpio_data,
	},
};


#ifdef FEATURE_CAMERA_GPIO_I2C
static struct i2c_gpio_platform_data cam_i2c_gpio_data = {
	.sda_pin    = 3,
	.scl_pin    = 2,
	.udelay     = 1,
};

static struct platform_device cam_i2c_gpio_device = {  
	.name       = "i2c-gpio",
	.id     = 6,
	.dev        = {
		.platform_data  = &cam_i2c_gpio_data,
	},
};
#endif

#if defined(CONFIG_MACH_JUNO_SKT) || defined(CONFIG_MACH_JUNO_KT)
static struct i2c_gpio_platform_data proximity_i2c_gpio_data = {
	.sda_pin    = 40,
	.scl_pin    = 39,
};

static struct platform_device proximity_i2c_gpio_device = {  
	.name       = "i2c-gpio",
	.id     = 7,
	.dev        = {
		.platform_data  = &proximity_i2c_gpio_data,
	},
};
#endif

static struct platform_device *devices[] __initdata = {
#if !defined(CONFIG_MSM_SERIAL_DEBUGGER)
#if !defined(CONFIG_MSM_ARM9_USES_UART3)
	&msm_device_uart3,
#endif
#endif
	&msm_device_smd,
	&msm_device_dmov,
	&msm_device_nand,

#ifdef CONFIG_USB_MSM_OTG_72K
	&msm_device_otg,
#ifdef CONFIG_USB_GADGET
	&msm_device_gadget_peripheral,
#endif
#endif

#ifdef CONFIG_USB_FUNCTION
	&msm_device_hsusb_peripheral,
	&mass_storage_device,
#endif

#ifdef CONFIG_USB_ANDROID
	&usb_mass_storage_device,
	&rndis_device,
#ifdef CONFIG_USB_ANDROID_DIAG
	&usb_diag_device,
#endif
	&android_usb_device,
#endif

	&msm_wlan_pm_device,

	&msm_device_i2c,
	&touch_i2c_gpio_device,
	&fsa9280_i2c_gpio_device,
	&sensor_i2c_gpio_device,
#ifdef FEATURE_CAMERA_GPIO_I2C
    &cam_i2c_gpio_device,
#endif	
#if defined(CONFIG_MACH_JUNO_SKT) || defined(CONFIG_MACH_JUNO_KT)
	&proximity_i2c_gpio_device,
#endif
	&smc91x_device,
	&android_pmem_kernel_ebi1_device,
	&android_pmem_device,
	&android_pmem_adsp_device,
	&msm_fb_device,
#ifdef CONFIG_FB_MSM_LCDC_S6D04M0_QVGA
	&lcdc_s6d04m0_panel_device,
#endif
#ifdef CONFIG_FB_MSM_LCDC_S6D04D1_WQVGA
	&lcdc_s6d04d1_panel_device,
#endif
#ifdef CONFIG_FB_MSM_LCDC_TA8566_WQVGA
		&lcdc_ta8566_panel_device,
#endif
/* 20100823 hongkuk.son */
#ifdef CONFIG_FB_MSM_LCDC_S6D16A0X_HVGA
		&lcdc_s6d16a0x_panel_device,
#endif
/***/
#ifdef CONFIG_FB_MSM_LCDC_S6E63M0_WVGA_PANEL
	&lcdc_s6e63m0_panel_device,
#endif

	&msm_device_uart_dm1,
	&msm_device_uart_dm2,
#ifdef CONFIG_BT
	&msm_bt_power_device,
	&msm_bluesleep_device,
#endif
	&msm_device_pmic_leds,
	&msm_device_snd,
	&msm_device_adspdec,
#ifdef CONFIG_ISX006
	&msm_camera_sensor_isx006,
#endif
#ifdef CONFIG_S5K4ECGX_COOPER
	&msm_camera_sensor_s5k4ecgx,
#endif
#ifdef CONFIG_S5K5CA //PCAM
	&msm_camera_sensor_s5k5ca,
#endif

#ifdef CONFIG_SR200PC10 //PGH
	&msm_camera_sensor_sr200pc10,
#endif

#ifdef CONFIG_S5K4CA //PGH
	&msm_camera_sensor_s5k4ca,
#endif

#ifdef CONFIG_S5K5CAGX_GIO
	&msm_camera_sensor_s5k5cagx,
#endif

#ifdef CONFIG_S5K5CCAF
	&msm_camera_sensor_s5k5ccaf,
#endif

#ifdef CONFIG_ISX005
	&msm_camera_sensor_isx005,
#endif

#ifdef CONFIG_SR030PC30
	&msm_camera_sensor_sr030pc30,
#endif

#ifdef CONFIG_SR030PC40
	&msm_camera_sensor_sr030pc40,
#endif

#ifdef CONFIG_MT9T013
	&msm_camera_sensor_mt9t013,
#endif
#ifdef CONFIG_MT9D112
	&msm_camera_sensor_mt9d112,
#endif
#ifdef CONFIG_S5K3E2FX
	&msm_camera_sensor_s5k3e2fx,
#endif
#ifdef CONFIG_MT9P012
	&msm_camera_sensor_mt9p012,
#endif
#ifdef CONFIG_MT9P012_KM
	&msm_camera_sensor_mt9p012_km,
#endif
#ifdef CONFIG_VB6801
	&msm_camera_sensor_vb6801,
#endif
#ifdef CONFIG_ARCH_MSM7X27
	&msm_kgsl_3d0,
#endif
	&hs_device,
	&msm_batt_device,
	&msm_vibrator_device,
#ifdef CONFIG_SAMSUNG_JACK
	&sec_device_jack,
#endif
};

static struct msm_panel_common_pdata mdp_pdata = {
	.gpio = 97,
};

static void __init msm_fb_add_devices(void)
{
	msm_fb_register_device("mdp", &mdp_pdata);
	msm_fb_register_device("pmdh", 0);
	msm_fb_register_device("lcdc", &lcdc_pdata);
}

extern struct sys_timer msm_timer;

static void __init msm7x2x_init_irq(void)
{
	msm_init_irq();
}

static struct msm_acpu_clock_platform_data msm7x2x_clock_data = {
	.acpu_switch_time_us = 50,
#ifdef CONFIG_800MHZ
	.max_speed_delta_khz = 400000,
#else
	.max_speed_delta_khz = 256000,
#endif
	.vdd_switch_time_us = 62,
	.max_axi_khz = 160000,
};

void msm_serial_debug_init(unsigned int base, int irq,
			   struct device *clk_device, int signal_irq);

#if (defined(CONFIG_MMC_MSM_SDC1_SUPPORT)\
	|| defined(CONFIG_MMC_MSM_SDC2_SUPPORT)\
	|| defined(CONFIG_MMC_MSM_SDC3_SUPPORT)\
	|| defined(CONFIG_MMC_MSM_SDC4_SUPPORT))

static void sdcc_gpio_init(void)
{
#ifdef CONFIG_MMC_MSM_CARD_HW_DETECTION
	int rc = 0;
	if (gpio_request(49, "sdc1_status_irq"))
		pr_err("failed to request gpio sdc1_status_irq\n");
	rc = gpio_tlmm_config(GPIO_CFG(49, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP,
				GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc)
		printk(KERN_ERR "%s: Failed to configure GPIO %d\n",
				__func__, rc);
#endif
	/* SDC1 GPIOs */
#ifdef CONFIG_MMC_MSM_SDC1_SUPPORT
	if (gpio_request(51, "sdc1_data_3"))
		pr_err("failed to request gpio sdc1_data_3\n");
	if (gpio_request(52, "sdc1_data_2"))
		pr_err("failed to request gpio sdc1_data_2\n");
	if (gpio_request(53, "sdc1_data_1"))
		pr_err("failed to request gpio sdc1_data_1\n");
	if (gpio_request(54, "sdc1_data_0"))
		pr_err("failed to request gpio sdc1_data_0\n");
	if (gpio_request(55, "sdc1_cmd"))
		pr_err("failed to request gpio sdc1_cmd\n");
	if (gpio_request(56, "sdc1_clk"))
		pr_err("failed to request gpio sdc1_clk\n");
#endif
	if (machine_is_msm7x25_ffa())
		return;

	/* SDC2 GPIOs */
#ifdef CONFIG_MMC_MSM_SDC2_SUPPORT
	if (gpio_request(62, "sdc2_clk"))
		pr_err("failed to request gpio sdc2_clk\n");
	if (gpio_request(63, "sdc2_cmd"))
		pr_err("failed to request gpio sdc2_cmd\n");
	if (gpio_request(64, "sdc2_data_3"))
		pr_err("failed to request gpio sdc2_data_3\n");
	if (gpio_request(65, "sdc2_data_2"))
		pr_err("failed to request gpio sdc2_data_2\n");
	if (gpio_request(66, "sdc2_data_1"))
		pr_err("failed to request gpio sdc2_data_1\n");
	if (gpio_request(67, "sdc2_data_0"))
		pr_err("failed to request gpio sdc2_data_0\n");
#ifdef CONFIG_MACH_COOPER_BASE_KOR
	if (gpio_request(WLAN_BT_EN, "wlan_bt_enable"))
		pr_err("failed to request gpio wlan_bt_enable\n");
	if (gpio_request(WLAN_RST, "wlan_reset"))
		pr_err("failed to request wlan_reset\n");
	if (gpio_request(GPIO_WLAN_HOST_WAKE, "wlan_wake_lock"))
		pr_err("failed to request wlan_wake_lock\n");
#endif
#endif
	if (machine_is_msm7x27_ffa())
		return;

	/* SDC3 GPIOs */
#if 0 // def CONFIG_MMC_MSM_SDC3_SUPPORT
	if (gpio_request(88, "sdc3_clk"))
		pr_err("failed to request gpio sdc3_clk\n");
	if (gpio_request(89, "sdc3_cmd"))
		pr_err("failed to request gpio sdc3_cmd\n");
	if (gpio_request(90, "sdc3_data_3"))
		pr_err("failed to request gpio sdc3_data_3\n");
	if (gpio_request(91, "sdc3_data_2"))
		pr_err("failed to request gpio sdc3_data_2\n");
	if (gpio_request(92, "sdc3_data_1"))
		pr_err("failed to request gpio sdc3_data_1\n");
	if (gpio_request(93, "sdc3_data_0"))
		pr_err("failed to request gpio sdc3_data_0\n");
#ifdef CONFIG_MMC_MSM_SDC3_8_BIT_SUPPORT
    if (gpio_request(19, "sdc3_data_7"))
		pr_err("failed to request gpio sdc3_data_7\n");
	if (gpio_request(20, "sdc3_data_6"))
		pr_err("failed to request gpio sdc3_data_6\n");
	if (gpio_request(21, "sdc3_data_5"))
		pr_err("failed to request gpio sdc3_data_5\n");
	if (gpio_request(108, "sdc3_data_4"))
		pr_err("failed to request gpio sdc3_data_4\n");
#endif
#endif

	/* SDC4 GPIOs */
#ifdef CONFIG_MMC_MSM_SDC4_SUPPORT
	if (gpio_request(19, "sdc4_data_3"))
		pr_err("failed to request gpio sdc4_data_3\n");
	if (gpio_request(20, "sdc4_data_2"))
		pr_err("failed to request gpio sdc4_data_2\n");
	if (gpio_request(21, "sdc4_data_1"))
		pr_err("failed to request gpio sdc4_data_1\n");
	if (gpio_request(107, "sdc4_cmd"))
		pr_err("failed to request gpio sdc4_cmd\n");
	if (gpio_request(108, "sdc4_data_0"))
		pr_err("failed to request gpio sdc4_data_0\n");
	if (gpio_request(109, "sdc4_clk"))
		pr_err("failed to request gpio sdc4_clk\n");
#endif
}


static unsigned long vreg_sts, gpio_sts;
static struct vreg *vreg_mmc;
static struct vreg *vreg_mmc2;
static unsigned mpp_mmc = 2;

struct sdcc_gpio {
	struct msm_gpio *cfg_data;
	uint32_t size;
	struct msm_gpio *sleep_cfg_data;
};

static struct msm_gpio sdc1_cfg_data[] = {
	{GPIO_CFG(51, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc1_dat_3"},
	{GPIO_CFG(52, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc1_dat_2"},
	{GPIO_CFG(53, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc1_dat_1"},
	{GPIO_CFG(54, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc1_dat_0"},
	{GPIO_CFG(55, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc1_cmd"},
	{GPIO_CFG(56, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA), "sdc1_clk"},
};

static struct msm_gpio sdc2_cfg_data[] = {
	{GPIO_CFG(62, 2, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA), "sdc2_clk"},
	{GPIO_CFG(63, 2, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc2_cmd"},
	{GPIO_CFG(64, 2, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc2_dat_3"},
	{GPIO_CFG(65, 2, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc2_dat_2"},
	{GPIO_CFG(66, 2, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc2_dat_1"},
	{GPIO_CFG(67, 2, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc2_dat_0"},
};

static struct msm_gpio sdc2_sleep_cfg_data[] = {
	{GPIO_CFG(62, 2, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA), "sdc2_clk"},
	{GPIO_CFG(63, 2, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc2_cmd"},
	{GPIO_CFG(64, 2, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc2_dat_3"},
	{GPIO_CFG(65, 2, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc2_dat_2"},
	{GPIO_CFG(66, 2, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc2_dat_1"},
	{GPIO_CFG(67, 2, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc2_dat_0"},
};

static struct msm_gpio sdc3_cfg_data[] = {
	{GPIO_CFG(88, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA), "sdc3_clk"},
	{GPIO_CFG(89, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc3_cmd"},
	{GPIO_CFG(90, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc3_dat_3"},
	{GPIO_CFG(91, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc3_dat_2"},
	{GPIO_CFG(92, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc3_dat_1"},
	{GPIO_CFG(93, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc3_dat_0"},
#ifdef CONFIG_MMC_MSM_SDC3_8_BIT_SUPPORT	
	{GPIO_CFG(19, 5, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc3_dat_7"},
    {GPIO_CFG(20, 5, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc3_dat_6"},
    {GPIO_CFG(21, 6, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc3_dat_5"},
    {GPIO_CFG(108, 3, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc3_dat_4"},
#endif    
};

static struct msm_gpio sdc4_cfg_data[] = {
	{GPIO_CFG(19, 3, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc4_dat_3"},
	{GPIO_CFG(20, 3, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc4_dat_2"},
	{GPIO_CFG(21, 4, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc4_dat_1"},
	{GPIO_CFG(107, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc4_cmd"},
	{GPIO_CFG(108, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_8MA), "sdc4_dat_0"},
	{GPIO_CFG(109, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA), "sdc4_clk"},
};

static struct sdcc_gpio sdcc_cfg_data[] = {
	{
		.cfg_data = sdc1_cfg_data,
		.size = ARRAY_SIZE(sdc1_cfg_data),
		.sleep_cfg_data = NULL,
	},
	{
		.cfg_data = sdc2_cfg_data,
		.size = ARRAY_SIZE(sdc2_cfg_data),
		.sleep_cfg_data = sdc2_sleep_cfg_data,
	},
	{
		.cfg_data = sdc3_cfg_data,
		.size = ARRAY_SIZE(sdc3_cfg_data),
		.sleep_cfg_data = NULL,
	},
	{
		.cfg_data = sdc4_cfg_data,
		.size = ARRAY_SIZE(sdc4_cfg_data),
		.sleep_cfg_data = NULL,
	},
};

static void msm_sdcc_setup_gpio(int dev_id, unsigned int enable)
{
	int rc = 0;
	struct sdcc_gpio *curr;

	curr = &sdcc_cfg_data[dev_id - 1];
	if (!(test_bit(dev_id, &gpio_sts)^enable))
		return;

	if (enable) {
		set_bit(dev_id, &gpio_sts);
		rc = msm_gpios_request_enable(curr->cfg_data, curr->size);
		if (rc)
			printk(KERN_ERR "%s: Failed to turn on GPIOs for slot %d\n",
				__func__,  dev_id);
	} else {
		clear_bit(dev_id, &gpio_sts);
		if (curr->sleep_cfg_data) {
			msm_gpios_enable(curr->sleep_cfg_data, curr->size);
			msm_gpios_free(curr->sleep_cfg_data, curr->size);
			return;
		}
		msm_gpios_disable_free(curr->cfg_data, curr->size);
	}
}

static uint32_t msm_sdcc_setup_power(struct device *dv, unsigned int vdd)
{
	int rc = 0;
	struct platform_device *pdev;

	pdev = container_of(dv, struct platform_device, dev);
	msm_sdcc_setup_gpio(pdev->id, !!vdd);

	if (pdev->id != 1)
		return 0;

	if (vdd == 0) {
		if (!vreg_sts)
			return 0;

		clear_bit(pdev->id, &vreg_sts);

		if (!vreg_sts) {
			if (machine_is_msm7x25_ffa() ||
					machine_is_msm7x27_ffa()) {
				rc = mpp_config_digital_out(mpp_mmc,
				     MPP_CFG(MPP_DLOGIC_LVL_MSMP,
				     MPP_DLOGIC_OUT_CTRL_LOW));
			} else
			{
				rc = vreg_disable(vreg_mmc);
				rc = vreg_disable(vreg_mmc2);
			}
			if (rc)
				printk(KERN_ERR "%s: return val: %d \n",
					__func__, rc);
		}
		return 0;
	}

	if (!vreg_sts) {
		if (machine_is_msm7x25_ffa() || machine_is_msm7x27_ffa()) {
			rc = mpp_config_digital_out(mpp_mmc,
			     MPP_CFG(MPP_DLOGIC_LVL_MSMP,
			     MPP_DLOGIC_OUT_CTRL_HIGH));
		} else {
			rc = vreg_set_level(vreg_mmc, OUT2850mV);
			rc = vreg_set_level(vreg_mmc2, OUT2850mV);
			if (!rc)
			{
				rc = vreg_enable(vreg_mmc);
				rc = vreg_enable(vreg_mmc2);
			}
		}
		if (rc)
			printk(KERN_ERR "%s: return val: %d \n",
					__func__, rc);
	}
	set_bit(pdev->id, &vreg_sts);
	return 0;
}

#ifdef CONFIG_MMC_MSM_CARD_HW_DETECTION
static unsigned int sdcc_slot_status(struct device *dev)
{
	int rc;
	rc = gpio_get_value( 49 );

	rc = rc?0:1 ;
	return rc;
}
#endif

#ifndef ATH_POLLING
static void (*wlan_status_notify_cb)(int card_present, void *dev_id);
void *wlan_devid;

static int register_wlan_status_notify(void (*callback)(int card_present, void *dev_id), void *dev_id)
{
	wlan_status_notify_cb = callback;
	wlan_devid = dev_id;
	return 0;
}

static unsigned int wlan_status(struct device *dev)
{
	int rc;
	rc = gpio_get_value(gpio_wlan_reset_n);

	return rc;
}
#endif /* ATH_POLLING */

#ifdef CONFIG_MACH_COOPER_BASE_KOR
//--> 2011.01.21 static wifi skb 
#define DHD_SKB_HDRSIZE 		336
#define DHD_SKB_1PAGE_BUFSIZE	((PAGE_SIZE*1)-DHD_SKB_HDRSIZE)
#define DHD_SKB_2PAGE_BUFSIZE	((PAGE_SIZE*2)-DHD_SKB_HDRSIZE)
#define DHD_SKB_4PAGE_BUFSIZE	((PAGE_SIZE*4)-DHD_SKB_HDRSIZE)

#define WLAN_SKB_BUF_NUM	17

struct sk_buff *wlan_static_skb[WLAN_SKB_BUF_NUM];
EXPORT_SYMBOL(wlan_static_skb);


int __init init_wifi_mem(void)
{
	int i;
	int j;

	printk("init_wifi_mem\n");
	for (i = 0; i < 8; i++) {
		wlan_static_skb[i] = dev_alloc_skb(DHD_SKB_1PAGE_BUFSIZE);
		if (!wlan_static_skb[i])
			goto err_skb_alloc;
	}
	
	for (; i < 16; i++) {
		wlan_static_skb[i] = dev_alloc_skb(DHD_SKB_2PAGE_BUFSIZE);
		if (!wlan_static_skb[i])
			goto err_skb_alloc;
	}
	
	wlan_static_skb[i] = dev_alloc_skb(DHD_SKB_4PAGE_BUFSIZE);
	if (!wlan_static_skb[i])
		goto err_skb_alloc;

	printk("init_wifi_mem success\n");
	return 0;

 err_skb_alloc:
	pr_err("Failed to skb_alloc for WLAN\n");
	for (j = 0 ; j <WLAN_SKB_BUF_NUM ; j++)
		dev_kfree_skb(wlan_static_skb[j]);

	return -ENOMEM;
}
//<-- 2011.01.21 static wifi skb 



#ifdef WLAN_HOST_WAKE
struct wlansleep_info {
	unsigned host_wake;
	unsigned host_wake_irq;
	struct wake_lock wake_lock;
};

static struct wlansleep_info *wsi;
static struct tasklet_struct hostwake_task;

static void wlan_hostwake_task(unsigned long data)
{
	printk(KERN_INFO "WLAN: wake lock timeout 0.5 sec...\n");

	wake_lock_timeout(&wsi->wake_lock, HZ / 2);
}

static irqreturn_t wlan_hostwake_isr(int irq, void *dev_id)
{	
	gpio_clear_detect_status(wsi->host_wake_irq);

	/* schedule a tasklet to handle the change in the host wake line */
	tasklet_schedule(&hostwake_task);
	return IRQ_HANDLED;
}

static int wlan_host_wake_init(void)
{
	int ret;
	
	wsi = kzalloc(sizeof(struct wlansleep_info), GFP_KERNEL);
	if (!wsi)
		return -ENOMEM;

	wake_lock_init(&wsi->wake_lock, WAKE_LOCK_SUSPEND, "bluesleep");
	tasklet_init(&hostwake_task, wlan_hostwake_task, 0);

	wsi->host_wake = 17;
	wsi->host_wake_irq = MSM_GPIO_TO_INT(wsi->host_wake);
	
	gpio_tlmm_config(GPIO_CFG(wsi->host_wake,0,GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	ret = request_irq(wsi->host_wake_irq, wlan_hostwake_isr,
				IRQF_DISABLED | IRQF_TRIGGER_RISING,
				"wlan hostwake", NULL);
	if (ret	< 0) {
		printk(KERN_ERR "WLAN: Couldn't acquire WLAN_HOST_WAKE IRQ");
		return -1;
	}

	ret = enable_irq_wake(wsi->host_wake_irq);
	if (ret < 0) {
		printk(KERN_ERR "WLAN: Couldn't enable WLAN_HOST_WAKE as wakeup interrupt");
		free_irq(wsi->host_wake_irq, NULL);
		return -1;
	}
	
	return 0;
}

static void wlan_host_wake_exit(void)
{
	if (disable_irq_wake(wsi->host_wake_irq))
		printk(KERN_ERR "WLAN: Couldn't disable hostwake IRQ wakeup mode \n");
	free_irq(wsi->host_wake_irq, NULL);

	wake_lock_destroy(&wsi->wake_lock);
	kfree(wsi);	
}
#endif /* WLAN_HOST_WAKE */

void wlan_setup_clock(int on)
{
	struct vreg *vwlan_3_3v;	
	struct vreg *vwlan_1_8v;
#if (CONFIG_BOARD_REVISION == 1)
	struct vreg *vwlan_3_3v_2;

	vwlan_3_3v = vreg_get(NULL, "ldo18");
	if (IS_ERR(vwlan_3_3v)) {
		printk(KERN_ERR "%s: vreg get failed (%ld)\n",
			   __func__, PTR_ERR(vwlan_3_3v));
		return;
	}

	vwlan_3_3v_2 = vreg_get(NULL, "ldo19");
	if (IS_ERR(vwlan_3_3v_2)) {
		printk(KERN_ERR "%s: vreg get failed (%ld)\n",
			   __func__, PTR_ERR(vwlan_3_3v_2));
		return;
	}
#else
	vwlan_3_3v = vreg_get(NULL, "ldo13");
	if (IS_ERR(vwlan_3_3v)) {
	printk(KERN_ERR "%s: vreg get failed (%ld)\n",
		   __func__, PTR_ERR(vwlan_3_3v));
	return;
	}
#endif
	vwlan_1_8v = vreg_get(NULL, "ldo15");
 if (IS_ERR(vwlan_1_8v)) {
  printk(KERN_ERR "%s: vreg get failed (%ld)\n",
			   __func__, PTR_ERR(vwlan_1_8v));
		return;
	}

	printk("%s %s --enter\n", __func__, on ? "on" : "down");

	if (on) {
		vreg_set_level(vwlan_3_3v, OUT3300mV);
		vreg_enable(vwlan_3_3v);		
#if (CONFIG_BOARD_REVISION == 1)	
		vreg_set_level(vwlan_3_3v_2, OUT3300mV);
		vreg_enable(vwlan_3_3v_2);
#endif
		vreg_set_level(vwlan_1_8v, OUT1800mV);
		vreg_enable(vwlan_1_8v);
#if (CONFIG_BOARD_REVISION != 1)	
		gpio_set_value(WLAN_BT_EN, 1);
#endif
//pascal
#if 0
		if (gpio_get_value(BT_RESET/*BT_nRST*/)==0) {
			/*If bt is off state, wlan need to turn on BT power*/
			bluetooth_setup_power(1);
		}
#endif
#if defined(CONFIG_MACH_PASCAL)
		/* additional delay for power on */
		mdelay(20);
#endif
	}
#if defined(CONFIG_MACH_CALLISTO) || defined(CONFIG_MACH_PASCAL)
	else
	{
		/* power off for sleep current */
		vreg_disable(vwlan_3_3v);
		vreg_disable(vwlan_1_8v);
//pascal
#if 0
		if (gpio_get_value(BT_RESET/*BT_nRST*/) ==0) {
			bluetooth_setup_power(0);
		}
#endif		
	}
#endif
}
void wlan_setup_power(int on, int detect)
{
	printk("%s %s --enter\n", __func__, on ? "on" : "down");

	if (detect != 1) {
			printk(/*KERN_DEBUG*/ "(on=%d, flag=%d)\n", on, detect);
	//For Starting/Stopping Tethering service
			if (on)
				gpio_direction_output(WLAN_RST, 1);
			else
				gpio_direction_output(WLAN_RST, 0);

			return;
	}

	if (on) {
		printk(KERN_DEBUG "WLAN GPIO state : WLAN_BT_EN = %d, WLAN_RST = %d\n", 
					   gpio_get_value(WLAN_BT_EN), gpio_get_value(WLAN_RST));
#if 1 //pascal
		if (gpio_get_value(WLAN_BT_EN) == 0) {			
			gpio_direction_output(WLAN_BT_EN, 1);	/* BT_WLAN_REG_ON */					
			mdelay(30);
		}			
		gpio_direction_output(WLAN_RST, 1);	/* WLAN_RESET */
#else
		if (gpio_get_value(WLAN_BT_EN) == 0) {			
			wlan_setup_clock(1);	
			mdelay(30);
		}		
		gpio_set_value (WLAN_RST, 1);	/* WLAN_RESET */
#endif
#ifdef WLAN_HOST_WAKE   
		wlan_host_wake_init();
#else     
              gpio_tlmm_config(GPIO_CFG(GPIO_WLAN_HOST_WAKE,0,GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), GPIO_CFG_ENABLE);           
#endif /* WLAN_HOST_WAKE */
	}
	else {
		printk(KERN_DEBUG "WLAN GPIO state : WLAN_BT_EN = %d, WLAN_RST = %d\n", 
					   gpio_get_value(WLAN_BT_EN), gpio_get_value(WLAN_RST));
#ifdef WLAN_HOST_WAKE
		wlan_host_wake_exit();
#endif /* WLAN_HOST_WAKE */
#if 1 //pascal
		gpio_direction_output(WLAN_RST, 0);	/* WLAN_RESET */				
		if(gpio_get_value(BT_RESET) == 0) 	// PASCAL_WIFI_SETUP				
		{					 
			msleep(30);					 
			gpio_direction_output(WLAN_BT_EN, 0);	/* BT_WLAN_REG_ON */				
		}

#else
		gpio_set_value (WLAN_RST, 0);	/* WLAN_RESET */

#if defined(CONFIG_MACH_PASCAL) /* Atheros */
		if (gpio_get_value(WLAN_BT_EN) == 0) {
			mdelay(30);
			wlan_setup_clock(0);		
		}
#endif
#endif /* Atheros */
	}	
#ifndef ATH_POLLING
	mdelay(100);
	if (detect) {
		/* Detect card */
		if (wlan_status_notify_cb)
			wlan_status_notify_cb(on, wlan_devid);
		else
			printk(KERN_ERR "WLAN: No notify available\n");
	}
#endif /* ATH_POLLING */
}
EXPORT_SYMBOL(wlan_setup_power);
#else
#ifdef WLAN_HOST_WAKE
struct wlansleep_info {
	unsigned host_wake;
	unsigned host_wake_irq;
	struct wake_lock wake_lock;
};

static struct wlansleep_info *wsi;
static struct tasklet_struct hostwake_task;

static void wlan_hostwake_task(unsigned long data)
{
	printk(KERN_INFO "WLAN: wake lock timeout 0.5 sec...\n");

	wake_lock_timeout(&wsi->wake_lock, HZ / 2);
}

static irqreturn_t wlan_hostwake_isr(int irq, void *dev_id)
{	
	//gpio_clear_detect_status(wsi->host_wake_irq);

	/* schedule a tasklet to handle the change in the host wake line */
	tasklet_schedule(&hostwake_task);
	return IRQ_HANDLED;
}

static int wlan_host_wake_init(void)
{
	int ret;
	
	wsi = kzalloc(sizeof(struct wlansleep_info), GFP_KERNEL);
	if (!wsi)
		return -ENOMEM;

	wake_lock_init(&wsi->wake_lock, WAKE_LOCK_SUSPEND, "bluesleep");
	tasklet_init(&hostwake_task, wlan_hostwake_task, 0);

	wsi->host_wake = 17;
	wsi->host_wake_irq = MSM_GPIO_TO_INT(wsi->host_wake);
/*
	host_wake_cfg = GPIO_CFG(17, 0, GPIO_CFG_INPUT,
		GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA);
	ret = gpio_tlmm_config(host_wake_cfg, GPIO_CFG_ENABLE);
	if (ret) {
		printk(KERN_ERR "%s: gpio_tlmm_config(%#x)=%d\n",
				__func__, host_wake_cfg, ret);
	}
*/	
	
	ret = request_irq(wsi->host_wake_irq, wlan_hostwake_isr,
				IRQF_DISABLED | IRQF_TRIGGER_RISING,
				"wlan hostwake", NULL);
	if (ret	< 0) {
		printk(KERN_ERR "WLAN: Couldn't acquire WLAN_HOST_WAKE IRQ");
		return -1;
	}

	ret = enable_irq_wake(wsi->host_wake_irq);
	if (ret < 0) {
		printk(KERN_ERR "WLAN: Couldn't enable WLAN_HOST_WAKE as wakeup interrupt");
		free_irq(wsi->host_wake_irq, NULL);
		return -1;
	}
	
	return 0;
}

static void wlan_host_wake_exit(void)
{
	if (disable_irq_wake(wsi->host_wake_irq))
		printk(KERN_ERR "WLAN: Couldn't disable hostwake IRQ wakeup mode \n");
	free_irq(wsi->host_wake_irq, NULL);

	wake_lock_destroy(&wsi->wake_lock);
	kfree(wsi);	
}
#endif /* WLAN_HOST_WAKE */

void wlan_setup_clock(int on)
{
struct vreg *vwlan_3_3v; 
 struct vreg *vwlan_1_8v;

 vwlan_3_3v = vreg_get(NULL, "ldo13");
 if (IS_ERR(vwlan_3_3v)) {
 printk(KERN_ERR "%s: vreg get failed (%ld)\n",
     __func__, PTR_ERR(vwlan_3_3v));
 return;
 }

 vwlan_1_8v = vreg_get(NULL, "ldo5");
 if (IS_ERR(vwlan_1_8v)) {
  printk(KERN_ERR "%s: vreg get failed (%ld)\n",
      __func__, PTR_ERR(vwlan_1_8v));
  return;
 }

 printk("%s %s --enter\n", __func__, on ? "on" : "down");

 if (on) {
  vreg_set_level(vwlan_3_3v, OUT3300mV);
  vreg_enable(vwlan_3_3v);  
  vreg_set_level(vwlan_1_8v, OUT1800mV);
  vreg_enable(vwlan_1_8v);
  gpio_set_value(VDD_WLAN_EN, 1);  /* VDD_WLAN_EN */
  /* additional delay for power on */
  mdelay(20);
 }
 else
 {
   gpio_set_value(VDD_WLAN_EN, 0);  /* VDD_WLAN_EN Disable */ 
  /* power off for sleep current */
  vreg_disable(vwlan_3_3v);
  vreg_disable(vwlan_1_8v);
 }
}

void wlan_setup_power(int on, int detect)
{
	printk("%s %s --enter\n", __func__, on ? "on" : "down");

	if (on) {
		if (gpio_get_value(BT_RESET) == 0) {			
			wlan_setup_clock(1);	
			mdelay(30);
		}		
//		gpio_tlmm_config(GPIO_CFG(gpio_wlan_reset_n,0,GPIO_CFG_OUTPUT,GPIO_CFG_NO_PULL, GPIO_CFG_16MA),GPIO_CFG_ENABLE); // nextfree.kim 2010-08-24 Wifi oA(AOAE c oA cC OAo)
		gpio_set_value(gpio_wlan_reset_n, 1);	/* WLAN_RESET */
#ifdef WLAN_HOST_WAKE
		wlan_host_wake_init();
#endif /* WLAN_HOST_WAKE */
	}
	else {
#ifdef WLAN_HOST_WAKE
		wlan_host_wake_exit();
#endif /* WLAN_HOST_WAKE */
		gpio_set_value(gpio_wlan_reset_n, 0);	/* WLAN_RESET */

#if defined(CONFIG_MACH_COOPER) /* Atheros */
		if (gpio_get_value(BT_RESET) == 0) {
			mdelay(30);
			wlan_setup_clock(0);		
		}
#endif /* Atheros */
	}	
#ifndef ATH_POLLING
	mdelay(100);
	if (detect) {
		/* Detect card */
		if (wlan_status_notify_cb)
			wlan_status_notify_cb(on, wlan_devid);
		else
			printk(KERN_ERR "WLAN: No notify available\n");
	}
#endif /* ATH_POLLING */
}
EXPORT_SYMBOL(wlan_setup_power);

#endif

#ifdef CONFIG_MMC_MSM_SDC1_SUPPORT
static struct mmc_platform_data msm7x2x_sdc1_data = {
	.ocr_mask	= MMC_VDD_28_29,
	.translate_vdd	= msm_sdcc_setup_power,
	.mmc_bus_width  = MMC_CAP_4_BIT_DATA,
#ifdef CONFIG_MMC_MSM_CARD_HW_DETECTION
	.status         = sdcc_slot_status,
	.status_irq	= MSM_GPIO_TO_INT(49),
	.irq_flags      = IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
#endif
#if 1 /* ATHEROS */
	.dummy52_required = 1,
#endif /* ATHEROS */
	.msmsdcc_fmin   = 144000,
	.msmsdcc_fmid   = 24576000,
	.msmsdcc_fmax   = 49152000,
	.nonremovable   = 0,
};
#endif

#ifdef CONFIG_MMC_MSM_SDC2_SUPPORT
static struct mmc_platform_data msm7x2x_sdc2_data = {
	.ocr_mask	= MMC_VDD_28_29,
	.translate_vdd	= msm_sdcc_setup_power,
	.mmc_bus_width  = MMC_CAP_4_BIT_DATA,
#ifndef ATH_POLLING
	.status = wlan_status,
	.register_status_notify	= register_wlan_status_notify,
#endif /* ATH_POLLING */
	.dummy52_required = 1,
	.msmsdcc_fmin   = 144000,
	.msmsdcc_fmid   = 24576000,
	.msmsdcc_fmax   = 49152000,
	.nonremovable   = 0,
};
#endif

#ifdef CONFIG_MMC_MSM_SDC3_SUPPORT
static struct mmc_platform_data msm7x2x_sdc3_data = {
	.ocr_mask	= MMC_VDD_28_29,
	.translate_vdd	= msm_sdcc_setup_power,
#ifdef CONFIG_MMC_MSM_SDC3_8_BIT_SUPPORT	
	.mmc_bus_width  = MMC_CAP_8_BIT_DATA,	// fetchers_chan_2G
#else
	.mmc_bus_width  = MMC_CAP_4_BIT_DATA,
#endif
	.msmsdcc_fmin	= 144000,
	.msmsdcc_fmid	= 24576000,
	.msmsdcc_fmax	= 49152000,
	.nonremovable	= 0,
};
#endif

#ifdef CONFIG_MMC_MSM_SDC4_SUPPORT
static struct mmc_platform_data msm7x2x_sdc4_data = {
	.ocr_mask	= MMC_VDD_28_29,
	.translate_vdd	= msm_sdcc_setup_power,
	.mmc_bus_width  = MMC_CAP_4_BIT_DATA,
	.msmsdcc_fmin	= 144000,
	.msmsdcc_fmid	= 24576000,
	.msmsdcc_fmax	= 49152000,
	.nonremovable	= 0,
};
#endif


static void __init msm7x2x_init_mmc(void)
{
	if (!machine_is_msm7x25_ffa() && !machine_is_msm7x27_ffa()) {
		vreg_mmc = vreg_get(NULL, "ldo16");
		vreg_mmc2 = vreg_get(NULL, "ldo15");
		if (IS_ERR(vreg_mmc)) {
			printk(KERN_ERR "%s: vreg get failed (%ld)\n",
			       __func__, PTR_ERR(vreg_mmc));
			return;
		}
	}

	#ifdef CONFIG_MACH_COOPER_BASE_KOR
	if (gpio_request(WLAN_BT_EN, "wlan_bt_enable"))
		pr_err("failed to request gpio wlan_bt_enable\n");
	if (gpio_request(WLAN_RST, "wlan_reset"))
		pr_err("failed to request wlan_reset\n");
	if (gpio_request(GPIO_WLAN_HOST_WAKE, "wlan_wake_lock"))
		pr_err("failed to request wlan_wake_lock\n");
    #else
	if (gpio_tlmm_config(GPIO_CFG(VDD_WLAN_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE))
		printk (KERN_ERR "Error configuraing VWALN_EN\n");	
	
	if (gpio_tlmm_config(GPIO_CFG(gpio_wlan_reset_n, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE))
		printk (KERN_ERR "Error configuraing WLAN_RESET_N\n");
	
	if (gpio_tlmm_config(GPIO_CFG(17, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE))
		printk (KERN_ERR "Error configuraing GPIO_WLAN_HOST_WAKE\n");
	#endif
	

#ifdef CONFIG_MMC_MSM_CARD_HW_DETECTION
	int rc = 0;
	if (gpio_request(49, "sdc1_status_irq"))
		pr_err("failed to request gpio sdc1_status_irq\n");
	rc = gpio_tlmm_config(GPIO_CFG(49, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc)
		printk(KERN_ERR "%s: Failed to configure GPIO %d\n",
				__func__, rc);
#endif


//	sdcc_gpio_init();
#ifdef CONFIG_MMC_MSM_SDC3_SUPPORT
	if (machine_is_msm7x25_surf() || machine_is_msm7x27_surf()) {
		msm_add_sdcc(3, &msm7x2x_sdc3_data);
	}
#endif
#ifdef CONFIG_MMC_MSM_SDC1_SUPPORT
	msm_add_sdcc(1, &msm7x2x_sdc1_data);
#endif

	if (machine_is_msm7x25_surf() || machine_is_msm7x27_surf() ||
		machine_is_msm7x27_ffa()) {
#ifdef CONFIG_MMC_MSM_SDC2_SUPPORT
		msm_add_sdcc(2, &msm7x2x_sdc2_data);
#endif
	}

	if (machine_is_msm7x25_surf() || machine_is_msm7x27_surf()) {
//mmcblk number ϱ   ø -- #ifdef CONFIG_MMC_MSM_SDC3_SUPPORT
//		msm_add_sdcc(3, &msm7x2x_sdc3_data);
//#endif
#ifdef CONFIG_MMC_MSM_SDC4_SUPPORT
		msm_add_sdcc(4, &msm7x2x_sdc4_data);
#endif
	}
}

#else
#define msm7x2x_init_mmc() do {} while (0)
#endif


static struct msm_pm_platform_data msm7x25_pm_data[MSM_PM_SLEEP_MODE_NR] = {
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE].latency = 16000,

	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN].latency = 12000,

	[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].latency = 2000,
};

static struct msm_pm_platform_data msm7x27_pm_data[MSM_PM_SLEEP_MODE_NR] = {
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE].supported = 1,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE].suspend_enabled = 1,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE].idle_enabled = 1,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE].latency = 16000,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE].residency = 20000,

	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN].supported = 1,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN].suspend_enabled = 1,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN].idle_enabled = 1,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN].latency = 12000,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN].residency = 20000,

	[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].supported = 1,
	[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].suspend_enabled
		= 1,
	[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].idle_enabled = 1,
	[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].latency = 2000,
	[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].residency = 0,
};

static void
msm_i2c_gpio_config(int iface, int config_type)
{
	int gpio_scl;
	int gpio_sda;
#ifdef FEATURE_CAMERA_GPIO_I2C    
    return;
#endif    
	if (iface) {
#if 0
		gpio_scl = 95;
		gpio_sda = 96;
#endif
		return;
	} else {
		gpio_scl = 60;
		gpio_sda = 61;
	}
	if (config_type) {
		gpio_tlmm_config(GPIO_CFG(gpio_scl, 1, GPIO_CFG_INPUT,
					GPIO_CFG_NO_PULL, GPIO_CFG_16MA), GPIO_CFG_ENABLE);
		gpio_tlmm_config(GPIO_CFG(gpio_sda, 1, GPIO_CFG_INPUT,
					GPIO_CFG_NO_PULL, GPIO_CFG_16MA), GPIO_CFG_ENABLE);
	} else {
		gpio_tlmm_config(GPIO_CFG(gpio_scl, 0, GPIO_CFG_OUTPUT,
					GPIO_CFG_NO_PULL, GPIO_CFG_16MA), GPIO_CFG_ENABLE);
		gpio_tlmm_config(GPIO_CFG(gpio_sda, 0, GPIO_CFG_OUTPUT,
					GPIO_CFG_NO_PULL, GPIO_CFG_16MA), GPIO_CFG_ENABLE);
	}
}

static struct msm_i2c_platform_data msm_i2c_pdata = {
#if 1//PCAM : Fast I2C
	.clk_freq = 380000,
#else//ORG : Normal I2C
	.clk_freq = 100000,
#endif//PCAM
	.rmutex  = 0,
	.pri_clk = 60,
	.pri_dat = 61,
	.aux_clk = 95,
	.aux_dat = 96,
	.msm_i2c_config_gpio = msm_i2c_gpio_config,
};

static void __init msm_device_i2c_init(void)
{
#ifndef FEATURE_CAMERA_GPIO_I2C    
	if (gpio_request(60, "i2c_pri_clk"))
		pr_err("failed to request gpio i2c_pri_clk\n");
	if (gpio_request(61, "i2c_pri_dat"))
		pr_err("failed to request gpio i2c_pri_dat\n");
#endif		
/* for SIM recognization */
#if 0
	if (gpio_request(95, "i2c_sec_clk"))
		pr_err("failed to request gpio i2c_sec_clk\n");
	if (gpio_request(96, "i2c_sec_dat"))
		pr_err("failed to request gpio i2c_sec_dat\n");
#endif

	if (cpu_is_msm7x27())
		msm_i2c_pdata.pm_lat =
		msm7x27_pm_data[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN]
		.latency;
	else
		msm_i2c_pdata.pm_lat =
		msm7x25_pm_data[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN]
		.latency;

	msm_device_i2c.dev.platform_data = &msm_i2c_pdata;
}

static void usb_mpp_init(void)
{
	unsigned rc;
	unsigned mpp_usb = 7;

	if (machine_is_msm7x25_ffa() || machine_is_msm7x27_ffa()) {
		rc = mpp_config_digital_out(mpp_usb,
				MPP_CFG(MPP_DLOGIC_LVL_VDD,
					MPP_DLOGIC_OUT_CTRL_HIGH));
		if (rc)
			pr_err("%s: configuring mpp pin"
					"to enable 3.3V LDO failed\n", __func__);
	}
}

#if defined(CONFIG_MSM_ARM9_USES_UART3)
extern int arm9_uses_uart3; 
#endif

static void __init msm7x2x_init(void)
{

#ifdef CONFIG_ARCH_MSM7X25
	msm_clock_init(msm_clocks_7x25, msm_num_clocks_7x25);
#elif defined(CONFIG_ARCH_MSM7X27)
	msm_clock_init(msm_clocks_7x27, msm_num_clocks_7x27);
#endif

#if defined(CONFIG_MSM_SERIAL_DEBUGGER)
	msm_serial_debug_init(MSM_UART3_PHYS, INT_UART3,
			&msm_device_uart3.dev, 1);
#endif

#if defined(CONFIG_SMC91X)
	if (machine_is_msm7x25_ffa() || machine_is_msm7x27_ffa()) {
		smc91x_resources[0].start = 0x98000300;
		smc91x_resources[0].end = 0x980003ff;
		smc91x_resources[1].start = MSM_GPIO_TO_INT(85);
		smc91x_resources[1].end = MSM_GPIO_TO_INT(85);
		if (gpio_tlmm_config(GPIO_CFG(85, 0,
					      GPIO_CFG_INPUT,
					      GPIO_CFG_PULL_DOWN,
					      GPIO_CFG_2MA),
				     GPIO_CFG_ENABLE)) {
			printk(KERN_ERR
			       "%s: Err: Config GPIO-85 INT\n",
				__func__);
		}
	}
#endif

	if (cpu_is_msm7x27())
		msm7x2x_clock_data.max_axi_khz = 200000;

	msm_acpu_clock_init(&msm7x2x_clock_data);

	usb_mpp_init();

#ifdef CONFIG_USB_FUNCTION
	msm_hsusb_pdata.swfi_latency =
		msm7x27_pm_data
		[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].latency;

	msm_device_hsusb_peripheral.dev.platform_data = &msm_hsusb_pdata;
#endif

#ifdef CONFIG_USB_MSM_OTG_72K
	msm_device_otg.dev.platform_data = &msm_otg_pdata;
	if (machine_is_msm7x25_surf() || machine_is_msm7x25_ffa()) {
		msm_otg_pdata.pemp_level =
			PRE_EMPHASIS_WITH_20_PERCENT;
		msm_otg_pdata.drv_ampl = HS_DRV_AMPLITUDE_5_PERCENT;
		msm_otg_pdata.cdr_autoreset = CDR_AUTO_RESET_ENABLE;
		msm_otg_pdata.phy_reset = msm_otg_rpc_phy_reset;
	}
	if (machine_is_msm7x27_surf() || machine_is_msm7x27_ffa()) {
		msm_otg_pdata.pemp_level =
			PRE_EMPHASIS_WITH_10_PERCENT;
		msm_otg_pdata.drv_ampl = HS_DRV_AMPLITUDE_5_PERCENT;
		msm_otg_pdata.cdr_autoreset = CDR_AUTO_RESET_DISABLE;
		msm_otg_pdata.phy_reset_sig_inverted = 1;
	}

#ifdef CONFIG_USB_GADGET
	msm_otg_pdata.swfi_latency =
		msm7x27_pm_data
		[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].latency;
	msm_device_gadget_peripheral.dev.platform_data = &msm_gadget_pdata;
	msm_gadget_pdata.is_phy_status_timer_on = 1;
	msm_gadget_pdata.self_powered = 1;
#endif
#endif

#ifdef CONFIG_SAMSUNG_JACK
	sec_jack_gpio_init();
#endif
	platform_add_devices(devices, ARRAY_SIZE(devices));

#if defined(CONFIG_MSM_ARM9_USES_UART3)
	if ( !arm9_uses_uart3 )
		platform_device_register(&msm_device_uart3);
#endif
	//
	printk(KERN_INFO"%s : msm7x2x_init #####\n",__func__);

#ifdef CONFIG_MSM_CAMERA
	config_camera_off_gpios(); /* might not be necessary */
#endif
#ifndef FEATURE_CAMERA_GPIO_I2C
	msm_device_i2c_init();
	
	i2c_register_board_info(0, cam_i2c_devices, ARRAY_SIZE(cam_i2c_devices));
#else
	i2c_register_board_info(6, cam_i2c_devices, ARRAY_SIZE(cam_i2c_devices));
#endif
	i2c_register_board_info(2, touch_i2c_devices, ARRAY_SIZE(touch_i2c_devices));
	i2c_register_board_info(3, mus_i2c_devices_new, ARRAY_SIZE(mus_i2c_devices_new));
	i2c_register_board_info(5, sensor_i2c_devices, ARRAY_SIZE(sensor_i2c_devices));
#if defined(CONFIG_MACH_JUNO_SKT) || defined(CONFIG_MACH_JUNO_KT)
	i2c_register_board_info(7, proximity_i2c_devices, ARRAY_SIZE(proximity_i2c_devices));
#endif
    
#ifdef CONFIG_SURF_FFA_GPIO_KEYPAD
	if (machine_is_msm7x25_ffa() || machine_is_msm7x27_ffa())
		platform_device_register(&keypad_device_7k_ffa);
	else
	{
		if( board_hw_revision >= 4 )
			platform_device_register(&keypad_device_surf_03);
		else
			platform_device_register(&keypad_device_surf_00);
	}
#endif
	//sec: sm.kim
	if( board_hw_revision >= 4 )
		keypad_gpio_control(&keypad_device_surf_03);
	else
		keypad_gpio_control(&keypad_device_surf_00);

	lcdc_gordon_gpio_init();
	msm_fb_add_devices();
#ifdef CONFIG_USB_EHCI_MSM
	msm7x2x_init_host();
#endif
	msm7x2x_init_mmc();
	bt_power_init();

	if (cpu_is_msm7x27())
		msm_pm_set_platform_data(msm7x27_pm_data,
					ARRAY_SIZE(msm7x27_pm_data));
	else
		msm_pm_set_platform_data(msm7x25_pm_data,
					ARRAY_SIZE(msm7x25_pm_data));

#ifdef CONFIG_MACH_COOPER_BASE_KOR
	//2011.01.21 static wifi skb 
	init_wifi_mem();
#endif
}

static unsigned pmem_kernel_ebi1_size = PMEM_KERNEL_EBI1_SIZE;
static int __init pmem_kernel_ebi1_size_setup(char *p)
{
	pmem_kernel_ebi1_size = memparse(p, NULL);
	return 0;
}
early_param("pmem_kernel_ebi1_size", pmem_kernel_ebi1_size_setup);

static unsigned pmem_mdp_size = MSM_PMEM_MDP_SIZE;
static int __init pmem_mdp_size_setup(char *p)
{
	pmem_mdp_size = memparse(p, NULL);
	return 0;
}
early_param("pmem_mdp_size", pmem_mdp_size_setup);

static unsigned pmem_adsp_size = MSM_PMEM_ADSP_SIZE;
static int __init pmem_adsp_size_setup(char *p)
{
	pmem_adsp_size = memparse(p, NULL);
	return 0;
}
early_param("pmem_adsp_size", pmem_adsp_size_setup);

static unsigned fb_size = MSM_FB_SIZE;
static int __init fb_size_setup(char *p)
{
	fb_size = memparse(p, NULL);
	return 0;
}
early_param("fb_size", fb_size_setup);

static void __init msm_msm7x2x_allocate_memory_regions(void)
{
	void *addr;
	unsigned long size;

	size = pmem_mdp_size;
	if (size) {
		addr = alloc_bootmem(size);
		android_pmem_pdata.start = __pa(addr);
		android_pmem_pdata.size = size;
		pr_info("allocating %lu bytes at %p (%lx physical) for mdp "
			"pmem arena\n", size, addr, __pa(addr));
	}

	size = pmem_adsp_size;
	if (size) {
		addr = alloc_bootmem(size);
		android_pmem_adsp_pdata.start = __pa(addr);
		android_pmem_adsp_pdata.size = size;
		pr_info("allocating %lu bytes at %p (%lx physical) for adsp "
			"pmem arena\n", size, addr, __pa(addr));
	}

	size = fb_size ? : MSM_FB_SIZE;
	addr = alloc_bootmem(size);
	msm_fb_resources[0].start = __pa(addr);
	msm_fb_resources[0].end = msm_fb_resources[0].start + size - 1;
	pr_info("allocating %lu bytes at %p (%lx physical) for fb\n",
		size, addr, __pa(addr));

	size = pmem_kernel_ebi1_size;
	if (size) {
		addr = alloc_bootmem_aligned(size, 0x100000);
		android_pmem_kernel_ebi1_pdata.start = __pa(addr);
		android_pmem_kernel_ebi1_pdata.size = size;
		pr_info("allocating %lu bytes at %p (%lx physical) for kernel"
			" ebi1 pmem arena\n", size, addr, __pa(addr));
	}
}

static void __init msm7x2x_map_io(void)
{
	msm_map_common_io();
	msm_msm7x2x_allocate_memory_regions();

	if (socinfo_init() < 0)
		BUG();

#ifdef CONFIG_CACHE_L2X0
	if (machine_is_msm7x27_surf() || machine_is_msm7x27_ffa()) {
		/* 7x27 has 256KB L2 cache:
			64Kb/Way and 4-Way Associativity;
			evmon/parity/share disabled. */
		if ((SOCINFO_VERSION_MAJOR(socinfo_get_version()) > 1)
			|| ((SOCINFO_VERSION_MAJOR(socinfo_get_version()) == 1)
			&& (SOCINFO_VERSION_MINOR(socinfo_get_version()) >= 3)))
			/* R/W latency: 4 cycles; */
			l2x0_init(MSM_L2CC_BASE, 0x0006801B, 0xfe000000);
		else
			/* R/W latency: 3 cycles; */
		l2x0_init(MSM_L2CC_BASE, 0x00068012, 0xfe000000);
	}
#endif
#ifdef CONFIG_MACH_COOPER_BASE_KOR
       s5p_reserve_bootmem(crespo_media_devs, ARRAY_SIZE(crespo_media_devs));
#endif
}

#ifdef	CONFIG_MACH_EUROPA
MACHINE_START(MSM7X27_SURF, "GT-I5500 Board")
#ifdef CONFIG_MSM_DEBUG_UART
	.phys_io        = MSM_DEBUG_UART_PHYS,
	.io_pg_offst    = ((MSM_DEBUG_UART_BASE) >> 18) & 0xfffc,
#endif
	.boot_params	= PHYS_OFFSET + 0x100,
	.map_io		= msm7x2x_map_io,
	.init_irq	= msm7x2x_init_irq,
	.init_machine	= msm7x2x_init,
	.timer		= &msm_timer,
MACHINE_END
#endif


#if defined(CONFIG_MACH_COOPER_SKT)
    MACHINE_START(MSM7X27_SURF, "SHW-M240S Board")
#ifdef CONFIG_MSM_DEBUG_UART
        .phys_io        = MSM_DEBUG_UART_PHYS,
        .io_pg_offst    = ((MSM_DEBUG_UART_BASE) >> 18) & 0xfffc,
#endif
        .boot_params    = PHYS_OFFSET + 0x100,
        .map_io     = msm7x2x_map_io,
        .init_irq   = msm7x2x_init_irq,
        .init_machine   = msm7x2x_init,
        .timer      = &msm_timer,
    MACHINE_END
#elif defined(CONFIG_MACH_COOPER_KT)
        MACHINE_START(MSM7X27_SURF, "SHW-M240K Board")
#ifdef CONFIG_MSM_DEBUG_UART
            .phys_io        = MSM_DEBUG_UART_PHYS,
            .io_pg_offst    = ((MSM_DEBUG_UART_BASE) >> 18) & 0xfffc,
#endif
            .boot_params    = PHYS_OFFSET + 0x100,
            .map_io     = msm7x2x_map_io,
            .init_irq   = msm7x2x_init_irq,
            .init_machine   = msm7x2x_init,
            .timer      = &msm_timer,
        MACHINE_END
#elif defined(CONFIG_MACH_GIO_SKT)
        MACHINE_START(MSM7X27_SURF, "SHW-M290S Board")
#ifdef CONFIG_MSM_DEBUG_UART
            .phys_io        = MSM_DEBUG_UART_PHYS,
            .io_pg_offst    = ((MSM_DEBUG_UART_BASE) >> 18) & 0xfffc,
#endif
            .boot_params    = PHYS_OFFSET + 0x100,
            .map_io     = msm7x2x_map_io,
            .init_irq   = msm7x2x_init_irq,
            .init_machine   = msm7x2x_init,
            .timer      = &msm_timer,
        MACHINE_END
#elif defined(CONFIG_MACH_GIO_KT)
        MACHINE_START(MSM7X27_SURF, "SHW-M290K Board")
#ifdef CONFIG_MSM_DEBUG_UART
            .phys_io        = MSM_DEBUG_UART_PHYS,
            .io_pg_offst    = ((MSM_DEBUG_UART_BASE) >> 18) & 0xfffc,
#endif
            .boot_params    = PHYS_OFFSET + 0x100,
            .map_io     = msm7x2x_map_io,
            .init_irq   = msm7x2x_init_irq,
            .init_machine   = msm7x2x_init,
            .timer      = &msm_timer,
        MACHINE_END
#elif defined(CONFIG_MACH_JUNO_SKT)
        MACHINE_START(MSM7X27_SURF, "SHW-M340S Board")
#ifdef CONFIG_MSM_DEBUG_UART
            .phys_io        = MSM_DEBUG_UART_PHYS,
            .io_pg_offst    = ((MSM_DEBUG_UART_BASE) >> 18) & 0xfffc,
#endif
            .boot_params    = PHYS_OFFSET + 0x100,
            .map_io     = msm7x2x_map_io,
            .init_irq   = msm7x2x_init_irq,
            .init_machine   = msm7x2x_init,
            .timer      = &msm_timer,
        MACHINE_END
#elif defined(CONFIG_MACH_JUNO_KT)
        MACHINE_START(MSM7X27_SURF, "SHW-M340K Board")
#ifdef CONFIG_MSM_DEBUG_UART
            .phys_io        = MSM_DEBUG_UART_PHYS,
            .io_pg_offst    = ((MSM_DEBUG_UART_BASE) >> 18) & 0xfffc,
#endif
            .boot_params    = PHYS_OFFSET + 0x100,
            .map_io     = msm7x2x_map_io,
            .init_irq   = msm7x2x_init_irq,
            .init_machine   = msm7x2x_init,
            .timer      = &msm_timer,
        MACHINE_END
#elif defined(CONFIG_MACH_COOPER)
MACHINE_START(MSM7X27_SURF, "GT-S5830 Board")
#ifdef CONFIG_MSM_DEBUG_UART
	.phys_io        = MSM_DEBUG_UART_PHYS,
	.io_pg_offst    = ((MSM_DEBUG_UART_BASE) >> 18) & 0xfffc,
#endif
	.boot_params	= PHYS_OFFSET + 0x100,
	.map_io		= msm7x2x_map_io,
	.init_irq	= msm7x2x_init_irq,
	.init_machine	= msm7x2x_init,
	.timer		= &msm_timer,
MACHINE_END

#else
MACHINE_START(MSM7X27_SURF, "GT-I5510 Board")
#ifdef CONFIG_MSM_DEBUG_UART
	.phys_io        = MSM_DEBUG_UART_PHYS,
	.io_pg_offst    = ((MSM_DEBUG_UART_BASE) >> 18) & 0xfffc,
#endif
	.boot_params	= PHYS_OFFSET + 0x100,
	.map_io		= msm7x2x_map_io,
	.init_irq	= msm7x2x_init_irq,
	.init_machine	= msm7x2x_init,
	.timer		= &msm_timer,
MACHINE_END
#endif

MACHINE_START(MSM7X27_FFA, "QCT MSM7x27 FFA")
#ifdef CONFIG_MSM_DEBUG_UART
	.phys_io        = MSM_DEBUG_UART_PHYS,
	.io_pg_offst    = ((MSM_DEBUG_UART_BASE) >> 18) & 0xfffc,
#endif
	.boot_params	= PHYS_OFFSET + 0x100,
	.map_io		= msm7x2x_map_io,
	.init_irq	= msm7x2x_init_irq,
	.init_machine	= msm7x2x_init,
	.timer		= &msm_timer,
MACHINE_END

MACHINE_START(MSM7X25_SURF, "QCT MSM7x25 SURF")
#ifdef CONFIG_MSM_DEBUG_UART
	.phys_io        = MSM_DEBUG_UART_PHYS,
	.io_pg_offst    = ((MSM_DEBUG_UART_BASE) >> 18) & 0xfffc,
#endif
	.boot_params	= PHYS_OFFSET + 0x100,
	.map_io		= msm7x2x_map_io,
	.init_irq	= msm7x2x_init_irq,
	.init_machine	= msm7x2x_init,
	.timer		= &msm_timer,
MACHINE_END

MACHINE_START(MSM7X25_FFA, "QCT MSM7x25 FFA")
#ifdef CONFIG_MSM_DEBUG_UART
	.phys_io        = MSM_DEBUG_UART_PHYS,
	.io_pg_offst    = ((MSM_DEBUG_UART_BASE) >> 18) & 0xfffc,
#endif
	.boot_params	= PHYS_OFFSET + 0x100,
	.map_io		= msm7x2x_map_io,
	.init_irq	= msm7x2x_init_irq,
	.init_machine	= msm7x2x_init,
	.timer		= &msm_timer,
MACHINE_END
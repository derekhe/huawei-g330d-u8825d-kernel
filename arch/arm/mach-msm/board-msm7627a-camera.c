/* Copyright (c) 2012, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/i2c.h>
#include <linux/i2c/sx150x.h>
#include <linux/gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <asm/mach-types.h>
#include <mach/msm_iomap.h>
#include <mach/board.h>
#include <mach/irqs-7xxx.h>
#include "devices-msm7x2xa.h"
#include "board-msm7627a.h"
#include <linux/hardware_self_adapt.h>
#ifdef CONFIG_HUAWEI_CAMERA
#define S5K5CA_IS_NOT_ON 0 
#endif
#include <mach/vreg.h>

#define GPIO_SKU1_CAM_VGA_SHDN    18
#define GPIO_SKU1_CAM_VGA_RESET_N 29
#define GPIO_SKU3_CAM_5MP_SHDN_N   5         /* PWDN */
#define GPIO_SKU3_CAM_5MP_CAMIF_RESET   6    /* (board_is(EVT))?123:121 RESET */
#define GPIO_SKU3_CAM_5MP_CAM_DRIVER_PWDN 30
#define GPIO_SKU7_CAM_VGA_SHDN    91
#define GPIO_SKU7_CAM_5MP_SHDN_N   93         /* PWDN */
#define GPIO_SKU7_CAM_5MP_CAMIF_RESET   23   /* (board_is(EVT))?123:121 RESET */

#ifdef CONFIG_MSM_CAMERA_V4L2
#define CAMERA_NAME_LEN 128 //this size is same with CAMERA_NAME_LEN in msm_sensor.h
char back_camera_name[CAMERA_NAME_LEN]  = {0};
char front_camera_name[CAMERA_NAME_LEN] = {0};
int cam_pwd = 119;
static uint32_t camera_off_gpio_table[] = {
	GPIO_CFG(7, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), /* VCM_PWD */
	GPIO_CFG(8, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /* RESET for front camera */  
	GPIO_CFG(9, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /* camera id */
	GPIO_CFG(15, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_16MA),
	GPIO_CFG(37, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /*PWD*/
	GPIO_CFG(49, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /* RESET */
	GPIO_CFG(60, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA), 
	GPIO_CFG(61, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA), 
	GPIO_CFG(119, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /*PWD*/
	GPIO_CFG(120, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /* PWD for front camera*/   
};

static uint32_t camera_on_gpio_table[] = {
	GPIO_CFG(7, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /* VCM_PWD */
	GPIO_CFG(8, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /* RESET for front camera*/  
	GPIO_CFG(9, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /* camera id */
	GPIO_CFG(15, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_16MA), /* MCLK */
	GPIO_CFG(37, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /*PWD*/
	GPIO_CFG(49, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /* RESET */
	GPIO_CFG(60, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA), 
	GPIO_CFG(61, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA), 
	GPIO_CFG(119, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /* PWD */
	GPIO_CFG(120, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /* PWD for front camera*/   
};

#ifdef CONFIG_OV7692
static struct gpio ov7692_cam_req_gpio[] = {
	{GPIO_SKU1_CAM_VGA_SHDN, GPIOF_DIR_OUT, "CAM_VGA_SHDN"},
	{GPIO_SKU1_CAM_VGA_RESET_N, GPIOF_DIR_OUT, "CAM_VGA_RESET"},
};

static struct msm_gpio_set_tbl ov7692_cam_gpio_set_tbl[] = {
	{GPIO_SKU1_CAM_VGA_SHDN, GPIOF_OUT_INIT_HIGH, 5000},
	{GPIO_SKU1_CAM_VGA_SHDN, GPIOF_OUT_INIT_LOW, 5000},
	{GPIO_SKU1_CAM_VGA_RESET_N, GPIOF_OUT_INIT_HIGH, 5000},
	{GPIO_SKU1_CAM_VGA_RESET_N, GPIOF_OUT_INIT_LOW, 5000},
};

static struct msm_camera_gpio_conf gpio_conf_ov7692 = {
	.cam_gpio_req_tbl = ov7692_cam_req_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(ov7692_cam_req_gpio),
	.cam_gpio_set_tbl = ov7692_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(ov7692_cam_gpio_set_tbl),
	.gpio_no_mux = 1,
};
#endif

#ifdef CONFIG_OV5647
static struct msm_camera_gpio_conf gpio_conf_ov5647 = {
	.camera_off_table = camera_off_gpio_table,
	.camera_on_table = camera_on_gpio_table,
	.gpio_no_mux = 1,
};
#endif

#ifdef CONFIG_MSM_CAMERA_FLASH
static struct msm_camera_sensor_flash_src msm_flash_src = {
	.flash_sr_type = MSM_CAMERA_FLASH_SRC_EXT,
	._fsrc.ext_driver_src.led_en = GPIO_CAM_GP_LED_EN1,
	._fsrc.ext_driver_src.led_flash_en = GPIO_CAM_GP_LED_EN2,
#ifdef CONFIG_HUAWEI_CAMERA
	.get_board_support_flash = board_support_flash,
#endif
};
#endif

/* camera vreg
 * the C8825D use Qualcomm wifi chipset, so the camera vreg is only L5
 * the array need to modify for board not using Qualcomm wifi chipset
 */
static struct camera_vreg_t msm_cam_vreg[] = {
	{ "wlan2", REG_LDO, 1300000, 1300000, 0},
	{ "bt", REG_LDO, 2850000, 2850000, 0},
};
static struct camera_vreg_t msm_cam_vreg_WIFI_QUALCOMM[] = {
	{ "wlan2", REG_LDO, 1300000, 1300000, 0},
	{ "usim2", REG_LDO, 2850000, 2850000, 0},
};
static struct camera_vreg_t msm_cam_vreg_HW_DS[] = {
	{ "wlan2", REG_LDO, 1300000, 1300000, 0},
};
static struct camera_vreg_t msm_cam_vreg_c8950d[] = {
	{ "bt", REG_LDO, 2850000, 2850000, 1000},
    { "wlan2", REG_LDO, 1300000, 1300000, 0},
};

void msm_get_camera_vreg (struct msm_camera_sensor_platform_info *info)
{
	struct msm_camera_sensor_platform_info *msm_info = info;
    if(machine_is_msm8x25_C8950D())
    {
        msm_info->cam_vreg = msm_cam_vreg_c8950d;
		msm_info->num_vreg = ARRAY_SIZE(msm_cam_vreg_c8950d);
    }
	else if (WIFI_QUALCOMM == get_hw_wifi_device_type())
	{
		if (HW_DS != get_hw_ds_type())
		{
			msm_info->cam_vreg = msm_cam_vreg_WIFI_QUALCOMM;
			msm_info->num_vreg = ARRAY_SIZE(msm_cam_vreg_WIFI_QUALCOMM);
		}
		else
		{
			msm_info->cam_vreg = msm_cam_vreg_HW_DS;
			msm_info->num_vreg = ARRAY_SIZE(msm_cam_vreg_HW_DS);
		}
	}
	else
	{
		msm_info->cam_vreg = msm_cam_vreg;
		msm_info->num_vreg = ARRAY_SIZE(msm_cam_vreg);
	}
}

struct msm_camera_device_platform_data msm_camera_device_data_csi1[] = {
	{
		.csid_core = 1,
		.is_csic = 1,
		.ioclk = {
			.vfe_clk_rate = 192000000,
		},
	},
	{
		.csid_core = 1,
		.is_csic = 1,
		.ioclk = {
			.vfe_clk_rate = 266667000,
		},
	},
};

struct msm_camera_device_platform_data msm_camera_device_data_csi0[] = {
	{
		.csid_core = 0,
		.is_csic = 1,
		.ioclk = {
			.vfe_clk_rate = 192000000,
		},
	},
	{
		.csid_core = 0,
		.is_csic = 1,
		.ioclk = {
			.vfe_clk_rate = 266667000,
		},
	},
};

/*all camera actuator use this struct, so we move it out of sk5k4e1*/
static struct i2c_board_info msm_act_main_cam_i2c_info = {
	I2C_BOARD_INFO("msm_actuator", 0x11),
};

#ifdef CONFIG_HUAWEI_SENSOR_S5K4E1
static struct msm_actuator_info msm_act_main_cam_4_info = {
	.board_info     = &msm_act_main_cam_i2c_info,
	.cam_name   = MSM_ACTUATOR_MAIN_CAM_4,
	.bus_id         = MSM_GSBI0_QUP_I2C_BUS_ID,
	.vcm_pwd        = 7,
	.vcm_enable     = 1,
};

static struct gpio s5k4e1_cam_req_gpio[] = {
	{49, GPIOF_DIR_OUT, "CAM_RESET"},
};

static struct msm_gpio_set_tbl s5k4e1_cam_gpio_set_tbl[] = {
	{49, GPIOF_OUT_INIT_LOW, 1000},
	{49, GPIOF_OUT_INIT_HIGH, 4000},
};

static struct msm_gpio_set_tbl s5k4e1_cam_gpio_config_tbl_power_down[] = {  
	{49 , GPIOF_OUT_INIT_LOW, 1000 }, 
};
static struct msm_camera_gpio_conf gpio_conf_s5k4e1 = {
	.camera_off_table = camera_off_gpio_table,
	.camera_off_table_size = ARRAY_SIZE(camera_off_gpio_table),
	.camera_on_table = camera_on_gpio_table,
	.camera_on_table_size = ARRAY_SIZE(camera_on_gpio_table),
	.cam_gpio_req_tbl = s5k4e1_cam_req_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(s5k4e1_cam_req_gpio),
	.cam_gpio_set_tbl = s5k4e1_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(s5k4e1_cam_gpio_set_tbl),
	.cam_gpio_config_tbl_power_down = s5k4e1_cam_gpio_config_tbl_power_down,
	.cam_gpio_config_tbl_power_down_size = ARRAY_SIZE(s5k4e1_cam_gpio_config_tbl_power_down),
	.gpio_no_mux = 1,
};

static struct msm_camera_sensor_flash_data flash_s5k4e1 = {
	.flash_type             = MSM_CAMERA_FLASH_LED,
	.flash_src              = &msm_flash_src
};

static struct msm_camera_sensor_platform_info sensor_board_info_s5k4e1 = {
	.mount_angle	= 90,
	.cam_vreg = msm_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_cam_vreg),
	.gpio_conf = &gpio_conf_s5k4e1,
};

static struct msm_camera_sensor_info msm_camera_sensor_s5k4e1_data = {
	.sensor_name    = (char*)back_camera_name,
	.sensor_reset_enable = 1,
	/*patch from Qualcomm*/
	.pdata                  = &msm_camera_device_data_csi1[0],
	.flash_data             = &flash_s5k4e1,
	.sensor_platform_info   = &sensor_board_info_s5k4e1,
	.csi_if                 = 1,
	.camera_type = BACK_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
	.slave_sensor = 0,
	.get_camera_vreg = msm_get_camera_vreg,
	.actuator_info = &msm_act_main_cam_4_info,
};
#endif

#ifdef CONFIG_HUAWEI_SENSOR_MT9P017
/*add the actuator info for mt9p017*/
static struct msm_actuator_info msm_act_main_cam_6_info = {
	.board_info     = &msm_act_main_cam_i2c_info,
	.cam_name   = MSM_ACTUATOR_MAIN_CAM_6,
	.bus_id         = MSM_GSBI0_QUP_I2C_BUS_ID,
	.vcm_pwd        = 7,
	.vcm_enable     = 1,
};
static struct gpio mt9p017_cam_req_gpio[] = {
	{49, GPIOF_DIR_OUT, "CAM_RESET"},
};

static struct msm_gpio_set_tbl mt9p017_cam_gpio_set_tbl[] = {
	{49, GPIOF_OUT_INIT_LOW, 1000},
	{49, GPIOF_OUT_INIT_HIGH, 4000},
};

static struct msm_camera_gpio_conf gpio_conf_mt9p017 = {
	.camera_off_table = camera_off_gpio_table,
	.camera_off_table_size = ARRAY_SIZE(camera_off_gpio_table),
	.camera_on_table = camera_on_gpio_table,
	.camera_on_table_size = ARRAY_SIZE(camera_on_gpio_table),
	.cam_gpio_req_tbl = mt9p017_cam_req_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(mt9p017_cam_req_gpio),
	.cam_gpio_set_tbl = mt9p017_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(mt9p017_cam_gpio_set_tbl),
	.gpio_no_mux = 1,
};

static struct msm_camera_sensor_flash_data flash_mt9p017 = {
	.flash_type             = MSM_CAMERA_FLASH_LED,
	.flash_src              = &msm_flash_src
};

static struct msm_camera_sensor_platform_info sensor_board_info_mt9p017 = {
	.mount_angle	= 90,
	.cam_vreg = msm_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_cam_vreg),
	.gpio_conf = &gpio_conf_mt9p017,
};

static struct msm_camera_sensor_info msm_camera_sensor_mt9p017_data = {
	.sensor_name    = (char*)back_camera_name,
	.sensor_reset_enable = 1,
	/*patch from Qualcomm*/
	.pdata                  = &msm_camera_device_data_csi1[0],
	.flash_data             = &flash_mt9p017,
	.sensor_platform_info   = &sensor_board_info_mt9p017,
	.csi_if                 = 1,
	.camera_type = BACK_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
	.slave_sensor = 0,
	.get_camera_vreg = msm_get_camera_vreg,
	.actuator_info = &msm_act_main_cam_6_info,
};
#endif

#ifdef CONFIG_OV5647

static struct msm_actuator_info msm_act_main_cam_5_info = {
	.board_info     = &msm_act_main_cam_i2c_info,
	.cam_name   = MSM_ACTUATOR_MAIN_CAM_5,
	.bus_id         = MSM_GSBI0_QUP_I2C_BUS_ID,
	.vcm_pwd        = GPIO_SKU3_CAM_5MP_CAM_DRIVER_PWDN,
	.vcm_enable     = 1,
};

static struct msm_camera_sensor_platform_info sensor_board_info_ov5647 = {
	.mount_angle = 90,
	.cam_vreg = msm_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_cam_vreg),
	.gpio_conf = &gpio_conf_ov5647,
};

/*patch from Qualcomm*/
static struct camera_vreg_t cam_ov5647_gpio_vreg[] = {
	{"cam0_avdd", REG_GPIO, 0, 0, 0},
	{"cam0_vdd", REG_GPIO, 0, 0, 0},
};
static struct msm_camera_sensor_flash_src msm_flash_src_ov5647 = {
	.flash_sr_type = MSM_CAMERA_FLASH_SRC_LED1,
	._fsrc.ext_driver_src.led_en = 13,
	._fsrc.ext_driver_src.led_flash_en = 32,
};

static struct msm_camera_sensor_flash_data flash_ov5647 = {
	.flash_type             = MSM_CAMERA_FLASH_LED,
	.flash_src              = &msm_flash_src_ov5647,
};

static struct msm_camera_sensor_info msm_camera_sensor_ov5647_data = {
	.sensor_name    = "ov5647",
	.sensor_reset_enable = 1,
	/*patch from Qualcomm*/
	.sensor_reset   = GPIO_SKU3_CAM_5MP_CAMIF_RESET,
	.sensor_pwd     = GPIO_SKU3_CAM_5MP_SHDN_N,
	.pdata          = &msm_camera_device_data_csi1[0],
	.flash_data     = &flash_ov5647,
	.sensor_platform_info   = &sensor_board_info_ov5647,
	.csi_if                 = 1,
	.camera_type	= BACK_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
	.slave_sensor = 0,
	.actuator_info = &msm_act_main_cam_5_info,
};

#endif

#ifdef CONFIG_HUAWEI_SENSOR_MT9E013
static struct gpio mt9e013_cam_req_gpio[] = {
	{49, GPIOF_DIR_OUT, "CAM_RESET"},
};

static struct msm_gpio_set_tbl mt9e013_cam_gpio_set_tbl[] = {
	{49, GPIOF_OUT_INIT_LOW, 10000},  //set the flag of gpio, then usleep the times
	{49, GPIOF_OUT_INIT_HIGH, 10000},
};
static struct msm_camera_gpio_conf gpio_conf_mt9e013 = {
	.camera_off_table = camera_off_gpio_table,
	.camera_off_table_size = ARRAY_SIZE(camera_off_gpio_table),
	.camera_on_table = camera_on_gpio_table,
	.camera_on_table_size = ARRAY_SIZE(camera_on_gpio_table),
	.cam_gpio_req_tbl = mt9e013_cam_req_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(mt9e013_cam_req_gpio),
	.cam_gpio_set_tbl = mt9e013_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(mt9e013_cam_gpio_set_tbl),
	.gpio_no_mux = 1,
};

static struct msm_camera_sensor_flash_data flash_mt9e013 = {
	.flash_type             = MSM_CAMERA_FLASH_LED,
	.flash_src              = &msm_flash_src
};

static struct msm_camera_sensor_platform_info sensor_board_info_mt9e013 = {
	.mount_angle	= 90,
	.cam_vreg = msm_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_cam_vreg),
	.gpio_conf = &gpio_conf_mt9e013,
};

static struct msm_camera_sensor_info msm_camera_sensor_mt9e013_data = {
	.sensor_name    = (char*)back_camera_name,
	.sensor_reset_enable = 1,
	/*patch from Qualcomm*/
	.pdata                  = &msm_camera_device_data_csi1[1],
	.flash_data             = &flash_mt9e013,
	.sensor_platform_info   = &sensor_board_info_mt9e013,
	.csi_if                 = 1,
	.camera_type = BACK_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
	.slave_sensor = 0,
	.get_camera_vreg = msm_get_camera_vreg,
};
#endif
#ifdef CONFIG_HUAWEI_CAMERA_SENSOR_MT9V113
static struct gpio mt9v113_cam_req_gpio[] = {
	{8, GPIOF_DIR_OUT, "CAM_RESET"},
	{120, GPIOF_DIR_OUT, "CAM_PWD"},
};

static struct msm_gpio_set_tbl mt9v113_cam_gpio_req_init_tbl[] = {
	{120, GPIOF_OUT_INIT_LOW, 1000},  //set the flag of gpio, then usleep the times
	{8, GPIOF_OUT_INIT_HIGH, 10000},
};

static struct msm_gpio_set_tbl mt9v113_cam_gpio_set_tbl[] = {
	{8, GPIOF_OUT_INIT_LOW, 10000},
	{8, GPIOF_OUT_INIT_HIGH, 10000},
};

static struct msm_gpio_set_tbl mt9v113_cam_gpio_config_tbl_power_down[] = {
	{8 ,   GPIOF_OUT_INIT_LOW,  1000 },
	{120 , GPIOF_OUT_INIT_HIGH, 1000 },
};
static struct msm_camera_gpio_conf gpio_conf_mt9v113 = {
	.camera_off_table = camera_off_gpio_table,
	.camera_off_table_size = ARRAY_SIZE(camera_off_gpio_table),
	.camera_on_table = camera_on_gpio_table,
	.camera_on_table_size = ARRAY_SIZE(camera_on_gpio_table),
	.cam_gpio_req_tbl = mt9v113_cam_req_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(mt9v113_cam_req_gpio),
	.cam_gpio_req_init_tbl = mt9v113_cam_gpio_req_init_tbl,
	.cam_gpio_req_init_tbl_size = ARRAY_SIZE(mt9v113_cam_gpio_req_init_tbl),
	.cam_gpio_set_tbl = mt9v113_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(mt9v113_cam_gpio_set_tbl),
	.cam_gpio_config_tbl_power_down = mt9v113_cam_gpio_config_tbl_power_down,
	.cam_gpio_config_tbl_power_down_size = ARRAY_SIZE(mt9v113_cam_gpio_config_tbl_power_down),
	.gpio_no_mux = 1,
};

static struct msm_camera_sensor_flash_data flash_mt9v113 = {
	.flash_type             = MSM_CAMERA_FLASH_NONE,
	.flash_src              = &msm_flash_src
};

static struct msm_camera_sensor_platform_info sensor_board_info_mt9v113 = {
	.mount_angle	= 270,
	.cam_vreg = msm_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_cam_vreg),
	.gpio_conf = &gpio_conf_mt9v113, 
};

static struct msm_camera_sensor_info msm_camera_sensor_mt9v113_data = {
	.sensor_name    = (char*)front_camera_name,
	.sensor_reset_enable = 1,
	/*patch from Qualcomm*/
	.pdata                  = &msm_camera_device_data_csi0[0],
	.flash_data             = &flash_mt9v113,
	.sensor_platform_info   = &sensor_board_info_mt9v113,
	.csi_if                 = 1,
	.camera_type = FRONT_CAMERA_2D,
	.sensor_type = YUV_SENSOR,
	.slave_sensor = 1,
	.get_camera_vreg = msm_get_camera_vreg,
};
#endif

#ifdef CONFIG_HUAWEI_CAMERA_SENSOR_IMX105
static struct msm_actuator_info msm_act_main_cam_7_info = {
	.board_info     = &msm_act_main_cam_i2c_info,
	.cam_name   = MSM_ACTUATOR_MAIN_CAM_7,
	.bus_id         = MSM_GSBI0_QUP_I2C_BUS_ID,
	.vcm_pwd        = 7,
	.vcm_enable     = 1,
};

static struct gpio imx105_cam_req_gpio[] = {
	{49, GPIOF_DIR_OUT, "CAM_RESET"},
};

//gpio 34 as the switch of CAM_DVDD(ldo)
static struct msm_gpio_set_tbl imx105_cam_gpio_req_init_tbl[] = {
    {34, GPIOF_OUT_INIT_HIGH, 1000}, 
};

static struct msm_gpio_set_tbl imx105_cam_gpio_set_tbl[] = {
	{49, GPIOF_OUT_INIT_HIGH, 1000},
};

static struct msm_gpio_set_tbl imx105_cam_gpio_config_tbl_power_down[] = {
    {34, GPIOF_OUT_INIT_HIGH, 1000}, 
};

static struct msm_camera_gpio_conf gpio_conf_imx105 = {
	.camera_off_table = camera_off_gpio_table,
	.camera_off_table_size = ARRAY_SIZE(camera_off_gpio_table),
	.camera_on_table = camera_on_gpio_table,
	.camera_on_table_size = ARRAY_SIZE(camera_on_gpio_table),
	.cam_gpio_req_tbl = imx105_cam_req_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(imx105_cam_req_gpio),
	.cam_gpio_req_init_tbl = imx105_cam_gpio_req_init_tbl,
	.cam_gpio_req_init_tbl_size = ARRAY_SIZE(imx105_cam_gpio_req_init_tbl),
	.cam_gpio_config_tbl_power_down = imx105_cam_gpio_config_tbl_power_down,
	.cam_gpio_config_tbl_power_down_size = ARRAY_SIZE(imx105_cam_gpio_config_tbl_power_down),
	.cam_gpio_set_tbl = imx105_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(imx105_cam_gpio_set_tbl),
	.gpio_no_mux = 1,
};

static struct msm_camera_sensor_flash_data flash_imx105 = {
	.flash_type             = MSM_CAMERA_FLASH_LED,
	.flash_src              = &msm_flash_src
};

static struct msm_camera_sensor_platform_info sensor_board_info_imx105 = {
	.mount_angle	= 270,
	.cam_vreg = msm_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_cam_vreg),
	.gpio_conf = &gpio_conf_imx105,
};

static struct msm_camera_sensor_info msm_camera_sensor_imx105_data = {
	.sensor_name    = (char*)back_camera_name,
	.sensor_reset_enable = 1,
	.pdata                  = &msm_camera_device_data_csi1[0],
	.flash_data             = &flash_imx105,
	.sensor_platform_info   = &sensor_board_info_imx105,
	.csi_if                 = 1,
	.camera_type = BACK_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
	.slave_sensor = 0,
	.get_camera_vreg = msm_get_camera_vreg,
	.actuator_info = &msm_act_main_cam_7_info,
};
#endif
#ifdef CONFIG_HUAWEI_CAMERA_SENSOR_S5K3H2
static struct msm_actuator_info msm_act_main_cam_8_info = {
	.board_info     = &msm_act_main_cam_i2c_info,
	.cam_name   = MSM_ACTUATOR_MAIN_CAM_8,
	.bus_id         = MSM_GSBI0_QUP_I2C_BUS_ID,
	.vcm_pwd        = 7,
	.vcm_enable     = 1,
};

static struct gpio s5k3h2_cam_req_gpio[] = {
	{49, GPIOF_DIR_OUT, "CAM_RESET"},
};

//gpio 34 as the switch of CAM_DVDD(ldo)
static struct msm_gpio_set_tbl s5k3h2_cam_gpio_req_init_tbl[] = {
    {34, GPIOF_OUT_INIT_HIGH, 1000}, 
};

static struct msm_gpio_set_tbl s5k3h2_cam_gpio_set_tbl[] = {
	{49, GPIOF_OUT_INIT_HIGH, 1000},
};

static struct msm_gpio_set_tbl s5k3h2_cam_gpio_config_tbl_power_down[] = {
    {34, GPIOF_OUT_INIT_HIGH, 1000}, 
};

static struct msm_camera_gpio_conf gpio_conf_s5k3h2 = {
	.camera_off_table = camera_off_gpio_table,
	.camera_off_table_size = ARRAY_SIZE(camera_off_gpio_table),
	.camera_on_table = camera_on_gpio_table,
	.camera_on_table_size = ARRAY_SIZE(camera_on_gpio_table),
	.cam_gpio_req_tbl = s5k3h2_cam_req_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(s5k3h2_cam_req_gpio),
	.cam_gpio_req_init_tbl = s5k3h2_cam_gpio_req_init_tbl,
	.cam_gpio_req_init_tbl_size = ARRAY_SIZE(s5k3h2_cam_gpio_req_init_tbl),
	.cam_gpio_config_tbl_power_down = s5k3h2_cam_gpio_config_tbl_power_down,
	.cam_gpio_config_tbl_power_down_size = ARRAY_SIZE(s5k3h2_cam_gpio_config_tbl_power_down),
	.cam_gpio_set_tbl = s5k3h2_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(s5k3h2_cam_gpio_set_tbl),
	.gpio_no_mux = 1,
};

static struct msm_camera_sensor_flash_data flash_s5k3h2 = {
	.flash_type             = MSM_CAMERA_FLASH_LED,
	.flash_src              = &msm_flash_src
};

static struct msm_camera_sensor_platform_info sensor_board_info_s5k3h2 = {
	.mount_angle	= 270,
	.cam_vreg = msm_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_cam_vreg),
	.gpio_conf = &gpio_conf_s5k3h2,
};

static struct msm_camera_sensor_info msm_camera_sensor_s5k3h2_data = {
	.sensor_name    = (char*)back_camera_name,
	.sensor_reset_enable = 1,
	.pdata                  = &msm_camera_device_data_csi1[0],
	.flash_data             = &flash_s5k3h2,
	.sensor_platform_info   = &sensor_board_info_s5k3h2,
	.csi_if                 = 1,
	.camera_type = BACK_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
	.slave_sensor = 0,
	.get_camera_vreg = msm_get_camera_vreg,
	.actuator_info = &msm_act_main_cam_8_info,
};
#endif
static struct platform_device msm_camera_server = {
	.name = "msm_cam_server",
	.id = 0,
};

static void __init msm7x27a_init_cam(void)
{
	if (!(machine_is_msm7x27a_ffa() || machine_is_msm7625a_ffa()
				|| machine_is_msm7627a_qrd1()
				|| machine_is_msm8625_ffa())) {
	}
	/*patch from Qualcomm*/
	if (machine_is_msm8625_evb()) {
#ifdef CONFIG_OV5647
		sensor_board_info_ov5647.cam_vreg =
			cam_ov5647_gpio_vreg;
		sensor_board_info_ov5647.num_vreg =
			ARRAY_SIZE(cam_ov5647_gpio_vreg);
#endif
	}
	platform_device_register(&msm_camera_server);
	if (machine_is_msm8625_surf() || machine_is_msm8625_evb()
			|| machine_is_msm8625_evt()
			|| machine_is_msm8625_qrd7()
			||  machine_is_msm8x25_U8825()
			|| machine_is_msm8x25_U8825D()
			|| machine_is_msm8x25_C8825D()
			|| machine_is_msm8x25_C8950D()
			|| machine_is_msm8x25_U8950D()
			|| machine_is_msm8x25_U8950()) {
		platform_device_register(&msm8625_device_csic0);
		platform_device_register(&msm8625_device_csic1);
	} else {
		platform_device_register(&msm7x27a_device_csic0);
		platform_device_register(&msm7x27a_device_csic1);
	}
	if (machine_is_msm8625_evb()
			|| machine_is_msm8625_evt()
			|| machine_is_msm8625_qrd7())
		*(int *) msm7x27a_device_clkctl.dev.platform_data = 1;
	platform_device_register(&msm7x27a_device_clkctl);
	platform_device_register(&msm7x27a_device_vfe);
}

static struct i2c_board_info i2c_camera_devices[] = {
#ifdef CONFIG_OV5647
	{
		/*The I2C address was repeat,rework I2C address avoidance repeat*/
		I2C_BOARD_INFO("ov5647", 0x6C >> 3),
		.platform_data = &msm_camera_sensor_ov5647_data,
	},
#endif
#ifdef CONFIG_HUAWEI_SENSOR_MT9E013
	{
		I2C_BOARD_INFO("mt9e013", 0x6C >> 2),
		.platform_data = &msm_camera_sensor_mt9e013_data,
	},
#endif
#ifdef CONFIG_HUAWEI_SENSOR_S5K4E1
	{
		I2C_BOARD_INFO("s5k4e1", 0x6E),
		.platform_data = &msm_camera_sensor_s5k4e1_data,
	},
#endif
#ifdef CONFIG_HUAWEI_SENSOR_MT9P017
	{
		I2C_BOARD_INFO("mt9p017", 0x6C),
		.platform_data = &msm_camera_sensor_mt9p017_data,
	},
#endif
#ifdef CONFIG_HUAWEI_FEATURE_TPS61310
	{
		I2C_BOARD_INFO("tps61310" , 0x33),
	},
#endif
#ifdef CONFIG_HUAWEI_CAMERA_SENSOR_MT9V113
	{
		I2C_BOARD_INFO("mt9v113", 0x7A),
		.platform_data = &msm_camera_sensor_mt9v113_data,
	},
#endif
#ifdef CONFIG_HUAWEI_CAMERA_SENSOR_IMX105
    {
   		I2C_BOARD_INFO("imx105_sunny", 0x6C >> 1),
		.platform_data = &msm_camera_sensor_imx105_data, 
    },
#endif
#ifdef CONFIG_HUAWEI_CAMERA_SENSOR_S5K3H2
    {
   		I2C_BOARD_INFO("s5k3h2", 0x6F),
		.platform_data = &msm_camera_sensor_s5k3h2_data, 
    },
#endif
};
#else
static uint32_t camera_off_gpio_table[] = {
	GPIO_CFG(8, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /* RESET For mt9v113 */
	GPIO_CFG(15, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_16MA),
	GPIO_CFG(32, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /*PWD for M660 camera*/
	GPIO_CFG(37, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /*PWD for U8185 camera*/
	GPIO_CFG(49, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /* RESET */

	/* Camera I2C config like normal gpio with value 0 when camera turn off */
	GPIO_CFG(60, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA), 
	GPIO_CFG(61, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA), 
	GPIO_CFG(119, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /*PWD*/
	GPIO_CFG(120, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /*PWD for mt9v113 */
};

static uint32_t camera_on_gpio_table[] = {
	GPIO_CFG(8,  0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /* RESET For mt9v113 */
	/*increase the driving capability of  MCLK*/
	GPIO_CFG(15, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA), /* MCLK */
	GPIO_CFG(32, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /*PWD for M660 camera*/
	GPIO_CFG(37, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /*PWD for U8185 camera*/
	GPIO_CFG(49, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /* RESET */

    GPIO_CFG(7, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /* VCM_PWD */
	/* Camera I2C config to I2C when camera start */
	GPIO_CFG(60, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA), 
	GPIO_CFG(61, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA), 
	GPIO_CFG(119, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /* PWD */
	GPIO_CFG(120, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /* PWD for mt9v113 */
};

#ifdef CONFIG_MSM_CAMERA_FLASH
static struct msm_camera_sensor_flash_src msm_flash_src = {
	.flash_sr_type = MSM_CAMERA_FLASH_SRC_EXT,
	._fsrc.ext_driver_src.led_en = GPIO_CAM_GP_LED_EN1,
	._fsrc.ext_driver_src.led_flash_en = GPIO_CAM_GP_LED_EN2,
};
#endif

static struct regulator_bulk_data regs_camera[] = {
	{ .supply = "msme1", .min_uV = 1800000, .max_uV = 1800000 },
	{ .supply = "gp2",   .min_uV = 2850000, .max_uV = 2850000 },
	{ .supply = "usb2",  .min_uV = 1800000, .max_uV = 1800000 },
};

static void qrd1_camera_gpio_cfg(void)
{

	int rc = 0;

	rc = gpio_request(QRD_GPIO_CAM_5MP_SHDN_EN, "ov5640");
	if (rc < 0)
		pr_err("%s: gpio_request---GPIO_CAM_5MP_SHDN_EN failed!",
				__func__);


	rc = gpio_tlmm_config(GPIO_CFG(QRD_GPIO_CAM_5MP_SHDN_EN, 0,
				GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP,
				GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc < 0) {
		pr_err("%s: unable to enable Power Down gpio for main"
				"camera!\n", __func__);
		gpio_free(QRD_GPIO_CAM_5MP_SHDN_EN);
	}


	rc = gpio_request(QRD_GPIO_CAM_5MP_RESET, "ov5640");
	if (rc < 0) {
		pr_err("%s: gpio_request---GPIO_CAM_5MP_RESET failed!",
				__func__);
		gpio_free(QRD_GPIO_CAM_5MP_SHDN_EN);
	}


	rc = gpio_tlmm_config(GPIO_CFG(QRD_GPIO_CAM_5MP_RESET, 0,
				GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP,
				GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc < 0) {
		pr_err("%s: unable to enable reset gpio for main camera!\n",
				__func__);
		gpio_free(QRD_GPIO_CAM_5MP_RESET);
	}

	rc = gpio_request(QRD_GPIO_CAM_3MP_PWDN, "ov7692");
	if (rc < 0)
		pr_err("%s: gpio_request---GPIO_CAM_3MP_PWDN failed!",
				__func__);

	rc = gpio_tlmm_config(GPIO_CFG(QRD_GPIO_CAM_3MP_PWDN, 0,
				GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP,
				GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc < 0) {
		pr_err("%s: unable to enable Power Down gpio for front"
				"camera!\n", __func__);
		gpio_free(QRD_GPIO_CAM_3MP_PWDN);
	}

	gpio_direction_output(QRD_GPIO_CAM_5MP_SHDN_EN, 1);
	gpio_direction_output(QRD_GPIO_CAM_5MP_RESET, 1);
	gpio_direction_output(QRD_GPIO_CAM_3MP_PWDN, 1);
}
#endif

#ifdef CONFIG_OV5647
static void evb_camera_gpio_cfg(void)
{
	int rc = 0;

	rc = gpio_request(msm_camera_sensor_ov5647_data.sensor_pwd, "ov5647");
	if (rc < 0)
		pr_err("%s: gpio_request OV5647 sensor_pwd: %d failed!",
			 __func__, msm_camera_sensor_ov5647_data.sensor_pwd);

	rc = gpio_tlmm_config(GPIO_CFG(msm_camera_sensor_ov5647_data.sensor_pwd,
				0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN,
				GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc < 0) {
		pr_err("%s:unable to enable Powr Dwn gpio for main camera!\n",
			 __func__);
		gpio_free(msm_camera_sensor_ov5647_data.sensor_pwd);
	}

	rc = gpio_direction_output(msm_camera_sensor_ov5647_data.sensor_pwd, 1);
	if (rc < 0)
		pr_err("%s: unable to set gpio: %d direction for ov5647 camera\n",
			__func__, msm_camera_sensor_ov5647_data.sensor_pwd);

	rc = gpio_request(msm_camera_sensor_ov5647_data.sensor_reset, "ov5647");
	if (rc < 0)
		pr_err("%s: gpio_request OV5647 sensor_reset: %d failed!",
			 __func__, msm_camera_sensor_ov5647_data.sensor_reset);

	rc = gpio_tlmm_config(GPIO_CFG(
				msm_camera_sensor_ov5647_data.sensor_reset,
				0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN,
				GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc < 0) {
		pr_err("%s: unable to enable reset gpio for main camera!\n",
			 __func__);
		gpio_free(msm_camera_sensor_ov5647_data.sensor_reset);
	}

	rc = gpio_direction_output(
			msm_camera_sensor_ov5647_data.sensor_reset, 1);
	if (rc < 0)
		pr_err("%s: unable to set gpio: %d direction for ov5647 camera\n",
			__func__, msm_camera_sensor_ov5647_data.sensor_reset);

}
#endif

#ifndef CONFIG_MSM_CAMERA_V4L2

static void msm_camera_vreg_config(int vreg_en)
{
	int rc = vreg_en ?
		regulator_bulk_enable(ARRAY_SIZE(regs_camera), regs_camera) :
		regulator_bulk_disable(ARRAY_SIZE(regs_camera), regs_camera);

	if (rc)
		pr_err("%s: could not %sable regulators: %d\n",
				__func__, vreg_en ? "en" : "dis", rc);
}

static int config_gpio_table(uint32_t *table, int len)
{
	int rc = 0, i = 0;

	for (i = 0; i < len; i++) {
		rc = gpio_tlmm_config(table[i], GPIO_CFG_ENABLE);
		if (rc) {
			pr_err("%s not able to get gpio\n", __func__);
			for (i--; i >= 0; i--)
				gpio_tlmm_config(camera_off_gpio_table[i],
							GPIO_CFG_ENABLE);
			break;
		}
	}
	return rc;
}

#ifdef CONFIG_HUAWEI_CAMERA
/* set and get the value of s5k5ca_is_on*/
static void set_s5k5ca_is_on(int s5k5ca_probe_success)
{
	s5k5ca_is_on = s5k5ca_probe_success ;
}

static int get_s5k5ca_is_on(void)
{
	return  s5k5ca_is_on;
}
#endif
static int config_camera_on_gpios_rear(void)
{
	int rc = 0;

	if (machine_is_msm7x27a_ffa() || machine_is_msm7625a_ffa()
				|| machine_is_msm7627a_qrd1()
				|| machine_is_msm8625_ffa())
		msm_camera_vreg_config(1);

	rc = config_gpio_table(camera_on_gpio_table,
			ARRAY_SIZE(camera_on_gpio_table));
	if (rc < 0) {
		pr_err("%s: CAMSENSOR gpio table request"
		"failed\n", __func__);
		return rc;
	}

	return rc;
}

static void config_camera_off_gpios_rear(void)
{
	if (machine_is_msm7x27a_ffa() || machine_is_msm7625a_ffa()
				|| machine_is_msm7627a_qrd1()
				|| machine_is_msm8625_ffa())
		msm_camera_vreg_config(0);

	config_gpio_table(camera_off_gpio_table,
			ARRAY_SIZE(camera_off_gpio_table));
}

static int config_camera_on_gpios_front(void)
{
	int rc = 0;

	if (machine_is_msm7x27a_ffa() || machine_is_msm7625a_ffa()
				|| machine_is_msm7627a_qrd1()
				|| machine_is_msm8625_ffa())
		msm_camera_vreg_config(1);

	rc = config_gpio_table(camera_on_gpio_table,
			ARRAY_SIZE(camera_on_gpio_table));
	if (rc < 0) {
		pr_err("%s: CAMSENSOR gpio table request"
			"failed\n", __func__);
		return rc;
	}

	return rc;
}

static void config_camera_off_gpios_front(void)
{
	if (machine_is_msm7x27a_ffa() || machine_is_msm7625a_ffa()
				|| machine_is_msm7627a_qrd1()
				|| machine_is_msm8625_ffa())
		msm_camera_vreg_config(0);

	config_gpio_table(camera_off_gpio_table,
			ARRAY_SIZE(camera_off_gpio_table));
}

struct msm_camera_device_platform_data msm_camera_device_data_rear = {
	.camera_gpio_on		= config_camera_on_gpios_rear,
	.camera_gpio_off	= config_camera_off_gpios_rear,
	.ioext.csiphy		= 0xA1000000,
	.ioext.csisz		= 0x00100000,
	.ioext.csiirq		= INT_CSI_IRQ_1,
	.ioclk.mclk_clk_rate	= 24000000,
	.ioclk.vfe_clk_rate	= 192000000,
	.ioext.appphy		= MSM7XXX_CLK_CTL_PHYS,
	.ioext.appsz		= MSM7XXX_CLK_CTL_SIZE,
};

struct msm_camera_device_platform_data msm_camera_device_data_front = {
	.camera_gpio_on		= config_camera_on_gpios_front,
	.camera_gpio_off	= config_camera_off_gpios_front,
	.ioext.csiphy		= 0xA0F00000,
	.ioext.csisz		= 0x00100000,
	.ioext.csiirq		= INT_CSI_IRQ_0,
	.ioclk.mclk_clk_rate	= 24000000,
	.ioclk.vfe_clk_rate	= 192000000,
	.ioext.appphy		= MSM7XXX_CLK_CTL_PHYS,
	.ioext.appsz		= MSM7XXX_CLK_CTL_SIZE,
};

#ifdef CONFIG_OV5647

static struct msm_camera_sensor_platform_info ov5647_sensor_7627a_info = {
	.mount_angle = 90
};

static struct msm_camera_sensor_flash_src msm_flash_src_ov5647 = {
	.flash_sr_type = MSM_CAMERA_FLASH_SRC_LED,
	._fsrc.led_src.led_name = "flashlight",
	._fsrc.led_src.led_name_len = 10,
};

static struct msm_camera_sensor_flash_data flash_ov5647 = {
	.flash_type             = MSM_CAMERA_FLASH_LED,
	.flash_src              = &msm_flash_src_ov5647,
};

static struct msm_camera_sensor_info msm_camera_sensor_ov5647_data = {
	.sensor_name    = "ov5647",
	.sensor_reset_enable = 1,
	.sensor_reset   = GPIO_SKU3_CAM_5MP_CAMIF_RESET,
	.pmic_gpio_enable  = 1,
	.sensor_pwd     = GPIO_SKU3_CAM_5MP_SHDN_N,
	.vcm_pwd        = GPIO_SKU3_CAM_5MP_CAM_DRIVER_PWDN,
	.vcm_enable     = 1,
	.pdata          = &msm_camera_device_data_rear,
	.flash_data     = &flash_ov5647,
	.sensor_platform_info   = &ov5647_sensor_7627a_info,
	.csi_if                 = 1
};

static struct platform_device msm_camera_sensor_ov5647 = {
	.name      = "msm_camera_ov5647",
	.dev       = {
		.platform_data = &msm_camera_sensor_ov5647_data,
	},
};
#endif

#ifdef CONFIG_HUAWEI_SENSOR_S5K4E1
static struct msm_camera_sensor_platform_info s5k4e1_sensor_7627a_info = {
	.mount_angle = 90
};

static struct msm_camera_sensor_flash_data flash_s5k4e1 = {
	.flash_type	     = MSM_CAMERA_FLASH_LED,
	.flash_src	      = &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_s5k4e1_data = {
	.sensor_name		= "s5k4e1",
	.sensor_reset_enable	= 1,
	.sensor_reset		= GPIO_CAM_GP_CAMIF_RESET_N,
	.pmic_gpio_enable       = 0,
	.sensor_pwd	     = 85,
	.vcm_pwd		= GPIO_CAM_GP_CAM_PWDN,
	.vcm_enable	     = 1,
	.pdata			= &msm_camera_device_data_rear,
	.flash_data	     = &flash_s5k4e1,
	.sensor_platform_info   = &s5k4e1_sensor_7627a_info,
	.csi_if			= 1
};

static struct platform_device msm_camera_sensor_s5k4e1 = {
	.name   = "msm_camera_s5k4e1",
	.dev    = {
		.platform_data = &msm_camera_sensor_s5k4e1_data,
	},
};
#endif

#ifdef CONFIG_IMX072
static struct msm_camera_sensor_platform_info imx072_sensor_7627a_info = {
	.mount_angle = 90
};

static struct msm_camera_sensor_flash_data flash_imx072 = {
	.flash_type	     = MSM_CAMERA_FLASH_LED,
	.flash_src	      = &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_imx072_data = {
	.sensor_name		= "imx072",
	.sensor_reset_enable	= 1,
	.sensor_reset		= GPIO_CAM_GP_CAMIF_RESET_N, /* TODO 106,*/
	.pmic_gpio_enable       = 0,
	.sensor_pwd	     = 85,
	.vcm_pwd		= GPIO_CAM_GP_CAM_PWDN,
	.vcm_enable	     = 1,
	.pdata			= &msm_camera_device_data_rear,
	.flash_data	     = &flash_imx072,
	.sensor_platform_info	= &imx072_sensor_7627a_info,
	.csi_if			= 1
};

static struct platform_device msm_camera_sensor_imx072 = {
	.name   = "msm_camera_imx072",
	.dev    = {
		.platform_data = &msm_camera_sensor_imx072_data,
	},
};
#endif

static struct msm_camera_sensor_info msm_camera_sensor_ov9726_data;
#ifdef CONFIG_WEBCAM_OV9726
static struct msm_camera_sensor_platform_info ov9726_sensor_7627a_info = {
	.mount_angle = 90
};

static struct msm_camera_sensor_flash_data flash_ov9726 = {
	.flash_type	     = MSM_CAMERA_FLASH_NONE,
	.flash_src	      = &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_ov9726_data = {
	.sensor_name		= "ov9726",
	.sensor_reset_enable	= 0,
	.sensor_reset		= GPIO_CAM_GP_CAM1MP_XCLR,
	.pmic_gpio_enable       = 0,
	.sensor_pwd	     = 85,
	.vcm_pwd		= 1,
	.vcm_enable	     = 0,
	.pdata			= &msm_camera_device_data_front,
	.flash_data	     = &flash_ov9726,
	.sensor_platform_info   = &ov9726_sensor_7627a_info,
	.csi_if			= 1
};

static struct platform_device msm_camera_sensor_ov9726 = {
	.name   = "msm_camera_ov9726",
	.dev    = {
		.platform_data = &msm_camera_sensor_ov9726_data,
	},
};
#else
static inline void msm_camera_vreg_init(void) { }
#endif

#ifdef CONFIG_MT9E013
static struct msm_camera_sensor_platform_info mt9e013_sensor_7627a_info = {
	.mount_angle = 90
};

static struct msm_camera_sensor_flash_data flash_mt9e013 = {
	.flash_type = MSM_CAMERA_FLASH_LED,
	.flash_src  = &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_mt9e013_data = {
	.sensor_name		= "mt9e013",
	.sensor_reset		= 0,
	.sensor_reset_enable	= 1,
	.pmic_gpio_enable       = 0,
	.sensor_pwd		= 85,
	.vcm_pwd		= 1,
	.vcm_enable		= 0,
	.pdata		= &msm_camera_device_data_rear,
	.flash_data		= &flash_mt9e013,
	.sensor_platform_info   = &mt9e013_sensor_7627a_info,
	.csi_if		= 1
};

static struct platform_device msm_camera_sensor_mt9e013 = {
	.name      = "msm_camera_mt9e013",
	.dev       = {
		.platform_data = &msm_camera_sensor_mt9e013_data,
	},
};
#endif
#ifdef CONFIG_HUAWEI_SENSOR_MT9P017
static struct msm_camera_sensor_platform_info mt9p017_sensor_info = {
	.mount_angle = 90
};

static struct msm_camera_sensor_flash_data flash_mt9p017 = {
	.flash_type = MSM_CAMERA_FLASH_LED,
	.flash_src  = &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_mt9p017_data = {
	.sensor_name    = "mt9p017",
	.sensor_reset   = 49,
	.sensor_reset_enable = 1,
	.sensor_pwd     = 119,
	.vcm_pwd        = 7,
	.vcm_enable     = 1,
	.pdata          = &msm_camera_device_data_rear,
	.flash_data     = &flash_mt9p017,
	.sensor_platform_info   = &mt9p017_sensor_info,
	.csi_if         = 1,
	.vreg_enable_func = msm_camera_vreg_config,
	.vreg_disable_func = msm_camera_vreg_config,
	.slave_sensor   = 0
};

static struct platform_device msm_camera_sensor_mt9p017 = {
	.name      = "msm_camera_mt9p017",
	.dev       = {
		.platform_data = &msm_camera_sensor_mt9p017_data,
	},
};
#endif

#ifdef CONFIG_HUAWEI_CAMERA_SENSOR_S5K5CA
static struct msm_camera_sensor_platform_info s5k5ca_sensor_info = {
	.mount_angle = 90
};

static struct msm_camera_sensor_flash_data flash_s5k5ca = {
	.flash_type = MSM_CAMERA_FLASH_LED,
	.flash_src  = &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_s5k5ca_data = {
	.sensor_name    = "s5k5ca",
	.sensor_reset   = 49,
	.sensor_reset_enable = 1,
	.sensor_pwd     = 119,
	.vcm_pwd        = 7,
	.vcm_enable     = 1,
	.pdata          = &msm_camera_device_data_rear,
	.flash_data     = &flash_s5k5ca,
	.sensor_platform_info   = &s5k5ca_sensor_info,
	.set_s5k5ca_is_on = set_s5k5ca_is_on,
	.csi_if         = 1,
	.vreg_enable_func = msm_camera_vreg_config,
	.vreg_disable_func = msm_camera_vreg_config,
	.slave_sensor   = 0
};

static struct platform_device msm_camera_sensor_s5k5ca = {
	.name      = "msm_camera_s5k5ca",
	.dev       = {
		.platform_data = &msm_camera_sensor_s5k5ca_data,
	},
};
#endif
#ifdef CONFIG_HUAWEI_CAMERA_SENSOR_MT9T113
static struct msm_camera_sensor_platform_info mt9t113_sensor_info = {
	.mount_angle = 90
};

static struct msm_camera_sensor_flash_data flash_mt9t113 = {
	.flash_type = MSM_CAMERA_FLASH_LED,
	.flash_src  = &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_mt9t113_data = {
	.sensor_name    = "mt9t113",
	.sensor_reset   = 49,
	.sensor_reset_enable = 1,
	.sensor_pwd     = 119,
	.vcm_pwd        = 7,
	.vcm_enable     = 1,
	.pdata          = &msm_camera_device_data_rear,
	.flash_data     = &flash_mt9t113,
	.sensor_platform_info   = &mt9t113_sensor_info,
	/*delete one line*/
	.csi_if         = 1,
	.vreg_enable_func = msm_camera_vreg_config,
	.vreg_disable_func = msm_camera_vreg_config,
	.slave_sensor   = 0
};

static struct platform_device msm_camera_sensor_mt9t113 = {
	.name      = "msm_camera_mt9t113",
	.dev       = {
		.platform_data = &msm_camera_sensor_mt9t113_data,
	},
};
#endif
#ifdef CONFIG_HUAWEI_CAMERA_SENSOR_MT9D113
static struct msm_camera_sensor_platform_info mt9d113_sensor_info = {
	.mount_angle = 90
};

static struct msm_camera_sensor_flash_data flash_mt9d113 = {
	.flash_type = MSM_CAMERA_FLASH_LED,
	.flash_src  = &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_mt9d113_data = {
	.sensor_name    = "mt9d113",
	.sensor_reset   = 49,
	.sensor_reset_enable = 1,
	.sensor_pwd     = 119,
	.vcm_pwd        = 7,
	.vcm_enable     = 1,
	.pdata          = &msm_camera_device_data_rear,
	.flash_data     = &flash_mt9d113,
	.sensor_platform_info   = &mt9d113_sensor_info,
	.csi_if         = 1,
	.vreg_enable_func = msm_camera_vreg_config,
	.vreg_disable_func = msm_camera_vreg_config,
	.slave_sensor   = 0
};

static struct platform_device msm_camera_sensor_mt9d113 = {
	.name      = "msm_camera_mt9d113",
	.dev       = {
		.platform_data = &msm_camera_sensor_mt9d113_data,
	},
};
#endif
#ifdef CONFIG_HUAWEI_CAMERA_SENSOR_MT9V113
static struct msm_camera_sensor_platform_info mt9v113_sensor_info = {
	.mount_angle = 270
};

static struct msm_camera_sensor_flash_data flash_mt9v113 = {
	.flash_type =MSM_CAMERA_FLASH_NONE,
	.flash_src  = &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_mt9v113_data = {
	.sensor_name    = "mt9v113",
	.sensor_reset   = 8,
	.sensor_reset_enable = 1,
	.sensor_pwd     = 120,
	.vcm_pwd        = 7,
	.vcm_enable     = 1,
	.pdata          = &msm_camera_device_data_front,
	.flash_data     = &flash_mt9v113,
	.sensor_platform_info   = &mt9v113_sensor_info,
	.csi_if         = 1,
	.vreg_enable_func = msm_camera_vreg_config,
	.vreg_disable_func = msm_camera_vreg_config,
	.get_s5k5ca_is_on = get_s5k5ca_is_on,
	.slave_sensor   = 1
};

static struct platform_device msm_camera_sensor_mt9v113 = {
	.name      = "msm_camera_mt9v113",
	.dev       = {
		.platform_data = &msm_camera_sensor_mt9v113_data,
	},
};
#endif
/* driver for hw device detect */
#ifdef CONFIG_HUAWEI_HW_DEV_DCT
static struct platform_device huawei_device_detect = {
	.name = "hw-dev-detect",
	.id   =-1,
};
#endif

#ifdef CONFIG_OV5640
static struct msm_camera_sensor_platform_info ov5640_sensor_info = {
	.mount_angle    = 90
};

static struct msm_camera_sensor_flash_src msm_flash_src_ov5640 = {
	.flash_sr_type = MSM_CAMERA_FLASH_SRC_LED,
	._fsrc.led_src.led_name = "flashlight",
	._fsrc.led_src.led_name_len = 10,
};

static struct msm_camera_sensor_flash_data flash_ov5640 = {
	.flash_type     = MSM_CAMERA_FLASH_LED,
	.flash_src      = &msm_flash_src_ov5640,
};

static struct msm_camera_sensor_info msm_camera_sensor_ov5640_data = {
	.sensor_name	    = "ov5640",
	.sensor_reset_enable    = 1,
	.pmic_gpio_enable  = 0,
	.sensor_reset	   = QRD_GPIO_CAM_5MP_RESET,
	.sensor_pwd	     = QRD_GPIO_CAM_5MP_SHDN_EN,
	.vcm_pwd		= 0,
	.vcm_enable	     = 0,
	.pdata			= &msm_camera_device_data_rear,
	.flash_data	     = &flash_ov5640,
	.sensor_platform_info   = &ov5640_sensor_info,
	.csi_if		 = 1,
};

static struct platform_device msm_camera_sensor_ov5640 = {
	.name   = "msm_camera_ov5640",
	.dev    = {
		.platform_data = &msm_camera_sensor_ov5640_data,
	},
};
#endif

#ifdef CONFIG_WEBCAM_OV7692_QRD
static struct msm_camera_sensor_platform_info ov7692_sensor_7627a_info = {
	.mount_angle = 90
};

static struct msm_camera_sensor_flash_data flash_ov7692 = {
	.flash_type     = MSM_CAMERA_FLASH_NONE,
};

static struct msm_camera_sensor_info msm_camera_sensor_ov7692_data = {
	.sensor_name	    = "ov7692",
	.sensor_reset_enable    = 0,
	.pmic_gpio_enable  = 1,
	.sensor_reset	   = GPIO_SKU1_CAM_VGA_RESET_N,
	.sensor_pwd	     = GPIO_SKU1_CAM_VGA_SHDN,
	.vcm_pwd		= 0,
	.vcm_enable	     = 0,
	.pdata			= &msm_camera_device_data_front,
	.flash_data	     = &flash_ov7692,
	.sensor_platform_info   = &ov7692_sensor_7627a_info,
	.csi_if		 = 1,
};

static struct platform_device msm_camera_sensor_ov7692 = {
	.name   = "msm_camera_ov7692",
	.dev    = {
		.platform_data = &msm_camera_sensor_ov7692_data,
	},
};
#endif

static struct i2c_board_info i2c_camera_devices[] = {
	#ifdef CONFIG_S5K4E1
	{
		I2C_BOARD_INFO("s5k4e1", 0x36),
	},
	{
		I2C_BOARD_INFO("s5k4e1_af", 0x8c >> 1),
	},
	#endif
	#ifdef CONFIG_WEBCAM_OV9726
	{
		I2C_BOARD_INFO("ov9726", 0x10),
	},
	#endif
	#ifdef CONFIG_IMX072
	{
		I2C_BOARD_INFO("imx072", 0x34),
	},
	#endif
	#ifdef CONFIG_MT9E013
	{
		I2C_BOARD_INFO("mt9e013", 0x6C >> 2),
	},
	#endif
	{
		I2C_BOARD_INFO("sc628a", 0x6E),
	},
};

static struct i2c_board_info i2c_camera_devices_qrd[] = {
	#ifdef CONFIG_OV5640
	{
		I2C_BOARD_INFO("ov5640", 0x78 >> 1),
	},
	#endif
	#ifdef CONFIG_WEBCAM_OV7692_QRD
	{
		I2C_BOARD_INFO("ov7692", 0x78),
	},
	#endif
};

static struct i2c_board_info i2c_camera_devices_evb[] = {
	#ifdef CONFIG_OV5647
	{
		I2C_BOARD_INFO("ov5647", 0x36 << 1),
	},
	{
		I2C_BOARD_INFO("ov5647_af", 0x18 >> 1),
	},
	#endif
	#ifdef CONFIG_WEBCAM_OV7692_QRD
	{
		I2C_BOARD_INFO("ov7692", 0x78),
	},
	#endif
};

static struct platform_device *camera_devices_msm[] __initdata = {
#ifdef CONFIG_HUAWEI_SENSOR_S5K4E1
	&msm_camera_sensor_s5k4e1,
#endif
#ifdef CONFIG_IMX072
	&msm_camera_sensor_imx072,
#endif
#ifdef CONFIG_WEBCAM_OV9726
	&msm_camera_sensor_ov9726,
#endif
#ifdef CONFIG_MT9E013
	&msm_camera_sensor_mt9e013,
#endif
};

static struct platform_device *camera_devices_qrd[] __initdata = {
#ifdef CONFIG_OV5640
	&msm_camera_sensor_ov5640,
#endif
#ifdef CONFIG_WEBCAM_OV7692_QRD
	&msm_camera_sensor_ov7692,
#endif
};

static struct platform_device *camera_devices_evb[] __initdata = {
#ifdef CONFIG_OV5647
	&msm_camera_sensor_ov5647,
#endif
#ifdef CONFIG_WEBCAM_OV7692_QRD
	&msm_camera_sensor_ov7692,
#endif
};
#endif

enum {
	SX150X_CAM,
};

static struct sx150x_platform_data sx150x_data[] __initdata = {
	[SX150X_CAM]    = {
		.gpio_base	      = GPIO_CAM_EXPANDER_BASE,
		.oscio_is_gpo	   = false,
		.io_pullup_ena	  = 0,
		.io_pulldn_ena	  = 0,
		.io_open_drain_ena      = 0x23,
		.irq_summary	    = -1,
	},
};

static struct i2c_board_info cam_exp_i2c_info[] __initdata = {
	{
		I2C_BOARD_INFO("sx1508q", 0x22),
		.platform_data  = &sx150x_data[SX150X_CAM],
	},
};

static void __init register_i2c_devices(void)
{
	i2c_register_board_info(MSM_GSBI0_QUP_I2C_BUS_ID,
				cam_exp_i2c_info,
				ARRAY_SIZE(cam_exp_i2c_info));
}

/*patch from Qualcomm*/

#ifdef CONFIG_HUAWEI_CAMERA
static void camera_sensor_pwd_config(void)
{
	/*distinguish the camera pwd gpio by lcd_interface_type*/
	if(LCD_IS_RGB == get_hw_lcd_interface_type())
	{
		cam_pwd = 37;
	}
}
#endif
void __init msm7627a_camera_init(void)
{

#ifndef CONFIG_MSM_CAMERA_V4L2
	int rc;
#endif

	pr_debug("msm7627a_camera_init Entered\n");

	if (machine_is_msm7627a_qrd3() || machine_is_msm8625_qrd7()) {
#ifdef CONFIG_OV7692
		ov7692_cam_req_gpio[0].gpio =
			GPIO_SKU7_CAM_VGA_SHDN;
		ov7692_cam_gpio_set_tbl[0].gpio = GPIO_SKU7_CAM_VGA_SHDN;
		ov7692_cam_gpio_set_tbl[1].gpio = GPIO_SKU7_CAM_VGA_SHDN;
#endif

#ifdef CONFIG_OV5647
		msm_camera_sensor_ov5647_data.sensor_pwd =
			GPIO_SKU7_CAM_5MP_SHDN_N;
		msm_camera_sensor_ov5647_data.sensor_reset =
			GPIO_SKU7_CAM_5MP_CAMIF_RESET;
#endif
	}

	/* LCD and camera power (VREG & LDO) init */
	if (machine_is_msm7627a_evb() || machine_is_msm8625_evb()
			|| machine_is_msm8625_evt()
			|| machine_is_msm7627a_qrd3()
			|| machine_is_msm8625_qrd7()) {

		/*patch from Qualcomm*/
#ifdef CONFIG_OV5647
		evb_camera_gpio_cfg();
#endif
	}

#ifndef CONFIG_MSM_CAMERA_V4L2
	if (machine_is_msm7627a_qrd1()) {
		qrd1_camera_gpio_cfg();
		platform_add_devices(camera_devices_qrd,
				ARRAY_SIZE(camera_devices_qrd));
	} else if (machine_is_msm7627a_evb()
			|| machine_is_msm8625_evb()
			|| machine_is_msm8625_evt()
			|| machine_is_msm7627a_qrd3()
			|| machine_is_msm8625_qrd7()) {
		platform_add_devices(camera_devices_evb,
				ARRAY_SIZE(camera_devices_evb));
	} else if (machine_is_msm7627a_qrd3())
		return;
	else
		platform_add_devices(camera_devices_msm,
				ARRAY_SIZE(camera_devices_msm));
#endif
	if (!machine_is_msm7627a_qrd1() || !machine_is_msm7627a_evb()
					|| !machine_is_msm8625_evb()
					|| !machine_is_msm8625_evt()
					|| !machine_is_msm7627a_qrd3()
					|| !machine_is_msm8625_qrd7())
		register_i2c_devices();
#ifndef CONFIG_MSM_CAMERA_V4L2
	rc = regulator_bulk_get(NULL, ARRAY_SIZE(regs_camera), regs_camera);

	if (rc) {
		pr_err("%s: could not get regulators: %d\n", __func__, rc);
		return;
	}

	rc = regulator_bulk_set_voltage(ARRAY_SIZE(regs_camera), regs_camera);

	if (rc) {
		pr_err("%s: could not set voltages: %d\n", __func__, rc);
		return;
	}
#endif

#if defined(CONFIG_MSM_CAMERA_V4L2)
	camera_sensor_pwd_config();
	msm7x27a_init_cam();
#endif
#ifndef CONFIG_MSM_CAMERA_V4L2
	if (machine_is_msm7627a_qrd1()) {
		i2c_register_board_info(MSM_GSBI0_QUP_I2C_BUS_ID,
				i2c_camera_devices_qrd,
				ARRAY_SIZE(i2c_camera_devices_qrd));
	} else if (machine_is_msm7627a_evb()
			|| machine_is_msm8625_evb()
			|| machine_is_msm8625_evt()
			|| machine_is_msm7627a_qrd3()
			|| machine_is_msm8625_qrd7()) {
		pr_debug("machine_is_msm7627a_evb i2c_register_board_info\n");
		i2c_register_board_info(MSM_GSBI0_QUP_I2C_BUS_ID,
				i2c_camera_devices_evb,
				ARRAY_SIZE(i2c_camera_devices_evb));
	} else
#endif
		pr_debug("i2c_register_board_info\n");
		i2c_register_board_info(MSM_GSBI0_QUP_I2C_BUS_ID,
				i2c_camera_devices,
				ARRAY_SIZE(i2c_camera_devices));
}

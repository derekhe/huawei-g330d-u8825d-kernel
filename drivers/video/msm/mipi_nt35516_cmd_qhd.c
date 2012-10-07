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

#include "msm_fb.h"
#include "mipi_dsi.h"
#include "hw_lcd_common.h"

#define LCD_DEVICE_NAME "mipi_cmd_nt35516_qhd_pt"

static lcd_panel_type lcd_panel_qhd = LCD_NONE;
static struct mipi_dsi_phy_ctrl dsi_cmd_mode_phy_db = {
	/* DSI Bit Clock at 419 MHz, 2 lane, RGB888 */
	/* regulator */
	{0x03, 0x01, 0x01, 0x00, 0x00}, 
	/* timing */ 
	{0x79, 0x2E, 0x10, 0x00, 0x3E, 0x45, 0x15, 0x32, 
	0x14, 0x3, 0x04, 0x00}, 
	/* phy ctrl */ 
	{0x7f, 0x00, 0x00, 0x00}, 
	/* strength */ 
	{0xbb, 0x02, 0x06, 0x00}, 
	/* pll control */ 
	{0x01, 0x9e, 0x31, 0xd2, 0x00, 0x40, 0x37, 0x62, 
	0x01, 0x0f, 0x07, 
	0x05, 0x14, 0x03, 0x0, 0x0, 0x0, 0x20, 0x0, 0x02, 0x0}, 
};

static struct dsi_buf nt35516_tx_buf;
static struct sequence * nt35516_lcd_init_table_debug = NULL;

/*LCD init code*/
static const struct sequence nt35516_hvga_standby_enter_table[]= 
{
	/*set the delay time 100ms*/
	/* Fixed LCD Flicker */
	//{0x0021,MIPI_DCS_COMMAND,0},
	{0x00028,MIPI_DCS_COMMAND,0}, //28h
	{0x00010,MIPI_DCS_COMMAND,20},
	{0x00029,MIPI_TYPE_END,150}, // add new command for 
};
/* let the reset go ,so remove this code */
static const struct sequence nt35516_hvga_standby_exit_table[]= 
{
	/*set the delay time 100ms*/
	{0x00011,MIPI_DCS_COMMAND,0}, //29h
	{0x00029,MIPI_DCS_COMMAND,120},
	/* Fixed LCD Flicker */
	//{0x0020,MIPI_DCS_COMMAND,20},
	{0x00029,MIPI_TYPE_END,20}, // add new command for 
};
/*lcd resume function*/
static int mipi_nt35516_lcd_on(struct platform_device *pdev)
{
	
	/*delete some lines */
	boolean para_debug_flag = FALSE;
	uint32 para_num = 0;
	struct msm_fb_data_type *mfd;
	mfd = platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL; 

	/*delete some lines */
	para_debug_flag = lcd_debug_malloc_get_para( "nt35516_lcd_init_table_debug", 
            (void**)&nt35516_lcd_init_table_debug,&para_num);

    if( (TRUE == para_debug_flag) && (NULL != nt35516_lcd_init_table_debug))
    {
        process_mipi_table(mfd,&nt35516_tx_buf,nt35516_lcd_init_table_debug,
		     para_num, lcd_panel_qhd);
    }
	else
	{
		//mipi_set_tx_power_mode(1);
		process_mipi_table(mfd,&nt35516_tx_buf,(struct sequence*)&nt35516_hvga_standby_exit_table,
		 	ARRAY_SIZE(nt35516_hvga_standby_exit_table), lcd_panel_qhd);
		//mipi_set_tx_power_mode(0);
		/*delete some lines */
	}

	if((TRUE == para_debug_flag)&&(NULL != nt35516_lcd_init_table_debug))
	{
		lcd_debug_free_para((void *)nt35516_lcd_init_table_debug);
	}
	
	pr_info("leave mipi_nt35516_lcd_on \n");
	return 0;
}
/*lcd suspend function*/
static int mipi_nt35516_lcd_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	mfd = platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

	process_mipi_table(mfd,&nt35516_tx_buf,(struct sequence*)&nt35516_hvga_standby_enter_table,
		 ARRAY_SIZE(nt35516_hvga_standby_enter_table), lcd_panel_qhd);
	pr_info("leave mipi_nt35516_lcd_off \n");
	return 0;
}

static int __devinit mipi_nt35516_lcd_probe(struct platform_device *pdev)
{
	msm_fb_add_device(pdev);

	return 0;
}

static struct sequence nt35516_cabc_enable_table[] =
{	
	{0x00051,MIPI_DCS_COMMAND,0}, 		
	{0x000ff,TYPE_PARAMETER,0},
	
	{0x00029,MIPI_TYPE_END,0},
};
void nt35516_set_cabc_backlight(struct msm_fb_data_type *mfd,uint32 bl_level)
{	
	nt35516_cabc_enable_table[1].reg = bl_level; // 1 will be changed if modify init code

	process_mipi_table(mfd,&nt35516_tx_buf,(struct sequence*)&nt35516_cabc_enable_table,
		 ARRAY_SIZE(nt35516_cabc_enable_table), lcd_panel_qhd);
}

static struct platform_driver this_driver = {
	.probe  = mipi_nt35516_lcd_probe,
	.driver = {
		.name   = LCD_DEVICE_NAME,
	},
};
static struct msm_fb_panel_data nt35516_panel_data = {
	.on					= mipi_nt35516_lcd_on,
	.off					= mipi_nt35516_lcd_off,
	.set_backlight 		= pwm_set_backlight,
	/*add cabc control backlight*/
	.set_cabc_brightness 	= nt35516_set_cabc_backlight,
};
static struct platform_device this_device = {
	.name   = LCD_DEVICE_NAME,
	.id	= 0,
	.dev	= {
		.platform_data = &nt35516_panel_data,
	}
};

static int mipi_cmd_nt35516_qhd_pt_init(void)
{
	int ret = 0;
	struct msm_panel_info *pinfo = NULL;
	
	lcd_panel_qhd = get_lcd_panel_type();
	if (MIPI_NT35516_TIANMA_QHD != lcd_panel_qhd )		
	{
		return 0;
	}

	pr_info("enter mipi_cmd_nt35516_qhd_pt_init \n");
	mipi_dsi_buf_alloc(&nt35516_tx_buf, DSI_BUF_SIZE);

	ret = platform_driver_register(&this_driver);
	if (!ret)
	{
	 	pinfo = &nt35516_panel_data.panel_info;
		pinfo->xres = 540;
		pinfo->yres = 960;
		pinfo->type = MIPI_CMD_PANEL;
		pinfo->pdest = DISPLAY_1;
		pinfo->wait_cycle = 0;
		pinfo->bpp = 24;		
		pinfo->bl_max = 255;
		pinfo->bl_min = 30;		
		pinfo->fb_num = 3;
		pinfo->clk_rate = 419000000;
		pinfo->lcd.refx100 = 6000; /* adjust refx100 to prevent tearing */

		pinfo->mipi.mode = DSI_CMD_MODE;
		pinfo->mipi.dst_format = DSI_CMD_DST_FORMAT_RGB888;
		pinfo->mipi.vc = 0;
		pinfo->mipi.rgb_swap = DSI_RGB_SWAP_RGB;
		pinfo->mipi.data_lane0 = TRUE;
		pinfo->mipi.data_lane1 = TRUE;
		pinfo->mipi.t_clk_post = 0x7f;
		pinfo->mipi.t_clk_pre = 0x2f;
		pinfo->mipi.stream = 0; /* dma_p */
		pinfo->mipi.mdp_trigger = DSI_CMD_TRIGGER_SW;
		pinfo->mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
		/*set hw te sync*/
		pinfo->lcd.hw_vsync_mode = TRUE;
		pinfo->lcd.vsync_enable = TRUE;
		pinfo->mipi.te_sel = 1; /* TE from vsync gpio */
		pinfo->mipi.interleave_max = 1;
		pinfo->mipi.insert_dcs_cmd = TRUE;
		pinfo->mipi.wr_mem_continue = 0x3c;
		pinfo->mipi.wr_mem_start = 0x2c;
		pinfo->mipi.dsi_phy_db = &dsi_cmd_mode_phy_db;
		pinfo->mipi.tx_eot_append = 0x01;
		pinfo->mipi.rx_eot_ignore = 0;
		pinfo->mipi.dlane_swap = 0x1;

		ret = platform_device_register(&this_device);
		if (ret)
			pr_err("%s: failed to register device!\n", __func__);
	}
	return ret;
}

module_init(mipi_cmd_nt35516_qhd_pt_init);

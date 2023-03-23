/** MIT licence

 Copyright (C) 2019 by Vu Nam https://github.com/vunam https://studiokoda.com

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to do
 so, subject to the following conditions: The above copyright notice and this
 permission notice shall be included in all copies or substantial portions of
 the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO
 EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
 OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.

*/

#include <math.h>
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "driver/i2s_std.h"
#include "ws2812.h"

#include "esp_log.h"
static const char* TAG = "ws2812";

i2s_chan_handle_t tx_handle;
i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM, I2S_ROLE_MASTER);
i2s_std_config_t std_cfg = {
		.clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
		.slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
		.gpio_cfg = {
				.mclk = I2S_GPIO_UNUSED,
				.bclk = I2S_GPIO_UNUSED,
				.ws = I2S_GPIO_UNUSED,
				.dout = I2S_DO_IO,
				.din = I2S_GPIO_UNUSED,
				.invert_flags = {
						.mclk_inv = false,
						.bclk_inv = false,
						.ws_inv = false,
				},
		},
};


static uint16_t out_buffer[LED_NUMBER * PIXEL_SIZE] = {0};
static uint16_t size_buffer;

static const uint16_t bitpatterns[4] = {0x88, 0x8e, 0xe8, 0xee};

void ws2812_init() {
	size_buffer = LED_NUMBER * PIXEL_SIZE;


	chan_cfg.auto_clear = true;
	chan_cfg.dma_frame_num = 1023;
	chan_cfg.dma_desc_num = 10;

	i2s_new_channel(&chan_cfg, &tx_handle, NULL);
	i2s_channel_init_std_mode(tx_handle, &std_cfg);
}


void ws2812_set_color(uint16_t led_index, ws2812_pixel_t color) {
	int loc = led_index * PIXEL_SIZE;
	out_buffer[loc] = bitpatterns[color.green >> 6 & 0x03];
	out_buffer[loc + 1] = bitpatterns[color.green >> 4 & 0x03];
	out_buffer[loc + 2] = bitpatterns[color.green >> 2 & 0x03];
	out_buffer[loc + 3] = bitpatterns[color.green & 0x03];

	out_buffer[loc + 4] = bitpatterns[color.red >> 6 & 0x03];
	out_buffer[loc + 5] = bitpatterns[color.red >> 4 & 0x03];
	out_buffer[loc + 6] = bitpatterns[color.red >> 2 & 0x03];
	out_buffer[loc + 7] = bitpatterns[color.red & 0x03];

	out_buffer[loc + 8] = bitpatterns[color.blue >> 6 & 0x03];
	out_buffer[loc + 9] = bitpatterns[color.blue >> 4 & 0x03];
	out_buffer[loc + 10] = bitpatterns[color.blue >> 2 & 0x03];
	out_buffer[loc + 11] = bitpatterns[color.blue & 0x03];	
}

void ws2812_write() {
	i2s_channel_enable(tx_handle);
	size_t bytes_written = 0;
	i2s_channel_write(tx_handle, out_buffer, size_buffer, &bytes_written, portMAX_DELAY);
	
	i2s_channel_disable(tx_handle);
}

void ws2812_update(ws2812_pixel_t *pixels) {
	size_t bytes_written = 0;

	for (uint16_t i = 0; i < LED_NUMBER; i++) {
		int loc = i * PIXEL_SIZE;

		out_buffer[loc] = bitpatterns[pixels[i].green >> 6 & 0x03];
		out_buffer[loc + 1] = bitpatterns[pixels[i].green >> 4 & 0x03];
		out_buffer[loc + 2] = bitpatterns[pixels[i].green >> 2 & 0x03];
		out_buffer[loc + 3] = bitpatterns[pixels[i].green & 0x03];

		out_buffer[loc + 4] = bitpatterns[pixels[i].red >> 6 & 0x03];
		out_buffer[loc + 5] = bitpatterns[pixels[i].red >> 4 & 0x03];
		out_buffer[loc + 6] = bitpatterns[pixels[i].red >> 2 & 0x03];
		out_buffer[loc + 7] = bitpatterns[pixels[i].red & 0x03];

		out_buffer[loc + 8] = bitpatterns[pixels[i].blue >> 6 & 0x03];
		out_buffer[loc + 9] = bitpatterns[pixels[i].blue >> 4 & 0x03];
		out_buffer[loc + 10] = bitpatterns[pixels[i].blue >> 2 & 0x03];
		out_buffer[loc + 11] = bitpatterns[pixels[i].blue & 0x03];
	}
	i2s_channel_write(tx_handle, out_buffer, size_buffer, &bytes_written, portMAX_DELAY);
}

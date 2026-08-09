/*
	Copyright (c) 2023 ByteBit/xtreme8000

	This file is part of CavEX.

	CavEX is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	CavEX is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with CavEX.  If not, see <http://www.gnu.org/licenses/>.
*/



#include "../../platform/gfx.h"
#include "../../platform/input.h"
#include "../../stack.h"
#include "../../util.h"
#include "../game_state.h"
#include "screen.h"
#include "../../graphics/gui_util.h"


#include <assert.h>
#include <dirent.h>
#include <m-lib/m-string.h>
#include <string.h>
#include <time.h>

#define GUI_WIDTH 87
#define GUI_HEIGHT 106

static struct stack* play_button = NULL;
static struct stack* settings_button = NULL;
static struct stack* exit_button = NULL;

typedef struct {
    int x, y, width, height;
    char label[32];
    void (*on_click)(); // A function pointer for when it's pressed
} Button;


static size_t gui_selection;
static int scroll_offset;

static int top_visible;
static int bottom_visible;
static int height_visible;
static int entry_height = 72;
static int side_padding = 4;

extern ini_t *configfile;


static void screen_main_reset(struct screen* s, int width, int height) {
	input_pointer_enable(true);

	if(gstate.local_player)
		gstate.local_player->data.local_player.capture_input = false;


	gui_selection = 0;
	scroll_offset = side_padding;
	top_visible = height * 0.133F;
	bottom_visible = height - 64;
	height_visible = bottom_visible - height * 0.133F;

}

static void screen_main_update(struct screen* s, float dt) {
	if(input_pressed(IB_GUI_CLICK)) {
		screen_set(&screen_select_world);
	}

	if(input_pressed(IB_HOME))
		ini_free(configfile);
		gstate.quit = true;
}

static void screen_main_render2D(struct screen* s, int width, int height) {
 	gutil_bg();


 	gutil_text((width - gutil_font_width("CavEX", 20)) / 2,
			   top_visible - 16 * 1.5F, "CavEX", 20, true);


 	gfx_texture(false);


	gfx_texture(true);


	int off_x = (gfx_width() - GUI_WIDTH * 2) / 2;
    int off_y = (gfx_height() - GUI_HEIGHT * 2) / 2;

    gfx_bind_texture(&texture_gui_buttons);
    gutil_texquad(off_x, off_y, 0, 0, GUI_WIDTH, GUI_HEIGHT, GUI_WIDTH * 2,
				  GUI_HEIGHT * 2);

    gutil_text((width - gutil_font_width("Play", 16)) / 2,
			   height / 2 - 75 , "\2478Play", 16, true);
   	gutil_text((width - gutil_font_width("Settings", 16)) / 2,
			   height / 2 - 5, "\2478Settings", 16, true);
   	gutil_text((width - gutil_font_width("Worlds", 16)) / 2,
			   height / 2 + 65, "\2478World", 16, true);







	int icon_offset = 32;
	icon_offset
		+= gutil_control_icon(icon_offset, IB_GUI_UP, "Change selection");
	icon_offset += gutil_control_icon(icon_offset, IB_GUI_CLICK, "Play world");
	icon_offset += gutil_control_icon(icon_offset, IB_HOME, "Quit");

}

struct screen screen_main = {
	.reset = screen_main_reset,
	.update = screen_main_update,
	.render2D = screen_main_render2D,
	.render3D = NULL,
	.render_world = false,
};

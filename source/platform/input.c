/*
	Copyright (c) 2022 ByteBit/xtreme8000

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




#include <assert.h>

#include "../cglm/cglm.h"
#include "gfx.h"
#include "input.h"

#ifdef PLATFORM_PC

#include <GLFW/glfw3.h>

extern GLFWwindow* window;

static bool input_pointer_enabled;
static double input_old_pointer_x, input_old_pointer_y;
static bool input_key_held[1024];

void input_init() {
	for(int k = 0; k < 1024; k++)
		input_key_held[k] = false;

	input_pointer_enabled = false;
	input_old_pointer_x = 0;
	input_old_pointer_y = 0;
}

void input_poll() { }

void input_native_key_status(int key, bool* pressed, bool* released,
							 bool* held) {
	if(key >= 1024) {
		*pressed = false;
		*released = false;
		*held = false;
	}

	int state = key < 1000 ? glfwGetKey(window, key) :
							 glfwGetMouseButton(window, key - 1000);

	*pressed = (state == GLFW_PRESS) && !input_key_held[key];
	*released = (state == GLFW_RELEASE) && input_key_held[key];
	*held = !(*released) && input_key_held[key];

	if(state == GLFW_PRESS)
		input_key_held[key] = true;

	if(state == GLFW_RELEASE)
		input_key_held[key] = false;
}

bool input_native_key_symbol(int key, int* symbol, int* symbol_help,
							 enum input_category* category, int* priority) {
	*category = INPUT_CAT_NONE;
	*symbol = 7;
	*symbol_help = 7;
	*priority = 1;
	return true;
}

bool input_native_key_any(int* key) {
	return false;
}

void input_pointer_enable(bool enable) {
	glfwSetInputMode(window, GLFW_CURSOR,
					 enable ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

	if(!input_pointer_enabled && enable)
		glfwSetCursorPos(window, gfx_width() / 2, gfx_height() / 2);

	if(input_pointer_enabled && !enable)
		glfwGetCursorPos(window, &input_old_pointer_x, &input_old_pointer_y);

	input_pointer_enabled = enable;
}

bool input_pointer(float* x, float* y, float* angle) {
	double x2, y2;
	glfwGetCursorPos(window, &x2, &y2);
	*x = x2;
	*y = y2;
	*angle = 0.0F;
	return input_pointer_enabled && x2 >= 0 && y2 >= 0 && x2 < gfx_width()
		&& y2 < gfx_height();
}

void input_native_joystick(float dt, float* dx, float* dy) {
	if(!input_pointer_enabled) {
		double x2, y2;
		glfwGetCursorPos(window, &x2, &y2);
		*dx = (x2 - input_old_pointer_x) * 0.001F;
		*dy = -(y2 - input_old_pointer_y) * 0.001F;
		input_old_pointer_x = x2;
		input_old_pointer_y = y2;
	} else {
		*dx = 0.0F;
		*dy = 0.0F;
	}
}

#endif

#ifdef PLATFORM_WII


#include "../game/game_state.h"
#include "../ini/ini.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include <wiiuse/wpad.h>
#include <gccore.h>


struct ini_t {
  char *data;
  char *end;
};

static struct {
	float dx, dy;
	float magnitude;
	bool available;
} joystick_input[5];


static bool js_emulated_btns_prev[5][4];
static bool js_emulated_btns_held[5][4];

extern ini_t *configfile;

int i;
int activePad = 0;
u16 keysHeld[4] = {0, 0, 0, 0};


void input_init() {

	WPAD_Init();
	PAD_Init();

	WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
	WPAD_SetVRes(WPAD_CHAN_0, gfx_width(), gfx_height());

	for(int k = 0; k < 4; k++) {
		for(int j = 0; j < 5; j++)
			js_emulated_btns_prev[j][k] = js_emulated_btns_held[j][k] = false;
	}


}


void input_poll() {
	WPAD_ScanPads();
	PAD_ScanPads();

    if(configfile == NULL){
		ini_free(configfile);
		gstate.quit = true;
	}


        for (int i = 0; i < 4; i++) {
            PAD_ControlMotor(i, 0);
            keysHeld[i] = PAD_ButtonsHeld(i);
			if (keysHeld[i] != 0) {
                PAD_ControlMotor(activePad, PAD_MOTOR_STOP);
                activePad = i;
            }
        }


	expansion_t e;
	WPAD_Expansion(WPAD_CHAN_0, &e);

	if(e.type == WPAD_EXP_NUNCHUK) {
		joystick_input[0].dx = sin(glm_rad(e.nunchuk.js.ang));
		joystick_input[0].dy = cos(glm_rad(e.nunchuk.js.ang));
		joystick_input[0].magnitude = e.nunchuk.js.mag;
		joystick_input[0].available = true;
	} else {
		joystick_input[0].available = false;
	}

	if(e.type == WPAD_EXP_CLASSIC) {
		joystick_input[1].dx = sin(glm_rad(e.classic.ljs.ang));
		joystick_input[1].dy = cos(glm_rad(e.classic.ljs.ang));
		joystick_input[1].magnitude = e.classic.ljs.mag;
		joystick_input[1].available = true;

		joystick_input[2].dx = sin(glm_rad(e.classic.rjs.ang));
		joystick_input[2].dy = cos(glm_rad(e.classic.rjs.ang));
		joystick_input[2].magnitude = e.classic.rjs.mag;
		joystick_input[2].available = true;
	} else {
		joystick_input[1].available = joystick_input[2].available = false;
	}

	if(activePad != 0) {
		float angle = atan2(PAD_StickY(activePad), PAD_StickX(activePad));
		float anglesub = atan2(PAD_SubStickY(activePad), PAD_SubStickX(activePad));
		joystick_input[3].dx = sin(angle);
		joystick_input[3].dy = cos(angle);
		joystick_input[3].magnitude = hypotf(PAD_StickX(activePad), PAD_StickY(activePad));
		joystick_input[3].available = true;

		joystick_input[4].dx = sin(anglesub);
		joystick_input[4].dy = cos(anglesub);
		joystick_input[4].magnitude = hypotf(PAD_SubStickX(activePad), PAD_SubStickY(activePad));
		joystick_input[4].available = true;
	} else {
		joystick_input[3].available = joystick_input[4].available = false;
	}

	for(int j = 0; j < 5; j++) {
		for(int k = 0; k < 4; k++) {
			js_emulated_btns_prev[j][k] = js_emulated_btns_held[j][k];
			js_emulated_btns_held[j][k] = false;
		}


		if(joystick_input[j].available) {
			float x = joystick_input[j].dx * joystick_input[j].magnitude;
			float y = joystick_input[j].dy * joystick_input[j].magnitude;

			if(x > 0.2F) {
				js_emulated_btns_held[j][3] = true;
			} else if(x < -0.2F) {
				js_emulated_btns_held[j][2] = true;
			}

			if(y > 0.2F) {
				js_emulated_btns_held[j][0] = true;
			} else if(y < -0.2F) {
				js_emulated_btns_held[j][1] = true;
			}
		}
	}
}





uint32_t input_converter(char *type, char *button){

	int val = 520;

	ini_sget(configfile, type, button, "%d", &val);

	switch(val){
		case 0: return WPAD_BUTTON_UP;
		case 1: return WPAD_BUTTON_DOWN;
		case 2: return WPAD_BUTTON_LEFT;
		case 3: return WPAD_BUTTON_RIGHT;
		case 4: return WPAD_BUTTON_A;
		case 5: return WPAD_BUTTON_B;
		case 6: return WPAD_BUTTON_1;
		case 7: return WPAD_BUTTON_2;
		case 8: return WPAD_BUTTON_PLUS;
		case 9: return WPAD_BUTTON_MINUS;
		case 10: return WPAD_BUTTON_HOME;
		case 11: return WPAD_NUNCHUK_BUTTON_Z;
		case 12: return WPAD_NUNCHUK_BUTTON_C;
		case 100: return WPAD_CLASSIC_BUTTON_UP;
		case 101: return WPAD_CLASSIC_BUTTON_DOWN;
		case 102: return WPAD_CLASSIC_BUTTON_LEFT;
		case 103: return WPAD_CLASSIC_BUTTON_RIGHT;
		case 104: return WPAD_CLASSIC_BUTTON_A;
		case 105: return WPAD_CLASSIC_BUTTON_B;
		case 106: return WPAD_CLASSIC_BUTTON_X;
		case 107: return WPAD_CLASSIC_BUTTON_Y;
		case 108: return WPAD_CLASSIC_BUTTON_ZL;
		case 109: return WPAD_CLASSIC_BUTTON_ZR;
		case 110: return WPAD_CLASSIC_BUTTON_FULL_L;
		case 111: return WPAD_CLASSIC_BUTTON_FULL_R;
		case 112: return WPAD_CLASSIC_BUTTON_PLUS;
		case 113: return WPAD_CLASSIC_BUTTON_MINUS;
		case 114: return WPAD_CLASSIC_BUTTON_HOME;
		case 200: return PAD_BUTTON_LEFT;
		case 201: return PAD_BUTTON_RIGHT;
		case 202: return PAD_BUTTON_DOWN;
		case 203: return PAD_BUTTON_UP;
		case 204: return PAD_TRIGGER_Z;
		case 205: return PAD_TRIGGER_R;
		case 206: return PAD_TRIGGER_L;
		case 207: return PAD_BUTTON_A;
		case 208: return PAD_BUTTON_B;
		case 209: return PAD_BUTTON_X;
		case 210: return PAD_BUTTON_Y;
		case 211: return PAD_BUTTON_MENU;
		case 212: return PAD_BUTTON_START;
		default: break;

	}
	return 0;

}






uint32_t input_wpad_translate(enum input_button key) {
	expansion_t e;
	WPAD_Expansion(WPAD_CHAN_0, &e);


	if(e.type == WPAD_EXP_NUNCHUK) {
		switch(key) {
			case IB_ACTION1:      return input_converter("wiimote", "action1-wiimote");
		    case IB_ACTION2:      return input_converter("wiimote", "action2-wiimote");
		    case IB_FORWARD:      return input_converter("wiimote", "forward-wiimote");
		    case IB_BACKWARD:     return input_converter("wiimote", "backward-wiimote");
		    case IB_LEFT:         return input_converter("wiimote", "left-wiimote");
		    case IB_RIGHT:        return input_converter("wiimote", "right-wiimote");
		    case IB_JUMP:         return input_converter("wiimote", "jump-wiimote");
		    case IB_SNEAK:        return input_converter("wiimote", "sneak-wiimote");
		    case IB_INVENTORY:    return input_converter("wiimote", "inventory-wiimote");
		    case IB_HOME:         return input_converter("wiimote", "home-wiimote");
		    case IB_SCROLL_LEFT:  return input_converter("wiimote", "scroll-left-wiimote");
		    case IB_SCROLL_RIGHT: return input_converter("wiimote", "scroll-right-wiimote");
		    case IB_GUI_UP:       return input_converter("wiimote", "gui-up-wiimote");
		    case IB_GUI_DOWN:     return input_converter("wiimote", "gui-down-wiimote");
		    case IB_GUI_LEFT:     return input_converter("wiimote", "gui-left-wiimote");
		    case IB_GUI_RIGHT:    return input_converter("wiimote", "gui-right-wiimote");
		    case IB_GUI_CLICK:    return input_converter("wiimote", "gui-click-wiimote");
		    case IB_GUI_CLICK_ALT:return input_converter("wiimote", "gui-click-alt-wiimote");
		    case IB_SCREENSHOT:   return input_converter("wiimote", "screenshot-wiimote");
			default: break;

		}
	} else if(e.type == WPAD_EXP_CLASSIC) {
		switch(key) {
			case IB_ACTION1: return input_converter("classic", "action1-classic");
		    case IB_ACTION2: return input_converter("classic", "action2-classic");
		    case IB_FORWARD: return input_converter("classic", "forward-classic");
		    case IB_BACKWARD: return input_converter("classic", "backward-classic");
		    case IB_LEFT: return input_converter("classic", "left-classic");
		    case IB_RIGHT: return input_converter("classic", "right-classic");
		    case IB_JUMP: return input_converter("classic", "jump-classic");
		    case IB_SNEAK: return input_converter("classic", "sneak-classic");
		    case IB_INVENTORY: return input_converter("classic", "inventory-classic");
		    case IB_HOME: return input_converter("classic", "home-classic");
		    case IB_SCROLL_LEFT: return input_converter("classic", "scroll-left-classic");
		    case IB_SCROLL_RIGHT: return input_converter("classic", "scroll-right-classic");
		    case IB_GUI_UP: return input_converter("classic", "gui-up-classic");
		    case IB_GUI_DOWN: return input_converter("classic", "gui-down-classic");
		    case IB_GUI_LEFT: return input_converter("classic", "gui-left-classic");
		    case IB_GUI_RIGHT: return input_converter("classic", "gui-right-classic");
		    case IB_GUI_CLICK: return input_converter("classic", "gui-click-classic");
		    case IB_GUI_CLICK_ALT: return input_converter("classic", "gui-click-alt-classic");
		    case IB_SCREENSHOT: return input_converter("classic", "screenshot-classic");
            default: break;
		}
	}

	if (activePad != 0)  {
		switch(key) {
			case IB_ACTION1: return input_converter("gamecube", "action1-gc");
			case IB_ACTION2: return input_converter("gamecube", "action2-gc");
			case IB_FORWARD: return input_converter("gamecube", "forward-gc");
			case IB_BACKWARD: return input_converter("gamecube", "backward-gc");
			case IB_LEFT: return input_converter("gamecube", "left-gc");
			case IB_RIGHT: return input_converter("gamecube", "right-gc");
			case IB_JUMP: return input_converter("gamecube", "jump-gc");
			case IB_SNEAK: return input_converter("gamecube", "sneak-gc");
			case IB_INVENTORY: return input_converter("gamecube", "inventory-gc");
			case IB_HOME: return input_converter("gamecube", "home-gc");
			case IB_SCROLL_LEFT: return input_converter("gamecube", "scroll-left-gc");
			case IB_SCROLL_RIGHT: return input_converter("gamecube", "scroll-right-gc");
			case IB_GUI_UP: return input_converter("gamecube", "gui-up-gc");
			case IB_GUI_DOWN: return input_converter("gamecube", "gui-down-gc");
			case IB_GUI_LEFT: return input_converter("gamecube", "gui-left-gc");
			case IB_GUI_RIGHT: return input_converter("gamecube", "gui-right-gc");
			case IB_GUI_CLICK: return input_converter("gamecube", "gui-click-gc");
			case IB_GUI_CLICK_ALT: return input_converter("gamecube", "gui-click-alt-gc");
			case IB_SCREENSHOT: return input_converter("gamecube", "screenshot-gc");
			default: break;
		}
	}
	return -1;

}





int input_JS_translate(enum input_button key) {
	expansion_t e;
	WPAD_Expansion(WPAD_CHAN_0, &e);


	if(e.type == WPAD_EXP_NUNCHUK) {
		switch(key) {
			case IB_JS_GUI_UP: return 900;
			case IB_JS_GUI_DOWN: return 901;
			case IB_JS_GUI_LEFT: return 902;
			case IB_JS_GUI_RIGHT: return 903;
			default: break;

		}
	} else if(e.type == WPAD_EXP_CLASSIC) {
		switch(key) {
			case IB_JS_FORWARD: return 910;
			case IB_JS_BACKWARD: return 911;
			case IB_JS_LEFT: return 912;
			case IB_JS_RIGHT: return 913;
			case IB_JS_GUI_UP: return 920;
			case IB_JS_GUI_DOWN: return 921;
			case IB_JS_GUI_LEFT: return 922;
			case IB_JS_GUI_RIGHT: return 923;
            default: break;
		}
	} else if(activePad != 0){
		switch(key) {
			case IB_JS_FORWARD: return 930;
			case IB_JS_BACKWARD: return 931;
			case IB_JS_LEFT: return 932;
			case IB_JS_RIGHT: return 933;
			case IB_JS_GUI_UP: return 940;
			case IB_JS_GUI_DOWN: return 941;
			case IB_JS_GUI_LEFT: return 942;
			case IB_JS_GUI_RIGHT: return 943;
			default: break;
		}
	}
	return 0;
}





void input_native_key_status(enum input_button b, bool* pressed, bool* released, bool* held) {
	expansion_t e;
	WPAD_Expansion(WPAD_CHAN_0, &e);
	int key =  input_JS_translate(b);

	if(key >= 900 && key < 944) {
		int js = (key - 900) / 10;
		int offset = (key - 900) % 10;
		if(offset < 4) {
			*held = js_emulated_btns_held[js][offset]
				&& js_emulated_btns_prev[js][offset];
			*pressed = js_emulated_btns_held[js][offset]
				&& !js_emulated_btns_prev[js][offset];
			*released = !js_emulated_btns_held[js][offset]
				&& js_emulated_btns_prev[js][offset];
			return;
		}
	}

	if(e.type == WPAD_EXP_CLASSIC || e.type == WPAD_EXP_NUNCHUK){
		*pressed = WPAD_ButtonsDown(WPAD_CHAN_0) & input_wpad_translate(b);
		*released = WPAD_ButtonsUp(WPAD_CHAN_0) & input_wpad_translate(b);
		*held = !(*pressed) && !(*released)
			&& WPAD_ButtonsHeld(WPAD_CHAN_0) & input_wpad_translate(b);
	}else if(keysHeld[i] != 0){
		*pressed = PAD_ButtonsDown(activePad) & input_wpad_translate(b);
		*released = PAD_ButtonsUp(activePad) & input_wpad_translate(b);
		*held = !(*pressed) && !(*released)
			&& PAD_ButtonsHeld(activePad) & input_wpad_translate(b);
	}


}




int input_symbol_translate(enum input_button key) {
	expansion_t e;
	WPAD_Expansion(WPAD_CHAN_0, &e);

	int sym = -1;

	if(e.type == WPAD_EXP_NUNCHUK) {
		switch(key) {
			case IB_ACTION1: ini_sget(configfile, "wiimote", "action1-wiimote", "%d", &sym); break;
		    case IB_ACTION2: ini_sget(configfile, "wiimote", "action2-wiimote", "%d", &sym); break;
		    case IB_FORWARD: ini_sget(configfile, "wiimote", "forward-wiimote", "%d", &sym); break;
		    case IB_BACKWARD: ini_sget(configfile, "wiimote", "backward-wiimote", "%d", &sym); break;
		    case IB_LEFT: ini_sget(configfile, "wiimote", "left-wiimote", "%d", &sym); break;
		    case IB_RIGHT: ini_sget(configfile, "wiimote", "right-wiimote", "%d", &sym); break;
		    case IB_JUMP: ini_sget(configfile, "wiimote", "jump-wiimote", "%d", &sym); break;
		    case IB_SNEAK: ini_sget(configfile, "wiimote", "sneak-wiimote", "%d", &sym); break;
		    case IB_INVENTORY: ini_sget(configfile, "wiimote", "inventory-wiimote", "%d", &sym); break;
		    case IB_HOME: ini_sget(configfile, "wiimote", "home-wiimote", "%d", &sym); break;
		    case IB_SCROLL_LEFT: ini_sget(configfile, "wiimote", "scroll-left-wiimote", "%d", &sym); break;
		    case IB_SCROLL_RIGHT: ini_sget(configfile, "wiimote", "scroll-right-wiimote", "%d", &sym); break;
		    case IB_GUI_UP: ini_sget(configfile, "wiimote", "gui-up-wiimote", "%d", &sym); break;
		    case IB_GUI_DOWN: ini_sget(configfile, "wiimote", "gui-down-wiimote", "%d", &sym); break;
		    case IB_GUI_LEFT: ini_sget(configfile, "wiimote", "gui-left-wiimote", "%d", &sym); break;
		    case IB_GUI_RIGHT: ini_sget(configfile, "wiimote",  "gui-right-wiimote", "%d", &sym); break;
		    case IB_GUI_CLICK: ini_sget(configfile, "wiimote", "gui-click-wiimote", "%d", &sym); break;
		    case IB_GUI_CLICK_ALT: ini_sget(configfile, "wiimote", "gui-click-alt-wiimote", "%d", &sym); break;
		    case IB_SCREENSHOT: ini_sget(configfile, "wiimote", "screenshot-wiimote", "%d", &sym); break;
			default: break;
	    }
	}
	if(e.type == WPAD_EXP_CLASSIC) {
		switch(key) {
			case IB_ACTION1: ini_sget(configfile, "classic", "action1-classic", "%d", &sym); break;
		    case IB_ACTION2: ini_sget(configfile, "classic", "action2-classic", "%d", &sym); break;
		    case IB_FORWARD: ini_sget(configfile, "classic", "forward-classic", "%d", &sym); break;
		    case IB_BACKWARD: ini_sget(configfile, "classic", "backward-classic", "%d", &sym); break;
			case IB_LEFT: ini_sget(configfile, "classic", "left-classic", "%d", &sym); break;
			case IB_RIGHT: ini_sget(configfile, "classic", "right-classic", "%d", &sym); break;
			case IB_JUMP: ini_sget(configfile, "classic", "jump-classic", "%d", &sym); break;
			case IB_SNEAK: ini_sget(configfile, "classic", "sneak-classic", "%d", &sym); break;
			case IB_INVENTORY: ini_sget(configfile, "classic", "inventory-classic", "%d", &sym); break;
			case IB_HOME: ini_sget(configfile, "classic", "home-classic", "%d", &sym); break;
			case IB_SCROLL_LEFT: ini_sget(configfile, "classic", "scroll-left-classic", "%d", &sym); break;
			case IB_SCROLL_RIGHT: ini_sget(configfile, "classic", "scroll-right-classic", "%d", &sym); break;
			case IB_GUI_UP: ini_sget(configfile, "classic", "gui-up-classic", "%d", &sym); break;
			case IB_GUI_DOWN: ini_sget(configfile, "classic", "gui-down-classic", "%d", &sym); break;
			case IB_GUI_LEFT: ini_sget(configfile, "classic", "gui-left-classic", "%d", &sym); break;
			case IB_GUI_RIGHT: ini_sget(configfile, "classic",  "gui-right-classic", "%d", &sym); break;
			case IB_GUI_CLICK: ini_sget(configfile, "classic", "gui-click-classic", "%d", &sym); break;
			case IB_GUI_CLICK_ALT: ini_sget(configfile, "classic", "gui-click-alt-classic", "%d", &sym); break;
			case IB_SCREENSHOT: ini_sget(configfile, "classic", "screenshot-classic", "%d", &sym); break;
		default: break;

		}
    }
	else if(keysHeld[i] != 0) {
		switch(key) {
			case IB_ACTION1: ini_sget(configfile, "gamecube", "action1-gc", "%d", &sym); break;
			case IB_ACTION2: ini_sget(configfile, "gamecube", "action2-gc", "%d", &sym); break;
			case IB_FORWARD: ini_sget(configfile, "gamecube", "forward-gc", "%d", &sym); break;
			case IB_BACKWARD: ini_sget(configfile, "gamecube", "backward-gc", "%d", &sym); break;
			case IB_LEFT: ini_sget(configfile, "gamecube", "left-gc", "%d", &sym); break;
			case IB_RIGHT: ini_sget(configfile, "gamecube", "right-gc", "%d", &sym); break;
			case IB_JUMP: ini_sget(configfile, "gamecube", "jump-gc", "%d", &sym); break;
			case IB_SNEAK: ini_sget(configfile, "gamecube", "sneak-gc", "%d", &sym); break;
			case IB_INVENTORY: ini_sget(configfile, "gamecube", "inventory-gc", "%d", &sym); break;
			case IB_HOME: ini_sget(configfile, "gamecube", "home-gc", "%d", &sym); break;
			case IB_SCROLL_LEFT: ini_sget(configfile, "gamecube", "scroll-left-gc", "%d", &sym); break;
			case IB_SCROLL_RIGHT: ini_sget(configfile, "gamecube", "scroll-right-gc", "%d", &sym); break;
			case IB_GUI_UP: ini_sget(configfile, "gamecube", "gui-up-gc", "%d", &sym); break;
			case IB_GUI_DOWN: ini_sget(configfile, "gamecube", "gui-down-gc", "%d", &sym); break;
			case IB_GUI_LEFT: ini_sget(configfile, "gamecube", "gui-left-gc", "%d", &sym); break;
			case IB_GUI_RIGHT: ini_sget(configfile, "gamecube", "gui-right-gc", "%d", &sym); break;
			case IB_GUI_CLICK: ini_sget(configfile, "gamecube", "gui-click-gc", "%d", &sym); break;
			case IB_GUI_CLICK_ALT: ini_sget(configfile, "gamecube", "gui-click-alt-gc", "%d", &sym); break;
			case IB_SCREENSHOT: ini_sget(configfile, "gamecube", "screenshot-gc", "%d", &sym); break;
			default: break;
		}
	}
	return sym;


}


bool input_native_key_symbol(int key, int* symbol, int* symbol_help,
							 enum input_category* category, int* priority) {
	if(key >= 900 && key < 904) {
		*symbol = *symbol_help = 17;
		*category = INPUT_CAT_NUNCHUK;
		*priority = 1;
		return true;
	}

	if(key >= 910 && key < 914) {
		*symbol = *symbol_help = 18;
		*category = INPUT_CAT_CLASSIC_CONTROLLER;
		*priority = 1;
		return true;
	}

	if(key >= 920 && key < 924) {
		*symbol = *symbol_help = 19;
		*category = INPUT_CAT_CLASSIC_CONTROLLER;
		*priority = 1;
		return true;
	}

	if(key < -1 || key > 213)
		return false;

	int symbols[] = {
		[0] = 25,	[1] = 26,	[2] = 27,	[3] = 28,	[4] = 0,	[5] = 1,
		[6] = 2,	[7] = 3,	[8] = 5,	[9] = 6,	[10] = 4,	[11] = 8,
		[12] = 9,	[100] = 25, [101] = 26, [102] = 27, [103] = 28, [104] = 10,
		[105] = 11, [106] = 12, [107] = 13, [108] = 14, [109] = 15, [110] = 22,
		[111] = 23, [112] = 5,	[113] = 6,	[114] = 4,
	};

	*category = INPUT_CAT_NONE;

	if(key >= 0 && key <= 10)
		*category = INPUT_CAT_WIIMOTE;

	if(key >= 11 && key <= 12)
		*category = INPUT_CAT_NUNCHUK;

	if(key >= 100 && key <= 114)
		*category = INPUT_CAT_CLASSIC_CONTROLLER;

	if(key >= 200 && key <= 212)
		*category = INPUT_CAT_GC;

	*symbol = symbols[key];
	*symbol_help = symbols[key];

	if(*symbol_help >= 25 && *symbol_help <= 28)
		*symbol_help = 24;

	expansion_t e;
	WPAD_Expansion(WPAD_CHAN_0, &e);

	if((*category == INPUT_CAT_NUNCHUK && e.type == WPAD_EXP_NUNCHUK)
	   || (*category == INPUT_CAT_CLASSIC_CONTROLLER
		   && e.type == WPAD_EXP_CLASSIC)) {
		*priority = 2;
	} else {
		*priority = 1;
	}

	return true;
}





bool input_native_key_any(int* key) {
	return false;
}





void input_pointer_enable(bool enable) { }

bool input_pointer(float* x, float* y, float* angle) {
	struct ir_t ir;
	WPAD_IR(WPAD_CHAN_0, &ir);
	*x = ir.x;
	*y = ir.y;
	*angle = ir.angle;
	return ir.valid;
}

void input_native_joystick(float dt, float* dx, float* dy) {
	if(joystick_input[0].available && joystick_input[0].magnitude > 0.1F) {
		*dx = joystick_input[0].dx * joystick_input[0].magnitude * dt;
		*dy = joystick_input[0].dy * joystick_input[0].magnitude * dt;
	} else if(joystick_input[2].available
			  && joystick_input[2].magnitude > 0.1F) {
		*dx = joystick_input[2].dx * joystick_input[2].magnitude * dt;
		*dy = joystick_input[2].dy * joystick_input[2].magnitude * dt;
	} else {
		*dx = 0.0F;
		*dy = 0.0F;
	}
}

#endif

#include "../game/game_state.h"



bool input_symbol(enum input_button b, int* symbol, int* symbol_help,
				  enum input_category* category) {

	int translate_key = input_symbol_translate(b);

	if(translate_key == -1)
		return false;

	int priority = 0;
	bool has_any = false;


	int symbol_tmp, symbol_help_tmp, priority_tmp;
	enum input_category category_tmp;
	if(input_native_key_symbol(translate_key, &symbol_tmp, &symbol_help_tmp, &category_tmp, &priority_tmp)
		&& priority_tmp > priority) {
		priority = priority_tmp;
		*symbol = symbol_tmp;
		*symbol_help = symbol_help_tmp;
		*category = category_tmp;
		has_any = true;
		return has_any;
	}else{
		return false;
	}



}

bool input_pressed(enum input_button b) {

	size_t length = 8;


	bool any_pressed = false;
	bool any_held = false;
	bool any_released = false;

	for(size_t k = 0; k < length; k++) {
		bool pressed, released, held;
        input_native_key_status(b, &pressed, &released, &held);

		if(pressed)
			any_pressed = true;
		if(released)
			any_released = true;
		if(held)
			any_held = true;
	}

	return any_pressed && !any_held && !any_released;
}

bool input_released(enum input_button b) {

	size_t length = 8;


	bool any_pressed = false;
	bool any_held = false;
	bool any_released = false;

	for(size_t k = 0; k < length; k++) {
		bool pressed, released, held;
        input_native_key_status(b, &pressed, &released, &held);

		if(pressed)
			any_pressed = true;
		if(released)
			any_released = true;
		if(held)
			any_held = true;
	}

	return !any_pressed && !any_held && any_released;
}

bool input_held(enum input_button b) {

	size_t length = 8;


	bool any_pressed = false;
	bool any_held = false;

	for(size_t k = 0; k < length; k++) {
		bool pressed, released, held;
        input_native_key_status(b, &pressed, &released, &held);

		if(pressed)
			any_pressed = true;
		if(held)
			any_held = true;
	}

	return any_pressed || any_held;
}

bool input_joystick (float dt, float* x, float* y) {
	input_native_joystick(dt, x, y);
	return true;
}

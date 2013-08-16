// Copyright (c) 2013- PPSSPP Project.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2.0 or later versions.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License 2.0 for more details.

// A copy of the GPL 2.0 should have been included with the program.
// If not, see http://www.gnu.org/licenses/

// Official git repository and contact information can be found at
// https://github.com/hrydgard/ppsspp and http://www.ppsspp.org/.

#include "base/logging.h"
#include "i18n/i18n.h"
#include "input/keycodes.h"
#include "input/input_state.h"
#include "ui/ui.h"
#include "ui/ui_context.h"

#include "Core/HLE/sceCtrl.h"
#include "Common/KeyMap.h"
#include "Core/Config.h"
#include "UI/ui_atlas.h"
#include "UI/ControlMappingScreen.h"
#include "UI/UIShader.h"

extern void DrawBackground(float alpha);

void KeyMappingScreen::update(InputState &input) {
	if (input.pad_buttons_down & PAD_BUTTON_BACK) {
		g_Config.Save();
		screenManager()->finishDialog(this, DR_OK);
	}
}

void KeyMappingScreen::render() {
	UIShader_Prepare();
	UIBegin(UIShader_Get());
	DrawBackground(1.0f);

	UIContext *ctx = screenManager()->getUIContext();
	UIFlush();

	I18NCategory *keyI18N = GetI18NCategory("KeyMapping");
	I18NCategory *generalI18N = GetI18NCategory("General");

#define KeyBtn(x, y, symbol) \
	if (UIButton(GEN_ID, Pos(x, y), 90, 0, KeyMap::NameKeyFromPspButton(symbol).c_str(), \
	ALIGN_TOPLEFT)) {\
	screenManager()->push(new KeyMappingNewKeyDialog(symbol), 0); \
	UIReset(); \
	} \
	UIText(0, Pos(x+30, y+50), KeyMap::NameDeviceFromPspButton(symbol).c_str(), 0xFFFFFFFF, 0.7f, ALIGN_HCENTER); \
	UIText(0, Pos(x+30, y+80), KeyMap::GetPspButtonName(symbol).c_str(), 0xFFFFFFFF, 0.5f, ALIGN_HCENTER); \

	int pad = 130;
	int hlfpad = pad / 2;

	int left = 30;
	KeyBtn(left, 30, CTRL_LTRIGGER);

	int top = 120;
	KeyBtn(left+hlfpad, top, CTRL_UP); // Up
	KeyBtn(left, top+hlfpad, CTRL_LEFT);// Left
	KeyBtn(left+pad, top+hlfpad, CTRL_RIGHT); // Right
	KeyBtn(left+hlfpad, top+pad, CTRL_DOWN); // Down

	top = 10;
	left = 250;
	KeyBtn(left+hlfpad, top, VIRTKEY_AXIS_Y_MAX); // Analog Up
	KeyBtn(left, top+hlfpad, VIRTKEY_AXIS_X_MIN);// Analog Left
	KeyBtn(left+pad, top+hlfpad, VIRTKEY_AXIS_X_MAX); // Analog Right
	KeyBtn(left+hlfpad, top+pad, VIRTKEY_AXIS_Y_MIN); // Analog Down

	top = 120;
	left = 480;
	KeyBtn(left+hlfpad, top, CTRL_TRIANGLE); // Triangle
	KeyBtn(left, top+hlfpad, CTRL_SQUARE); // Square
	KeyBtn(left+pad, top+hlfpad, CTRL_CIRCLE); // Circle
	KeyBtn(left+hlfpad, top+pad, CTRL_CROSS); // Cross

	left = 610;
	KeyBtn(left, 30, CTRL_RTRIGGER);

	top += pad + 50;
	left = 250;
	KeyBtn(left, top, CTRL_SELECT); // Select
	KeyBtn(left + pad, top, CTRL_START); //Start

	top = 10;
	left = 720;
	KeyBtn(left, top, VIRTKEY_UNTHROTTLE);
	top += 100;
	KeyBtn(left, top, VIRTKEY_SPEED_TOGGLE);
	top += 100;
	KeyBtn(left, top, VIRTKEY_PAUSE);
	top += 100;
	KeyBtn(left, top, VIRTKEY_RAPID_FIRE);
#undef KeyBtn
	// TODO: Add rapid fire somewhere?

	if (UIButton(GEN_ID, Pos(dp_xres - 10, dp_yres - 10), LARGE_BUTTON_WIDTH, 0, generalI18N->T("Back"), ALIGN_RIGHT | ALIGN_BOTTOM)) {
		screenManager()->finishDialog(this, DR_OK);
	}

	UIEnd();
}

void KeyMappingNewKeyDialog::CreatePopupContents(UI::ViewGroup *parent) {
	using namespace UI;

	I18NCategory *keyI18N = GetI18NCategory("KeyMapping");
	I18NCategory *generalI18N = GetI18NCategory("General");

	std::string pspButtonName = KeyMap::GetPspButtonName(this->pspBtn_);

	parent->Add(new TextView(std::string(keyI18N->T("Map a new key for ")) + pspButtonName));
	
	std::string buttonKey = KeyMap::NameKeyFromPspButton(this->pspBtn_);
	std::string buttonDevice = KeyMap::NameDeviceFromPspButton(this->pspBtn_);
	parent->Add(new TextView(std::string(keyI18N->T("Previous:")) + " " + buttonKey + " - " + buttonDevice));
}

void KeyMappingNewKeyDialog::key(const KeyInput &key) {
	if (key.flags & KEY_DOWN) {
		if (key.keyCode == NKCODE_EXT_MOUSEBUTTON_1) {
			return;
		}
		
		KeyMap::SetKeyMapping(pspBtn_, KeyDef(key.deviceId, key.keyCode), replace_);
		screenManager()->finishDialog(this, DR_OK);
	}
}

void KeyMappingNewKeyDialog::axis(const AxisInput &axis) {
	if (axis.value > AXIS_BIND_THRESHOLD) {
		KeyMap::SetAxisMapping(axis.deviceId, axis.axisId, 1, pspBtn_, replace_);
		screenManager()->finishDialog(this, DR_OK);
	}

	if (axis.value < -AXIS_BIND_THRESHOLD) {
		KeyMap::SetAxisMapping(axis.deviceId, axis.axisId, -1, pspBtn_, replace_);
		screenManager()->finishDialog(this, DR_OK);
	}
}

/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL$
 * $Id$
 *
 */

#ifndef ASYLUM_MENU_H_
#define ASYLUM_MENU_H_

#include "common/events.h"

#include "asylum/asylum.h"
#include "asylum/respack.h"
#include "asylum/scene.h"
#include "asylum/graphics.h"
#include "asylum/text.h"
#include "asylum/cursor.h"

namespace Asylum {

class Text;
class Cursor;

class MainMenu {
public:
	MainMenu(AsylumEngine *vm);
	~MainMenu();

	void handleEvent(Common::Event *event, bool doUpdate);
	bool isActive() {
		return _active;
	}

	void openMenu();
	void closeMenu();

private:
	AsylumEngine *_vm;

	enum EyesAnimation {
		kEyesFront       = 0,
		kEyesLeft        = 1,
		kEyesRight       = 2,
		kEyesTop         = 3,
		kEyesBottom      = 4,
		kEyesTopLeft     = 5,
		kEyesTopRight    = 6,
		kEyesBottomLeft  = 7,
		kEyesBottomRight = 8,
		kEyesCrossed     = 9
	};

	enum MenuScreen {
		kNewGame         = 0,
		kLoadGame        = 1,
		kSaveGame        = 2,
		kDeleteGame      = 3,
		kViewCinematics  = 4,
		kQuitGame        = 5,
		kTextOptions     = 6,
		kAudioOptions    = 7,
		kSettings        = 8,
		kKeyboardConfig  = 9,
		kShowCredits     = 10,
		kReturnToGame    = 11,
		kMainMenu        = 12
	};

	Common::Event *_ev;

	Cursor *_cursor;
	int32  _activeIcon;
	int32  _previousActiveIcon;
	uint32 _curIconFrame;
	uint32 _curMouseCursor;
	int32  _cursorStep;
	uint32 _creditsBgFrame;
	uint32 _creditsTextScroll;
	bool   _leftClick;
	bool   _active;

	// FIXME this shouldnt be here. Need a proper config place
	uint32 _confGammaLevel;
	uint32 _confGameQuality;

	MenuScreen      _activeMenuScreen;
	ResourcePack    *_resPack;
	GraphicResource *_bgResource;
	GraphicResource *_eyeResource;
	GraphicResource *_iconResource;
	GraphicResource *_creditsResource;
	GraphicResource *_creditsFadeResource;

	void updateEyesAnimation();
	void updateMainMenu();

	void updateSubMenu();
	void exitSubMenu();
	void updateSubMenuNewGame();
	void updateSubMenuCinematics();
	void updateSubMenuSettings();
	void updateSubMenuQuitGame();
	void updateSubMenuShowCredits();

	void update();

}; // end of class MainMenu

} // end of namespace Asylum

#endif
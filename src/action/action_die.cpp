//       _________ __                 __
//      /   _____//  |_____________ _/  |______     ____  __ __  ______
//      \_____  \\   __\_  __ \__  \\   __\__  \   / ___\|  |  \/  ___/
//      /        \|  |  |  | \// __ \|  |  / __ \_/ /_/  >  |  /\___ |
//     /_______  /|__|  |__|  (____  /__| (____  /\___  /|____//____  >
//             \/                  \/          \//_____/            \/
//  ______________________                           ______________________
//                        T H E   W A R   B E G I N S
//         Stratagus - A free fantasy real time strategy game engine
//
/**@name action_die.cpp - The die action. */
//
//      (c) Copyright 1998-2005 by Lutz Sammer and Jimmy Salmon
//
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; only version 2 of the License.
//
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
//      02111-1307, USA.
//

//@{

/*----------------------------------------------------------------------------
--  Includes
----------------------------------------------------------------------------*/

#include "stratagus.h"

#include "action/action_die.h"

#include "animation.h"
#include "iolib.h"
#include "unit.h"
#include "unittype.h"

/*----------------------------------------------------------------------------
--  Functions
----------------------------------------------------------------------------*/

/* static */ std::unique_ptr<COrder> COrder::NewActionDie()
{
	return std::make_unique<COrder_Die>();
}

void COrder_Die::Save(CFile &file, const CUnit &unit) const /* override */
{
	file.printf("{\"action-die\"");
	if (this->Finished) {
		file.printf(", \"finished\"");
	}
	file.printf("}");
}

bool COrder_Die::ParseSpecificData(lua_State *l,
                                   int &j,
                                   std::string_view value,
                                   const CUnit &unit) /* override */
{
	return false;
}

bool COrder_Die::IsValid() const /* override */
{
	return true;
}

PixelPos COrder_Die::Show(const CViewport &, const PixelPos &lastScreenPos) const /* override */
{
	return lastScreenPos;
}


static bool AnimateActionDie(CUnit &unit)
{
	const CAnimations *animations = unit.Type->Animations;

	if (animations == nullptr) {
		return false;
	}
	if (animations->Death[unit.DamagedType]) {
		UnitShowAnimation(unit, animations->Death[unit.DamagedType]);
		return true;
	} else if (animations->Death[ANIMATIONS_DEATHTYPES]) {
		UnitShowAnimation(unit, animations->Death[ANIMATIONS_DEATHTYPES]);
		return true;
	}
	return false;
}


void COrder_Die::Execute(CUnit &unit) /* override */
{
	// Show death animation
	if (AnimateActionDie(unit) == false) {
		// some units has no death animation
		unit.Anim.Unbreakable = 0;
	}
	if (unit.Anim.Unbreakable) {
		return ;
	}
	const CUnitType &type = *unit.Type;

	// Die sequence terminated, generate corpse.
	if (type.CorpseType == nullptr) {
		unit.Remove(nullptr);
		unit.Release();
		return ;
	}

	const CUnitType &corpseType = *type.CorpseType;
	// Assert(type.TileWidth >= corpseType.TileWidth && type.TileHeight >= corpseType.TileHeight);

	// Update sight for new corpse
	// We have to unmark BEFORE changing the type.
	// Always do that, since types can have different vision properties.

	unit.Remove(nullptr);
	unit.Type = &corpseType;
	unit.Stats = const_cast<CUnitStats *>(&corpseType.Stats[unit.Player->Index]);
	UpdateUnitSightRange(unit);
	unit.Place(unit.tilePos);

	unit.Frame = 0;
	UnitUpdateHeading(unit);
	AnimateActionDie(unit); // with new corpse.
}

//@}

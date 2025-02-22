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
/**@name animation_exactframe.cpp - The animation ExactFrame. */
//
//      (c) Copyright 2012 by Joris Dauphin
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

#include "animation/animation_exactframe.h"

#include "unit.h"

void CAnimation_ExactFrame::Action(CUnit &unit, int & /*move*/, int /*scale*/) const /* override */
{
	Assert(unit.Anim.Anim == this);
	unit.Frame = ParseAnimInt(&unit);
}

void CAnimation_ExactFrame::Init(std::string_view s, lua_State *) /* override */
{
	this->frame = s;
}

std::optional<int> CAnimation_ExactFrame::GetStillFrame(const CUnitType &type) /* override */
{
	return ParseAnimInt(nullptr) + type.NumDirections / 2;
}

int CAnimation_ExactFrame::ParseAnimInt(const CUnit *unit) const
{
	if (unit == nullptr) {
		return to_number(this->frame);
	} else {
		return ::ParseAnimInt(*unit, this->frame);
	}
}

//@}

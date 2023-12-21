/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore BVBA and others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef MU_ENGRAVING_TIME_MARKER_H
#define MU_ENGRAVING_TIME_MARKER_H

#include "stafftextbase.h"

#include "draw/types/geometry.h"
#include "infrastructure/ld_access.h"

namespace mu::engraving {
//---------------------------------------------------------
//   TimeMarker
//---------------------------------------------------------

/**
   A time marker is a special type of staff text that carries with it the exact
   time location of the text. This means that even if the staves, measures,
   etc. change, the TimeMarker will remain at the same location in time.
 **/
class TimeMarker final : public TextBase
{
    OBJECT_ALLOCATOR(engraving, TimeMarker)
    DECLARE_CLASSOF(ElementType::TIME_MARKER)

public:
        TimeMarker(EngravingItem* destination, Segment* parent = 0, TextStyleType = TextStyleType::STAFF);

    TimeMarker* clone() const override { return new TimeMarker(*this); }

    bool canBeExcludedFromOtherParts() const override { return true; }

    double locationInSeconds() const { return m_timeLocationSeconds; }
  void setTimeLocation(const double& locationInSeconds);
  void needsStaffLocationUpdate();
  void updateStaffLocationIfNeeded() const;

  mu::RectF pageBoundingRect(LD_ACCESS mode = LD_ACCESS::CHECK) const override;

 private:
    PropertyValue propertyDefault(Pid id) const override;

  double m_timeLocationSeconds = 0.0;
  // Mutable because this property is not part of the actual data associated
  // with TimeMarker, it's an internal flag for bookkeeping.
  mutable bool m_needsStaffLocationUpdate = false;
};
} // namespace mu::engraving
#endif

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

#include "timemarker.h"

#include "dom/system.h"
#include "dom/measure.h"
#include "dom/segment.h"
#include "draw/types/pen.h"
#include "draw/types/brush.h"
#include "repeatlist.h"
#include "score.h"

#include "log.h"

using namespace mu;
using mu::draw::Pen;

namespace mu::engraving {
//---------------------------------------------------------
//   timeMarkerStyle
//---------------------------------------------------------

static const ElementStyle timeMarkerStyle {
    { Sid::timeMarkerPlacement, Pid::PLACEMENT },
    { Sid::timeMarkerMinDistance, Pid::MIN_DISTANCE },
};

//---------------------------------------------------------
//   TimeMarker
//---------------------------------------------------------

TimeMarker::TimeMarker(EngravingItem* destination, Segment* parent, TextStyleType tid)
    : TextBase(ElementType::TIME_MARKER, parent, tid, ElementFlag::MOVABLE | ElementFlag::ON_STAFF)
{
  initElementStyle(&timeMarkerStyle);

  const Fraction ticks = destination->tick();
  const int midiTicks = ticks.ticks();
  const double time = destination->score()->repeatList(true).utick2utime(midiTicks);
  m_timeLocationSeconds = time;

  setTextMarkerType(TextMarkerType::VERTICAL_MARKER);
}

void TimeMarker::setTimeLocation(const double& locationInSeconds) {
  m_timeLocationSeconds = locationInSeconds;
}

void TimeMarker::needsStaffLocationUpdate() {
  m_needsStaffLocationUpdate = true;
}

void TimeMarker::updateStaffLocationIfNeeded() const {
  if (!m_needsStaffLocationUpdate) {
    return;
  }

  const int midiTicks = this->score()->repeatList(true).utime2utick(m_timeLocationSeconds);
  const Fraction ticks = Fraction::fromTicks(midiTicks);

  Measure* measure = this->score()->tick2measure(ticks);
  if (!measure) {
    LOGD("MidiTempo::setTempoToScore: no measure for tick %d", midiTicks);
    return;
  }

  Segment* segment = measure->getSegment(SegmentType::ChordRest, ticks);
  if (!segment) {
    LOGD("MidiTempo::setTempoToScore: no chord/rest segment for tempo at %d", midiTicks);
    return;
  }

  // It's not clear that the remove/add operations on Segment actually need to
  // be const, but it seems that to handle general item types they are marked as
  // such. In the end TimeMarkers are annotations so the remove/add operations
  // are really just adding/removing the TimeMarkers from a list, which should
  // not have side-effects (especially since the list doesn't own the memory).
  TimeMarker* mutableThis = const_cast<TimeMarker*>(this);

  // Remove the text from its existing segment
  EngravingObject* parent = explicitParent();
  if (parent == nullptr || !parent->isSegment()) {
    LOGD("parent %s", parent->typeName());
  } else {
    toSegment(parent)->remove(mutableThis);
  }

  segment->add(mutableThis);

  // Only flip off the update bit after we've successfully updated the location.
  m_needsStaffLocationUpdate = false;
}

//---------------------------------------------------------
//   propertyDefault
//---------------------------------------------------------

engraving::PropertyValue TimeMarker::propertyDefault(Pid id) const
{
    switch (id) {
      case Pid::TEXT_STYLE:
        return TextStyleType::TIME_MARKER;
      default:
        return TextBase::propertyDefault(id);
    }
}

RectF TimeMarker::pageBoundingRect(LD_ACCESS mode) const
{
  RectF rect = ldata()->bbox(mode).translated(pagePos());
  EngravingObject* parent = explicitParent();
  if (parent->isSegment() || parent->isMeasure()) {
    Measure* measure = parent->isSegment() ? toSegment(parent)->measure() : toMeasure(parent);
    System* system = measure->system();
    if (system->staves().size() > 0) {
      float minY = system->staff(0)->y();
      float maxY = 0;
      for (const SysStaff* staff : system->staves()) {
        minY = staff->y() < minY ? staff->y() : minY;
        maxY = staff->y() + system->y() + staff->yBottom() > maxY ? staff->y() + system->y() + staff->yBottom() : maxY;
      }

      RectF newRect(PointF(rect.left(), minY), PointF(rect.right(), maxY));
      rect = newRect;
    }
  }

  return rect;
}

}

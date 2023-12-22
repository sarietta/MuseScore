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
#ifndef MU_PLAYBACK_VIDEOPLAYERMODEL_H
#define MU_PLAYBACK_VIDEOPLAYERMODEL_H

#include <QAbstractVideoSurface>
#include <QImage>
#include <QRect>
#include <QVideoFrame>

#include "async/asyncable.h"

#include "modularity/ioc.h"
#include "iplaybackcontroller.h"

#include "actions/iactionsdispatcher.h"

namespace mu::playback {
class VideoPlayerModel : public QObject, public async::Asyncable
{
    Q_OBJECT

    INJECT(IPlaybackController, playbackController)
    INJECT(actions::IActionsDispatcher, dispatcher)

    Q_PROPERTY(bool alwaysOnTop READ alwaysOnTop WRITE setAlwaysOnTop NOTIFY alwaysOnTopChanged)
public:
    explicit VideoPlayerModel(QObject* parent = nullptr);

    Q_INVOKABLE void load();

    Q_INVOKABLE void handleVideoOnPlaying();
    Q_INVOKABLE void handleVideoOnPaused();
    Q_INVOKABLE void rewindMusicPlayback();

    Q_INVOKABLE int currentMusicPositionInMilliseconds() const;

    Q_INVOKABLE void setVideoFrameRate(const QVariant& frameRate);
    Q_INVOKABLE void adjustPlaybackByFrames(const int& amount);

    bool alwaysOnTop() const { return m_alwaysOnTop; }
    void setAlwaysOnTop(const bool& alwaysOnTop);

signals:
    void musicPlaybackStarted();
    void musicPlaybackPaused();
    void musicPlaybackRewind();
    // Will only be emitted if we think a human adjusted the position.
    void musicPlaybackPositionUpdated();

    void alwaysOnTopChanged();
private:
    float m_currentMusicPlaybackPositionSeconds = 0.0;
    float m_videoFrameRate;

    bool m_alwaysOnTop = false;
};
}

#endif // MU_PLAYBACK_VIDEOPLAYERMODEL_H

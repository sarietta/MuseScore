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
#include "videoplayermodel.h"

#include <QVideoSurfaceFormat>
#include <QtMultimedia/QMediaMetaData>
#include <QtMultimedia/QMediaPlayer>

#include "audio/audiotypes.h"
#include "log.h"

using namespace mu::playback;
using namespace mu::framework;

VideoPlayerModel::VideoPlayerModel(QObject* parent)
    : QObject(parent)
{
}

void VideoPlayerModel::load()
{
    playbackController()->isPlayingChanged().onNotify(this, [this]() {
        if (playbackController()->isPlaying()) {
            emit musicPlaybackStarted();
        } else {
            emit musicPlaybackPaused();
        }
    });

    playbackController()->playbackPositionChanged().onNotify(this, [this]() {
        // If the playback position has changed significantly, we assume that was
        // because of human intervention and do a video seek.
        const float newPosition = playbackController()->playbackPositionInSeconds();
        if (fabs(m_currentMusicPlaybackPositionSeconds - newPosition) > 0.05f) {
            // Need to immediately update this so any reactions to the emission get
            // the correct time.
            m_currentMusicPlaybackPositionSeconds = newPosition;
            emit musicPlaybackPositionUpdated();
        }

        m_currentMusicPlaybackPositionSeconds = newPosition;

        if (m_currentMusicPlaybackPositionSeconds == 0.0) {
            emit musicPlaybackRewind();
        }
    });
}

void VideoPlayerModel::handleVideoOnPlaying()
{
    if (!playbackController()->isPlaying()) {
        const int positionMilliseconds = static_cast<int>(m_currentMusicPlaybackPositionSeconds * 1000.0);
        playbackController()->seek(static_cast<audio::msecs_t>(positionMilliseconds));
        dispatcher()->dispatch("play");
    }
}

void VideoPlayerModel::handleVideoOnPaused()
{
    if (playbackController()->isPlaying()) {
        dispatcher()->dispatch("stop");
    }
}

void VideoPlayerModel::rewindMusicPlayback()
{
    playbackController()->seek(static_cast<audio::msecs_t>(0));
}

int VideoPlayerModel::currentMusicPositionInMilliseconds() const
{
    const int positionMilliseconds = static_cast<int>(m_currentMusicPlaybackPositionSeconds * 1000.0);
    return positionMilliseconds;
}

void VideoPlayerModel::adjustPlaybackByFrames(const int& amount)
{
    if (playbackController()->isPlaying()) {
        dispatcher()->dispatch("stop");
    }

    const int currentPositionMilliseconds = static_cast<int>(m_currentMusicPlaybackPositionSeconds * 1000.0);
    const float frameRate = m_videoFrameRate;
    if (frameRate == 0.0) {
        LOGD("Could not determine frame rate");
        return;
    }

    const float secondsOffset = static_cast<float>(amount) / frameRate;
    const int millisecondsOffset = static_cast<int>(1000.0 * secondsOffset);
    playbackController()->seek(static_cast<audio::msecs_t>(
                                   currentPositionMilliseconds + millisecondsOffset));
    m_currentMusicPlaybackPositionSeconds += secondsOffset;
}

void VideoPlayerModel::setVideoFrameRate(const QVariant& frameRate)
{
    LOGD("Found new frame rate: %s", frameRate.toString().toStdString().c_str());
    m_videoFrameRate = frameRate.toFloat();
}

void VideoPlayerModel::setAlwaysOnTop(const bool& alwaysOnTop)
{
    m_alwaysOnTop = alwaysOnTop;
    emit alwaysOnTopChanged();
}

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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "engines/nancy/action/recordtypes.h"
#include "engines/nancy/nancy.h"
#include "engines/nancy/scene.h"
#include "engines/nancy/logic.h"
#include "engines/nancy/nancy.h"
#include "engines/nancy/graphics.h"

namespace Nancy {

void HotspotDesc::readData(Common::SeekableReadStream &stream) {
    frameID = stream.readUint16LE();
    coords.left = stream.readUint32LE();
    coords.top = stream.readUint32LE();
    coords.right = stream.readUint32LE();
    coords.bottom = stream.readUint32LE();
}
uint16 HotMultiframeSceneChange::readData(Common::SeekableReadStream &stream) {
    stream.seek(8, SEEK_CUR);
    int16 size = stream.readSint16LE() * 0x12 + 0xA;
    stream.seek(-10, SEEK_CUR);

    return readRaw(stream, size); // TODO
}

uint16 SceneChange::readData(Common::SeekableReadStream &stream) {
    sceneID = stream.readUint16LE();
    frameID = stream.readUint16LE();
    verticalOffset = stream.readUint16LE();
    doNotStartSound = (bool)(stream.readUint16LE());
    return 8; // TODO
}

void SceneChange::execute(NancyEngine *engine) {
    engine->sceneManager->_sceneID = sceneID;
    engine->playState.queuedViewFrame = frameID;
    engine->playState.queuedMaxVerticalScroll = verticalOffset;
    engine->sceneManager->doNotStartSound = doNotStartSound;
    engine->sceneManager->_state = SceneManager::kLoadNew;
}

uint16 Hot1FrSceneChange::readData(Common::SeekableReadStream &stream) {
    SceneChange::readData(stream);
    hotspotDesc.readData(stream);
    return 0x1A;
}

void Hot1FrSceneChange::execute(NancyEngine *engine) {
    switch (state) {
        case kBegin:
            hotspot = hotspotDesc.coords;
            // fall through
        case kRun:
            if (hotspotDesc.frameID == engine->playState.currentViewFrame) {
                hasHotspot = true;
            } else {
                hasHotspot = false;
            }
            break;
        case kEnd:
            SceneChange::execute(engine);
            break;
    }
}

uint16 HotMultiframeMultisceneChange::readData(Common::SeekableReadStream &stream) {
    stream.seek(0x14, SEEK_CUR);
    uint size = stream.readUint16LE() * 0x12 + 0x16;
    stream.seek(-0x16, SEEK_CUR);

    return readRaw(stream, size); // TODO
}

uint16 Hot1frExitSceneChange::readData(Common::SeekableReadStream &stream) {
    return readRaw(stream, 0x1A); // TODO, could be same struct as Hot1FrSceneChange
}

uint16 StartFrameNextScene::readData(Common::SeekableReadStream &stream) {
    return readRaw(stream, 0x4); // TODO
}

uint16 StartStopPlayerScrolling::readData(Common::SeekableReadStream &stream) {
    type = stream.readByte();    
    return 1;
}

uint16 PlayPrimaryVideoChan0::readData(Common::SeekableReadStream &stream) {
    uint16 bytesRead = stream.pos();
    stream.read(videoData, 0x69C);

    uint16 numResponses = stream.readUint16LE();
    if (numResponses > 0) {
        for (uint i = 0; i < numResponses; ++i) {
            uint16 numConditionFlags = stream.readUint32LE();
            responses.push_back(ResponseStruct());
            ResponseStruct &response = responses[responses.size()-1];

            if (numConditionFlags > 0) {
                for (uint16 j = 0; j < numConditionFlags; ++j) {
                    response.conditionFlags.push_back(ConditionFlags());
                    ConditionFlags &flags = response.conditionFlags[response.conditionFlags.size()-1];
                    stream.read(flags.unknown, 5);
                }
            }

            stream.read(response.unknown, 0x1D8);
        }
    }

    uint16 numSceneBranchStructs = stream.readUint16LE();
    if (numSceneBranchStructs > 0) {
        // TODO
    }

    uint16 numFlagsStructs = stream.readUint16LE();
    if (numFlagsStructs > 0)  {
        for (uint16 i = 0; i < numFlagsStructs; ++i) {
            uint16 numConditionFlags = stream.readUint16LE();
            flagsStructs.push_back(FlagsStruct());
            FlagsStruct &flagsStruct = flagsStructs[flagsStructs.size()-1];

            if (numConditionFlags > 0) {
                // Not sure about this
                if (numConditionFlags > 0) {
                for (uint16 j = 0; j < numConditionFlags; ++j) {
                    flagsStruct.conditionFlags.push_back(ConditionFlags());
                    ConditionFlags &flags = flagsStruct.conditionFlags[flagsStruct.conditionFlags.size()-1];
                    stream.read(flags.unknown, 5);
                }
            }
            }

            flagsStruct.unknown = stream.readUint32LE();
        }
    }

    bytesRead = stream.pos() - bytesRead;
    return bytesRead;
}

uint16 PlaySecondaryVideo::readData(Common::SeekableReadStream &stream) {
    stream.seek(0x33, SEEK_CUR);
    int16 size = stream.readSint16LE() * 0x42 + 0x35;
    stream.seek(-0x35, SEEK_CUR);

    return readRaw(stream, size); // TODO
}

uint16 PlaySecondaryMovie::readData(Common::SeekableReadStream &stream) {
    stream.seek(0xD2, SEEK_CUR);
    uint16 size = stream.readUint16LE() * 0x42 + 0xD4;
    stream.seek(-0xD4, SEEK_CUR);

    return readRaw(stream, size); // TODO
}

uint16 PlayStaticBitmapAnimation::readData(Common::SeekableReadStream &stream) {
    // TODO
    uint16 bytesRead = stream.pos();
    byte *seek = bitmapData;
    
    uint16 currentSize = 0x72;
    stream.read(seek, currentSize);

    seek += currentSize;
    currentSize = (uint16)(bitmapData[0x18]) - (uint16)(bitmapData[0x16]);
    ++currentSize;
    currentSize *= 16;
    stream.read(seek, currentSize);

    seek += 0x252;
    currentSize = (uint16)(bitmapData[0x70]);
    currentSize *= 34;
    stream.read(seek, currentSize);

    bytesRead= stream.pos() - bytesRead;
    return bytesRead;
}

uint16 PlayIntStaticBitmapAnimation::readData(Common::SeekableReadStream &stream) {
    // TODO
    uint16 bytesRead = stream.pos();
    byte *seek = bitmapData;
    
    uint16 currentSize = 0x76;
    stream.read(seek, currentSize);

    seek += currentSize;
    currentSize = (uint16)(bitmapData[0x18]) - (uint16)(bitmapData[0x16]);
    ++currentSize;
    currentSize *= 16;
    stream.read(seek, currentSize);

    seek += 0x256;
    currentSize = (uint16)(bitmapData[0x74]);
    currentSize *= 34;
    stream.read(seek, currentSize);

    bytesRead= stream.pos() - bytesRead;
    return bytesRead;
}

uint16 MapCall::readData(Common::SeekableReadStream &stream) {
    mapData = stream.readByte();
    return 1;
}

uint16 MapCallHot1Fr::readData(Common::SeekableReadStream &stream) {
    return readRaw(stream, 0x12); // TODO
}

uint16 MapCallHotMultiframe::readData(Common::SeekableReadStream &stream) {
    uint16 size = stream.readUint16LE() * 0x12 + 0x2;
    stream.seek(-2, SEEK_CUR);

    return readRaw(stream, size); // TODO
}

uint16 MapLocationAccess::readData(Common::SeekableReadStream &stream) {
    return readRaw(stream, 0x4); // TODO
}

uint16 MapSound::readData(Common::SeekableReadStream &stream) {
    return readRaw(stream, 0x10); // TODO
}

uint16 MapAviOverride::readData(Common::SeekableReadStream &stream) {
    return readRaw(stream, 0x2); // TODO
}

uint16 MapAviOverrideOff::readData(Common::SeekableReadStream &stream) {
    overrideOffData = stream.readByte();
    return 1;
}

uint16 TextBoxWrite::readData(Common::SeekableReadStream &stream) {
    uint16 size = stream.readUint16LE();
    stream.seek(-2, SEEK_CUR);

    if (size > 0x2710) {
        error("Action Record atTextboxWrite has too many text box chars: %d", size);;
    }

    return readRaw(stream, size+2); // TODO
}

uint16 TextBoxClear::readData(Common::SeekableReadStream &stream) {
    clearData = stream.readByte();
    return 1;
}

uint16 BumpPlayerClock::readData(Common::SeekableReadStream &stream) {
    return readRaw(stream, 0x5); // TODO
}

uint16 SaveContinueGame::readData(Common::SeekableReadStream &stream) {
    saveContinueData = stream.readByte();
    return 1;
}

uint16 TurnOffMainRendering::readData(Common::SeekableReadStream &stream) {
    turnOffData = stream.readByte();
    return 1;
}

uint16 TurnOnMainRendering::readData(Common::SeekableReadStream &stream) {
    turnOnData = stream.readByte();
    return 1;
}

uint16 ResetAndStartTimer::readData(Common::SeekableReadStream &stream) {
    stream.skip(1);
    return 1;
}

void ResetAndStartTimer::execute(NancyEngine *engine) {
    engine->playState.timerIsActive = true;
    engine->playState.timerTime = 0;
    isDone = true;
}

uint16 StopTimer::readData(Common::SeekableReadStream &stream) {
    stream.skip(1);
    return 1;
}

void StopTimer::execute(NancyEngine *engine) {
    engine->playState.timerIsActive = false;
    engine->playState.timerTime = 0;
    isDone = true;
}

uint16 EventFlags::readData(Common::SeekableReadStream &stream) {
    for (uint i = 0; i < 10; ++i) {
        descs[i].label = stream.readSint16LE();
        descs[i].flag = (PlayState::Flag)(stream.readUint16LE());
    }
    return 0x28;
}

void EventFlags::execute(NancyEngine *engine) {
    for (uint i = 0; i < 10; ++i) {
        if (descs[i].label != -1) {
            engine->playState.eventFlags[descs[i].label] = descs[i].flag;
        }
    }
    isDone = true;
}

uint16 EventFlagsMultiHS::readData(Common::SeekableReadStream &stream) {
    uint16 returnSize = EventFlags::readData(stream);
    uint16 numHotspots = stream.readUint16LE();
    for (uint16 i = 0; i < numHotspots; ++i) {
        hotspots.push_back(HotspotDesc());
        HotspotDesc &newDesc = hotspots[i];
        newDesc.readData(stream);
    }
    returnSize += numHotspots * 0x12 + 0x2;

    return returnSize;
}

void EventFlagsMultiHS::execute(NancyEngine *engine) {
    switch (state) {
        case kBegin:
            // turn main rendering on
            state = kRun;
            // fall through
        case kRun:
            hasHotspot = false;
            for (uint i = 0; i < hotspots.size(); ++i) {
                if (hotspots[i].frameID == engine->playState.currentViewFrame) {
                    hasHotspot = true;
                    hotspot = hotspots[i].coords;
                }
            }
            break;
        case kEnd:
            hasHotspot = false;
            EventFlags::execute(engine);
            break;
        default:
            break;
    }
}

uint16 OrderingPuzzle::readData(Common::SeekableReadStream &stream) {
    return readRaw(stream, 0x26D); // TODO
}

uint16 LoseGame::readData(Common::SeekableReadStream &stream) {
    loseData = stream.readByte();
    return 1;
}

uint16 PushScene::readData(Common::SeekableReadStream &stream) {
    pushData = stream.readByte();
    return 1;
}

uint16 PopScene::readData(Common::SeekableReadStream &stream) {
    popData = stream.readByte();
    return 1;
}

uint16 WinGame::readData(Common::SeekableReadStream &stream) {
    winData = stream.readByte();
    return 1;
}

uint16 LeverPuzzle::readData(Common::SeekableReadStream &stream) {
    return readRaw(stream, 0x192); // TODO
}

uint16 Telephone::readData(Common::SeekableReadStream &stream) {
    rawData = new byte[0x2016];
    stream.read(rawData, 0x48C);

    int32 sizeNext = (int16)(rawData[0x48A]) * 235;
    stream.read(rawData + 0x48C, sizeNext);
    return sizeNext + 0x48C;
}

uint16 SliderPuzzle::readData(Common::SeekableReadStream &stream) {
    return readRaw(stream, 0x544); // TODO
}

uint16 PasswordPuzzle::readData(Common::SeekableReadStream &stream) {
    return readRaw(stream, 0xD7); // TODO
}

uint16 AddInventoryNoHS::readData(Common::SeekableReadStream &stream) {
    return readRaw(stream, 0x2); // TODO
}

uint16 RemoveInventoryNoHS::readData(Common::SeekableReadStream &stream) {
    return readRaw(stream, 0x2); // TODO
}

uint16 DifficultyLevel::readData(Common::SeekableReadStream &stream) {
    difficulty = stream.readUint16LE();
    flagLabel = stream.readSint16LE();
    flagCondition = stream.readUint16LE();
    return 6;
}

void DifficultyLevel::execute(NancyEngine *engine) {
    engine->playState.difficulty = difficulty;
    if (flagLabel != -1) {
        engine->playState.eventFlags[flagLabel] = (PlayState::Flag)flagCondition;
    }
    isDone = true;
}

uint16 RotatingLockPuzzle::readData(Common::SeekableReadStream &stream) {
    return readRaw(stream, 0x2A4); // TODO
}

uint16 ShowInventoryItem::readData(Common::SeekableReadStream &stream) {
    stream.seek(0xC, SEEK_CUR);
    uint16 size = stream.readUint16LE() * 0x22 + 0xE;
    stream.seek(-0xE, SEEK_CUR);

    return readRaw(stream, size); // TODO
}

uint16 PlayDigiSoundAndDie::readData(Common::SeekableReadStream &stream) {
    return readRaw(stream, 0x2B); // TODO
}

uint16 PlaySoundPanFrameAnchorAndDie::readData(Common::SeekableReadStream &stream) {
    return readRaw(stream, 0x20); // TODO
}

uint16 PlaySoundMultiHS::readData(Common::SeekableReadStream &stream) {
    stream.seek(0x2F, SEEK_CUR);
    uint16 size = stream.readUint16LE() * 0x12 + 0x31;
    stream.seek(-0x31, SEEK_CUR);

    return readRaw(stream, size); // TODO
}

uint16 HintSystem::readData(Common::SeekableReadStream &stream) {
    return readRaw(stream, 0x23); // TODO
}

} // End of namespace Nancy
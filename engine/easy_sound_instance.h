// The MIT License (MIT)
//
// Copyright (c) 2017 - 2019 Huldra
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#ifndef ENGINE_EASY_SOUND_INSTANCE_H_
#define ENGINE_EASY_SOUND_INSTANCE_H_

#include <atomic>
#include <memory>
#include <vector>

#include "engine/arctic_types.h"

namespace arctic {

/// @addtogroup global_advanced
/// @{

enum SoundDataFormat {
  kSoundDataWav,
  kSoundDataVorbis
};

class SoundInstance {
  SoundDataFormat format_;
  std::vector<Ui8> data_;
  std::atomic<Si32> playing_count_;
 public:
  explicit SoundInstance(Ui32 wav_samples);
  explicit SoundInstance(std::vector<Ui8> vorbis_file);
  Si16* GetWavData();
  Ui8* GetVorbisData() const;
  Si32 GetVorbisSize() const;
  SoundDataFormat GetFormat() const;
  Si32 GetDurationSamples();
  bool IsPlaying();
  void IncPlaying();
  void DecPlaying();
};


/// @brief Creates a sound instance from the data
std::shared_ptr<SoundInstance> LoadWav(const Ui8 *data,
    const Si64 size);

/// @}

}  // namespace arctic


#endif  // ENGINE_EASY_SOUND_INSTANCE_H_

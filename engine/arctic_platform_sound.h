// The MIT License (MIT)
//
// Copyright (c) 2016 - 2019 Huldra
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

#ifndef ENGINE_ARCTIC_PLATFORM_SOUND_H_
#define ENGINE_ARCTIC_PLATFORM_SOUND_H_

#include <deque>
#include <string>

#include "engine/easy_sound.h"

namespace arctic {

class SoundPlayerImpl;

class AudioDeviceInfo {
 public:
  std::string system_name = "";
  std::string description_for_human = "";
  bool is_input = false;
  bool is_output = false;

  AudioDeviceInfo(const char *in_system_name,
    const char *in_description_for_human,
    bool in_is_input,
    bool in_is_output)
      : system_name(in_system_name)
      , description_for_human(in_description_for_human)
      , is_input(in_is_input)
      , is_output(in_is_output) {
  }
};

class SoundPlayer {
 public:
  std::deque<AudioDeviceInfo> GetDeviceList();
  void Initialize();
  void Initialize(const char *input_device_system_name,
    const char *output_device_system_name);
  void Deinitialize();
  ~SoundPlayer();
 protected:
  SoundPlayerImpl *impl = nullptr;
};

void StartSoundBuffer(easy::Sound sound, float volume);
void StopSoundBuffer(easy::Sound sound);

void SetMasterVolume(float volume);
float GetMasterVolume();

}  // namespace arctic

#endif  // ENGINE_ARCTIC_PLATFORM_SOUND_H_
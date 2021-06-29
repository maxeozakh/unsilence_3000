# Welcome to the unsilence_3000

This is command line program for a remove some silent moments from video
This is also my first C++ code ever (hope not a last one)

## Todo

- <s>write algorithm</s>
- figure out what tools needed in C++ ecosystem
- divider
- part maker
- video destroyer
- audio destroyer
- connector

## Algorithm

1. **STREAMS_DIVIDER** - devide video file into video and audio files
2. **SILENT_PART_MAKER** - analyse audio file -> find parts with low volume -> write each time stamps of part in the array -> return a "silent parts"
3. **VIDEO_PARTS_DESTROYER** - cut correspond parts from video stream
4. **AUDIO_PARTS_DESTROYER** - cut correspond parts from audio stream
5. **STREAMS_CONNECTOR** - connect video and audio files into one video file, TA-Da!

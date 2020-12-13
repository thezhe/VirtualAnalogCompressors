Zhe Deng 12/3: I plan to upload my code to a Github repo, but below are notes regarding implementation of the ideas on the blog.

# VirtualAnalogCompressors

[Website and Paper](spatialaudio.digitalscholar.rochester.edu/audiosoftwaredesign1fall2020/audiosoftwaredesignvirtualsymposium/musical-va-compressors/)

## SOUL

[Feedforward VCA Compressor - reference design](https://soul.dev/lab/?id=eb8374b6895f0a245cae518d49ad82a5)

[Feedforward TPT VCA](https://soul.dev/lab/?id=e0728f3e5ca6979ef1b22ce17f7af129)

[Feedback VCA]()

TODO:

- Feedforward reference design: implement in C
- Feedforward TPT: Further Testing in SOUL, implement in C
- Feedback VCA: Code in SOUL and C

## C

Switch to [tinywav](https://github.com/mhroth/tinywav) as C framework:

- Need to write processed audio to a file for analysis
- Need to read from file for easy and consistent tests (ie recording from a mic or DAC won't do)
- Real time output is a convenient, but not a necessity  (Port Audio not needed)
- Can also use libsndfile

[Drum Sample for testing](https://freesound.org/s/128886/)

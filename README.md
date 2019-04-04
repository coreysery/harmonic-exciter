# Harmonic Distorter

This is an audio plugin that performs cubic distortion on the current fundamental frequency.

The basic concept is this:
1. Perform Fast Fourier Transform on incoming signal.
2. Grab the frequency w/ the highest amplitude (ie the fundamental frequency).
3. Zero all other frequencies in frequency domain and perform Inverse Fast Fourier Transform to revert back to time domain.
4. Apply cubic distortion, with some introduced randomness, to the signal with only the fundamental frequencies.
5. Mix the distorted signal back in with the original signal.

It's a bit of work in progress. And by that I mean it sounds awful. But I haven't figured out if my implementation is wrong or the whole concept is bad. So until I figure it out I'll keep tweaking.

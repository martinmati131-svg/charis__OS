from pyo import *

# Initialize the audio server
s = Server(audio="jack").boot() # Use JACK for low latency on Linux
s.start()

# --- THE SYNTH VOICE ARCHITECTURE (Defined from Scratch) ---

# 1. Oscillator: The raw sound source (e.g., a morphing sawtooth/square wave)
# We map its frequency and 'duty cycle' (sharpness) to inputs later.
osc = Phazor(freq=110, mul=0.2)
morph = Sine(freq=0.5, mul=0.5, add=0.5) # A slow LFO for internal texture
source = Waveguide(osc, freq=110, dur= morph)

# 2. Filter: A resonant low-pass filter (the classic synth 'squelch')
# We must map the Cutoff and Resonance to vision inputs.
cutoff_freq = Sig(1000) # Placeholder starting value
resonance = Sig(0.7)    # Placeholder high resonance
filt = MoogLadder(source, freq=cutoff_freq, res=resonance)

# 3. Output Output: Define the main output and its volume envelope
# The amplitude is controlled by a noise gate or gesture velocity.
output =filt.out()

# This script is now running, generating a static buzz. 
# We need to inject your hand data into 'cutoff_freq' and 'osc.freq'.

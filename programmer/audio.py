import numpy as np
from scipy.io.wavfile import write

# Define parameters
sample_rate = 44100  # Sample rate (samples per second)
bit_rate = 40  # Bit rate (bits per second)
carrier_frequency = 2500  # Frequency of the carrier signal (in Hz)

# Calculate the number of samples per bit
samples_per_bit = int(sample_rate / bit_rate)

# Generate Manchester encoded data
data = "1100110011001100110011010101010101"  # Example Manchester encoded data
encoded_data = ""
for bit in data:
    encoded_data += bit * samples_per_bit

# Convert encoded data to numpy array
audio_data = np.array(list(encoded_data), dtype=int)

# Modulate Manchester encoded data to audio signal
t = np.arange(len(audio_data)) / sample_rate
carrier_wave = np.sin(2 * np.pi * carrier_frequency * t)
modulated_signal = np.multiply(carrier_wave, audio_data)

# Normalize the signal
modulated_signal /= np.max(np.abs(modulated_signal))

# Write audio to a file
write("manchester_code_audio.wav", sample_rate, modulated_signal.astype(np.float32))

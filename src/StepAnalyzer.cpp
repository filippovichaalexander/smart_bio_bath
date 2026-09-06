#include "StepAnalyzer.h"
#include "Config.h"
#include <Arduino.h>
#include <arduinoFFT.h>

namespace StepAnalyzer {

namespace {
double vReal[FFT_SAMPLES];
double vImag[FFT_SAMPLES];
ArduinoFFT<double> fft(vReal, vImag, FFT_SAMPLES, SAMPLING_FREQ_HZ);
unsigned long lastGlobalStepTime = 0;

void captureSignal() {
  for (int i = 0; i < FFT_SAMPLES; i++) {
    vReal[i] = analogRead(PIN_PIEZO) - 2048;
    vImag[i] = 0.0;
    delayMicroseconds(1000000 / SAMPLING_FREQ_HZ);
  }
}

int detectStepStart(double threshold = 100) {
  for (int i = 1; i < FFT_SAMPLES - 10; i++) {
    if (fabs(vReal[i] - vReal[i - 1]) > threshold) return i;
  }
  return -1;
}
} // namespace

StepParams analyzeStep() {
  StepParams p = {0, 0, 0, 0, 0, 0, 0};

  captureSignal();

  int startIdx = detectStepStart();
  if (startIdx < 0) return p;

  int endIdx = startIdx;
  for (int i = startIdx + 10; i < FFT_SAMPLES - 5; i++) {
    if (fabs(vReal[i]) < 30 && fabs(vReal[i + 1]) < 30) {
      endIdx = i;
      break;
    }
  }
  if (endIdx == startIdx) endIdx = startIdx + 30;

  // 1. Duration
  p.duration = (double)(endIdx - startIdx) / SAMPLING_FREQ_HZ;

  // 2. Peak amplitude (volts)
  double maxVal = 0;
  for (int i = startIdx; i < endIdx; i++) {
    if (fabs(vReal[i]) > maxVal) maxVal = fabs(vReal[i]);
  }
  p.amplitude = maxVal / 4095.0 * 3.3;

  // 3. Steepest rising edge (volts/second)
  double maxSlope = 0;
  for (int i = startIdx + 1; i < endIdx; i++) {
    double slope = fabs(vReal[i] - vReal[i - 1]) * (SAMPLING_FREQ_HZ / 4095.0 * 3.3);
    if (slope > maxSlope) maxSlope = slope;
  }
  p.slope = maxSlope;

  // 4. Micro-vibration count (local extrema within the step)
  int vibCount = 0;
  for (int i = startIdx + 2; i < endIdx - 2; i++) {
    if ((vReal[i] > vReal[i - 1] && vReal[i] > vReal[i + 1]) ||
        (vReal[i] < vReal[i - 1] && vReal[i] < vReal[i + 1])) {
      vibCount++;
    }
  }
  p.microVib = vibCount;

  // 5. Interval since previous step
  unsigned long now = millis();
  p.interval = (lastGlobalStepTime > 0) ? (now - lastGlobalStepTime) / 1000.0 : 0.5;
  lastGlobalStepTime = now;

  // 6. Spectral energy in the 200-400 Hz band
  fft.windowing(FFTWindow::Hamming, FFTDirection::Forward);
  fft.compute(FFTDirection::Forward);
  fft.complexToMagnitude();

  int idx200 = 200 * FFT_SAMPLES / SAMPLING_FREQ_HZ;
  int idx400 = 400 * FFT_SAMPLES / SAMPLING_FREQ_HZ;
  double sumFreq = 0;
  for (int i = idx200; i < idx400 && i < FFT_SAMPLES / 2; i++) sumFreq += vReal[i];
  p.freq200_400 = sumFreq / (idx400 - idx200);

  // 7. Signal/noise ratio at step onset
  int startNoise = (startIdx - 10 < 0) ? 0 : startIdx - 10;
  double noiseSum = 0;
  for (int i = startNoise; i < startIdx; i++) noiseSum += fabs(vReal[i]);
  double noiseAvg = (startIdx > startNoise) ? noiseSum / (startIdx - startNoise) : 0;

  int sigEnd = (startIdx + 10 > endIdx) ? endIdx : startIdx + 10;
  double signalSum = 0;
  for (int i = startIdx; i < sigEnd; i++) signalSum += fabs(vReal[i]);
  double signalAvg = (sigEnd > startIdx) ? signalSum / (sigEnd - startIdx) : 0;

  p.startRatio = (noiseAvg > 0) ? (signalAvg / noiseAvg) : 10.0;

  Serial.printf("Step: dur=%.3f amp=%.2f slope=%.1f vib=%.0f interval=%.2f freq=%.0f snr=%.2f\n",
                p.duration, p.amplitude, p.slope, p.microVib, p.interval, p.freq200_400, p.startRatio);

  return p;
}

} // namespace StepAnalyzer

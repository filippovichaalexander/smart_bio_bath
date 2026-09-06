#pragma once

// Interface to the Physarum polycephalum wetware: a low-voltage pulse on
// PIN_BIO_STIM is the only "training signal" the plasmodium receives (see
// docs/WETWARE.md). There is no digital learning algorithm here — the
// biological tissue itself is the adaptive element; this module only
// delivers the punish/reward pulses at the electrodes.
namespace BioAdapter {

void begin();

// Negative reinforcement: longer pulse, triggered by the "No" button.
void punish();

// Positive reinforcement: short pulse, triggered by a correct recognition.
void reward();

} // namespace BioAdapter

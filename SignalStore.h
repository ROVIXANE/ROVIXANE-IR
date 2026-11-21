#ifndef IHR_SIGNALSTORE_H
#define IHR_SIGNALSTORE_H

#include <Arduino.h>
#include <ArduinoJson.h>

namespace SignalStore {

// Loads /signals.json. On first boot, seeds it from the compiled-in starter
// database (data/devices.json) so ArduinoDroid needs no filesystem upload.
bool begin();

// Access the in-memory database. The reference is stable until reload().
JsonDocument &doc();

// Persists the current in-memory document atomically.
bool save();

// Replaces the database with `json` after normalising it.
bool replace(const String &json);

// Serialises the database to a String for /api/signals and export.
String serialize();

// Removes all profiles (factory reset).
bool clear();

// Returns true when a profile/signal with the given id exists.
bool findSignal(const char *profileId, const char *signalId,
                JsonObject &profile, JsonObject &signal);

}  // namespace SignalStore

#endif

#ifndef IHR_SIGNALSTORE_H
#define IHR_SIGNALSTORE_H

#include <Arduino.h>
#include <ArduinoJson.h>

namespace SignalStore {

// Loads /signals.json, recovering from .bak when needed. On first boot it
// seeds the database from the compiled-in starter devices.
bool begin();

// Access the in-memory database. The reference is stable until reload().
JsonDocument &doc();

// Persists the current in-memory document with the crash-safe writer.
bool save();

// Replaces the database with `json` only after it parses and validates.
// On any failure the previous in-memory database is restored from disk.
bool replace(const String &json);

// Serialises the database to a String (tests / small exports).
String serialize();

// Streaming helpers for /api/signals: avoids a second full-size String copy.
size_t jsonSize();
size_t serializeTo(Print &out);

// Removes all profiles (factory reset).
bool clear();

// Returns true when a profile/signal with the given id exists.
bool findSignal(const char *profileId, const char *signalId,
                JsonObject &profile, JsonObject &signal);
bool findProfile(const char *profileId, JsonObject &profile);

size_t profileCount();
size_t signalCount();

}  // namespace SignalStore

#endif

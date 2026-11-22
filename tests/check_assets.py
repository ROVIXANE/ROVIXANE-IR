#!/usr/bin/env python3
"""Static checks: dashboard <-> firmware API contract and offline assets."""
import importlib.util
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DATA = ROOT / "data"
errors = []


def check(condition, message):
    if not condition:
        errors.append(message)


def read(name):
    return (DATA / name).read_text(encoding="utf-8")


html, js, css = read("index.html"), read("app.js"), read("app.css")
manifest = json.loads(read("manifest.webmanifest"))
ino = (ROOT / "rovixane-ir.ino").read_text(encoding="utf-8")

# 1. WebAssets.h must be exactly what the generator produces from data/.
spec = importlib.util.spec_from_file_location("ihr_asset_generator",
                                              ROOT / "tools/generate_assets.py")
generator = importlib.util.module_from_spec(spec)
spec.loader.exec_module(generator)
expected = generator.build_header()
actual = (ROOT / "WebAssets.h").read_text(encoding="utf-8")
check(actual == expected, "WebAssets.h is out of sync with data/ generator")

# 2. Every DOM id used by app.js must exist in the HTML or be created by JS.
html_ids = set(re.findall(r'\bid="([^"]+)"', html))
js_ids = set(re.findall(r"\$\('([^']+)'\)", js))
js_ids |= set(re.findall(r"getElementById\('([^']+)'\)", js))
js_created_ids = set(re.findall(r'\bid="([^"]+)"', js))
js_created_ids |= set(re.findall(r"(?:fieldInput|fieldSelect)\('([^']+)'", js))
missing = sorted(js_ids - html_ids - js_created_ids)
check(not missing, f"app.js references missing DOM ids: {missing}")

# 3. Every API route used by the dashboard must be registered by the firmware.
used_routes = set(re.findall(r"['\"](\/api\/[A-Za-z0-9_\/-]+)['\"]", js))
ino_routes = set(re.findall(r'gServer\.on\("([^"]+)"', ino))
missing = sorted(used_routes - ino_routes)
check(not missing, f"firmware is missing API routes: {missing}")

# 4. Every translation key used in HTML/JS must exist in both languages.
def dict_keys(lang):
    start = js.index(lang + ": {")
    depth, i = 0, start + len(lang) + 1
    while i < len(js):
        if js[i] == "{":
            depth += 1
        elif js[i] == "}":
            depth -= 1
            if depth == 0:
                return set(re.findall(r"\b([A-Za-z][A-Za-z0-9]*)\s*:", js[start + 2:i]))
        i += 1
    return set()

en, bn = dict_keys("en"), dict_keys("bn")
used_keys = set(re.findall(r'data-i18n="([^"]+)"', html))
used_keys |= set(re.findall(r'data-i18n-(?:aria|title|placeholder)="([^"]+)"', html))
used_keys |= set(re.findall(r"\bt\('([^']+)'\)", js))
for lang, keys in (("en", en), ("bn", bn)):
    gap = sorted(used_keys - keys)
    check(not gap, f"{lang} translation missing: {gap}")
check(en == bn, "Bangla and English dictionaries have different keys")

# 5. PWA manifest branding and wiring.
check(manifest.get("name") == "ROVIXANE IR — Universal Infrared Remote",
      "PWA manifest full product name")
check(manifest.get("short_name") == "ROVIXANE IR", "PWA manifest short name")
check('rel="manifest" href="/manifest.webmanifest"' in html,
      "dashboard links the PWA manifest")
check('gServer.on("/manifest.webmanifest"' in ino,
      "firmware registers the PWA manifest route")

# 6. Starter database schema. "ihr-signals" is a persisted format identifier;
#    it must stay stable so existing signal databases keep working.
starter = json.loads(read("devices.json"))
check(starter.get("format") == "ihr-signals", "starter format field")
check(len(starter.get("profiles", [])) > 0, "starter database must have profiles")
for profile in starter["profiles"]:
    check(bool(profile.get("id")) and bool(profile.get("name")), "profile id/name present")
    check(len(profile.get("signals", [])) > 0, "profile has signals")
    for sig in profile["signals"]:
        check(bool(sig.get("id")) and bool(sig.get("label")), "signal id/label present")
        check(sig.get("kind") in ("protocol", "raw"), "signal kind valid")
        if sig["kind"] == "protocol":
            check(bool(sig.get("protocol")) and bool(sig.get("value")) and
                  isinstance(sig.get("bits"), int), "protocol signal fields valid")
        else:
            check(isinstance(sig.get("timings"), list) and len(sig["timings"]) >= 4,
                  "raw signal timings valid")

# 7. Truly offline: no remote scripts, styles, fonts or images.
for name, text in (("index.html", html), ("app.css", css), ("app.js", js)):
    check(not re.search(r"(?:src|href)\s*=\s*[\"']https?://", text),
          f"{name} loads a remote resource")
    check("@import" not in text and "url(http" not in text, f"{name} has remote CSS")

if errors:
    print("Asset checks FAILED:")
    for e in errors:
        print("  -", e)
    raise SystemExit(1)
print(f"Asset checks passed ({len(used_routes)} routes, "
      f"{len(used_keys)} translation keys, {len(html_ids)} DOM ids)")

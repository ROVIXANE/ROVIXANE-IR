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
js_created_ids |= set(re.findall(r"(?:inputField|selectField)\('([^']+)'", js))
missing = sorted(js_ids - html_ids - js_created_ids)
check(not missing, f"app.js references missing DOM ids: {missing}")

# 3. Every API route used by the dashboard must be registered by the firmware.
used_routes = set(re.findall(r"['\"](\/api\/[A-Za-z0-9_\/-]+)['\"]", js))
ino_routes = set(re.findall(r'gServer\.on\("([^"]+)"', ino))
missing = sorted(used_routes - ino_routes)
check(not missing, f"firmware is missing API routes: {missing}")

# 4. Translation table is key -> [English, Bangla]. Every key must have both
#    strings, and every key used by the dashboard must exist in the table.
start = js.index("var I18N = {")
end = js.index("\n  };", start)
i18n_body = js[start:end]
pairs = re.findall(r"\b([A-Za-z][A-Za-z0-9]*)\s*:\s*\[([^\]]*)\]", i18n_body)
keys = set()
for key, value in pairs:
    keys.add(key)
    strings = re.findall(r"'(?:[^'\\]|\\.)*'", value)
    check(len(strings) == 2, f"translation {key} must have English and Bangla strings")
used_keys = set(re.findall(r'data-i18n="([^"]+)"', html))
used_keys |= set(re.findall(r'data-i18n-(?:aria|title|placeholder)="([^"]+)"', html))
used_keys |= set(re.findall(r"\bt\('([^']+)'\)", js))
gap = sorted(used_keys - keys)
check(not gap, f"translation missing: {gap}")

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

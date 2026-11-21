# Contributing

Thanks for helping improve IHR Universal IR Remote. Please keep changes small, beginner-friendly and compatible with the existing NodeMCU ESP-12E architecture.

## Ground rules

- Do not commit secrets, tokens, passwords, personal Wi-Fi credentials or private configuration.
- Do not commit generated `.bin`, `.elf`, `.map` or local build directories.
- Preserve offline dashboard behavior; all web assets must remain compiled into `WebAssets.h`.
- Do not claim physical hardware testing unless you actually performed it.

## Development workflow

1. Fork the repository and create a branch from `main`.
2. Make focused changes.
3. Run host checks:

   ```sh
   sh tests/run_tests.sh
   ```

4. If you changed anything in `data/`, regenerate the embedded assets:

   ```sh
   python3 tools/generate_assets.py
   ```

   Then commit the updated `WebAssets.h` too.

5. Compile the real firmware if you changed firmware code:

   ```sh
   arduino-cli core install esp8266:esp8266@3.1.2
   arduino-cli lib install ArduinoJson@7.4.2
   arduino-cli lib install IRremoteESP8266@2.8.6
   arduino-cli compile \
     --fqbn "esp8266:esp8266:nodemcuv2:xtal=80,eesz=4M1M,led=2,ip=lm2f,dbg=Disabled,lvl=None____,wipe=none,baud=115200" \
     --build-path build/firmware .
   ```

6. Open a pull request describing what changed and how you verified it.

## Tests

`tests/run_tests.sh` runs:

- asset consistency checks (routes, translation keys, DOM ids);
- host-side firmware integration checks with Arduino stubs;
- real IRremoteESP8266 2.8.6 header API checks.

These are software checks. GitHub Actions also performs a real ESP8266 compile and publishes artifacts only when compilation succeeds.

## Documentation changes

If you add a feature, update:

- `README.md`
- the relevant file in `docs/`
- `RELEASE_NOTES.md` if it changes user-visible behavior
- `CHANGELOG.md` for a new version

## Releases

Releases are automated from `main`. The release job runs only after host checks and the firmware build both succeed. It verifies the BIN header and uploads a checksum file. Do not manually attach unverified binaries.

## Code style

- Keep C++ readable and explicit; avoid clever macros.
- Keep Bengali and English UI strings together in `data/app.js`.
- Prefer small functions and clear names.
- Add comments only where the reason is not obvious.

## Reporting bugs

Include firmware version, board, phone/browser, wiring summary, steps to reproduce and logs or screenshots. Redact Wi-Fi passwords and tokens.

# Host regression tests

Run the firmware update failure tests from the repository root:

```sh
python3 test/host/run_tests.py
```

The runner uses a native C compiler (`cc`, or the `CC` environment variable)
with AddressSanitizer and UndefinedBehaviorSanitizer. It compiles the production
bootloader source against lightweight mocks of FatFs, flash, CRC and board
interfaces. No third-party Python packages or embedded toolchain are required.

The tests cover a successful update, a short flash write, a failure to open the
application for verification and a programming read error. They also check that
opened files are closed and the SD card is unmounted on failure. Matching data
is supplied by the flash mock even after a short write, so an existing matching
image cannot hide a programming failure.

These tests do not exercise physical flash programming, SD card hardware or
startup/jump code; target firmware builds and hardware validation remain separate.

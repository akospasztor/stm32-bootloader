"""Run firmware update regressions against the production bootloader."""

import os
from pathlib import Path
import shlex
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[2]
HEADERS = (
    "bootloader.h", "board.h", "crc.h", "ff.h", "flash.h", "led.h", "log.h",
    "mcu_hal.h", "sd_diskio.h", "sd_fatfs.h", "sdmmc.h", "systick.h",
)


def main():
    (ROOT / "build").mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(
        prefix="bootloader-host-", dir=ROOT / "build"
    ) as directory:
        build = Path(directory)
        for header in HEADERS:
            (build / header).write_text('#include "mocks.h"\n')
        executable = build / "bootloader-update-test"
        command = shlex.split(os.environ.get("CC", "cc"))
        command += [
            "-std=c11", "-Wall", "-Wextra", "-Werror",
            "-Wno-int-to-pointer-cast", "-fsanitize=address,undefined",
            "-fno-sanitize-recover=all", "-g",
            "-I", str(build), "-I", str(ROOT / "test/host"),
            str(ROOT / "bootloader/source/bootloader.c"),
            str(ROOT / "test/host/bootloader_update_test.c"),
            "-o", str(executable),
        ]
        subprocess.run(command, check=True)
        subprocess.run([str(executable)], check=True)
    print("All four bootloader update regression cases passed.")


if __name__ == "__main__":
    main()

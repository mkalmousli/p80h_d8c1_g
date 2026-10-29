# P80h

Open boot-platform research for the Teclast P80h (MT8163). Goal: boot Alpine
directly through a native C boot manager. The installed system is preserved;
there is no replacement firmware or native Alpine boot yet.

## Feature status

| State | Count | Current evidence |
|---|---:|---|
| Verified | 1 | USB gadget and versioned P80H/1 binary link; boot-region read |
| Implemented | 1 | C boot selection and timeout policy |
| Observed | 16 | Drivers bind or hardware is identified; many functions lack acceptance tests |
| Blocked | 3 | Wi-Fi, Bluetooth, GPU: open driver or firmware path unproven |
| Planned | 9 | DRAM init, CPU frequency, FM, video, cameras, suspend, ROM stage, secure monitor, Alpine boot |

Linux 7.3 tinyconfig builds, but exact MT8163 board support is missing from the
audited tree. A live private eMMC read is underway; it is not a coherent backup
or restore proof. No boot partition has been changed. The silicon mask ROM is
fixed; replacement work can cover writable stages after it. Detailed driver
evidence and acceptance gates are in [`src/device/features.c`](src/device/features.c).

## Build and use

The C host CLI sources are in [`src/host/`](src/host/). `p80h features` prints
the full inventory; `p80h rom-info` queries the MediaTek boot path. The tablet
agent runs temporarily from RAM. See the [roadmap](docs/plan.md).

```console
cc -std=c11 -Wall -Wextra -Werror -pedantic build.c -o build
./build test
./build package
```

Authored code is C, with at most 100 lines per file. Raw dumps, credentials,
calibration and device-specific data stay in ignored `local/`; they are never
included in public artifacts.

# Open problems and evidence needed

These are concrete research gaps, not instructions to substitute proprietary files.
The feature inventory and dependency view provide component-level prerequisites.

## B01 / Physical ROM and entry security

The chip's mask ROM is immutable. A replacement payload must be accepted through
a documented or demonstrated entry path. Current access through an installed
daemon does not prove installation on erased storage. Determine security state,
available ROM transport and the need for an independently implemented download
agent. Falsifier: no FOSS path can execute on the actual hardware configuration.

## B02 / DRAM, power and trusted interfaces

No complete new FOSS DRAM initialization or monitor exists in this project.
Identify DRAM geometry/training, PMIC rail sequencing, SRAM limits, exception
levels and reserved memory. Test RAM-resident payloads before persistent firmware.
Falsifier: memory tests, cold boots or required monitor calls fail without stock stages.

## B03 / Independent recovery

Current critical boot backups match readback; they are not a complete consistent
filesystem backup. The ability to restore after replacing early firmware is not
demonstrated. Prove recovery from outside the stage being tested. Do not carry
the previous project's watchdog assumptions into a new boot chain.

## B04 / Upstream source and controller gap audit

The v7.3 audit commit is pinned, but GitHub clone stalled and archive GET returned
HTTP 429. No new kernel compile is accepted yet. Retry a verified upstream mirror
with bounded fetch time, then inventory MT8163 controllers and binding gaps.
Do not interpret successful tinyconfig as a bootable tablet kernel.

## B05 / Firmware-dependent peripherals

The current radios and touch path may depend on vendor loadable firmware. Source
provenance and a stock-free initialization path must be established for each.
If no free implementation exists, the feature stays blocked; do not hide a blob
inside an array or package. Calibration needs hardware-local retrieval or a free
generation process, never a private donor image as an installation prerequisite.

## B06 / GPU, video and camera interfaces

Historical GPU identification is a lead, not an open-driver contract. Identify
register/command interfaces, memory ownership and actual camera sensors. Software
rendering is not GPU acceptance; a display screenshot is not video/camera support.

## B07 / Battery and system power

The initial reading was about 3.53 V at 0 percent; after charging, P80H/1 reported
about 3.86 V and 49 percent. Neither report is calibrated. The runtime tree declares
`ti,bq24156`, while a historical survey identifies a FAN5405 family part. Confirm
the physical charger/gauge, compare with measured hardware, verify charge limits
and termination, and test suspend/resume before claiming reliable power support.

## B08 / Binary transport migration

The C host and static ARM64 server pass wire-level tests and live USB round trips;
boot-region reads match prior verified copies. The one-in-flight 64 KiB client is
the accepted transfer path. A four-request window stalled USB services and was
reverted. The server runs from RAM and must be relaunched after reboot; the full
live user-area read is incomplete. Keep the legacy daemon for recovery operations.

## B09 / Native menu and loader

Menu policy and memory/header validators pass host tests. Board rendering, touch/
keys, storage access, image trust, DTB fixups and the actual handoff are missing.
Complete each against the ARM64 boot contract and prove recovery after failed boot.

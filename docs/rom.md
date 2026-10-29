# Native C ROM access

`p80h rom-info` is a read-only identity-free hardware/security probe. It does not
reboot the tablet, unlock security, exploit ROM, load a vendor agent, write memory,
write flash, read MEID/SOCID, or prove erased-device installation works.

## Transport contract

- Linux usbfs, no libusb or Python runtime dependency.
- Exactly one 0e8d:0003 or 0e8d:2000 candidate; refuse ambiguous devices.
- Inspect descriptors and claim the zero-altsetting bulk interface.
- Perform volatile CDC setup if supported; do not assume hardcoded endpoints.
- Synchronize A0/0A/50/05 with complemented echoes.
- Query FD for hardware code/version; refuse anything except MT8163.
- Query FC for subcode/hardware/software version and D8 for security flags.
- Validate echoes, lengths and zero status before interpreting responses.
- Release the interface on exit; permission errors remain explicit.

The running Linux gadget is deliberately excluded. A physical ROM/preloader
entry and recovery test is still required. A failed query may leave the temporary
ROM session parked; do not enter it without a known reset/power route.

## Provenance and limits

Protocol facts were checked against the existing FOSS mtkclient reference, notably
mtk_preloader.py and Port.py. This is an independent C usbfs implementation;
no vendor download agent or copied stock implementation is included.
Reference: https://github.com/bkerler/mtkclient (upstream retains its own license).

Synthetic tests verify descriptor parsing, exact query sequence, wrong echoes and
wrong-chip refusal. They are not hardware acceptance. Preloader timing, CDC behavior,
ROM security policy and an independent restore path remain unverified.

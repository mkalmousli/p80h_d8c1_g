# 001 / A fresh remote lab

Recorded: 2026-10-29T18:22:48Z. Source checkpoint: 3568216.

The first question was organizational: could the new project be understood without
inheriting the old tree's assumptions? We created an independent repository and
made the compute boundary explicit: Codespaces and Actions build; the PC owns USB.

The first Codespaces image lacked an SSH server. Adding the supported SSH feature
and recreating the empty workers established two usable remote labs. Strict C
builds, parser tests and protocol tests passed there and in Actions.

This established a development foundation, not working replacement firmware.
The next step was to establish exactly which tablet installation we could reach.

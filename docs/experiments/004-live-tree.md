# 004 / Runtime device-tree inventory

Recorded: 2026-10-29T20:13Z. Device: installed Linux, model MT8163.

## Method and privacy

The C device link read only `/proc/device-tree/model`, the root `compatible`,
and each direct SoC child's `compatible` property. No raw DTB, chosen arguments,
serial number, MAC address, NVMEM, calibration or firmware was copied. Values
below are hardware observations, not clean-room register specifications.

## Root identity and SoC nodes

Root compatible: `teclast,p80h`, `mediatek,mt8163`. The running tree declares
39 SoC children. The table records their node names and compatible strings.
Addresses name register blocks and do not establish that a driver works.

| Runtime node | Compatible |
|---|---|
| aal@1400e000 | mediatek,mt8173-disp-aal |
| adc@11001000 | mediatek,mt8163-auxadc; mediatek,mt8173-auxadc |
| audio-controller@11220000 | mediatek,mt8163-afe |
| ccorr@1400d000 | mediatek,mt8183-disp-ccorr |
| clock-controller@10000000 | mediatek,mt8163-topckgen |
| clock-controller@10001000 | mediatek,mt8163-infracfg; syscon |
| clock-controller@1000c000 | mediatek,mt8163-apmixedsys |
| color@1400c000 | mediatek,mt8167-disp-color |
| connectivity@18070000 | mediatek,mt8163-consys |
| dither@14010000 | mediatek,mt8183-disp-dither |
| dma-controller@11000700 | mediatek,btif_tx |
| dma-controller@11000780 | mediatek,btif_rx |
| dsi-phy@10215000 | mediatek,mt8163-mipi-tx |
| dsi@14012000 | mediatek,mt8173-dsi |
| gamma@1400f000 | mediatek,mt8173-disp-gamma |
| i2c@11007000, @11008000, @11009000 | mediatek,mt8173-i2c |
| interrupt-controller@10200620 | mediatek,mt8173-sysirq; mediatek,mt6577-sysirq |
| interrupt-controller@10220000 | arm,gic-400 |
| keypad@10010000 | mediatek,mt6779-keypad |
| mmc@11230000, @11240000 | mediatek,mt2701-mmc |
| mutex@14015000 | mediatek,mt8163-disp-mutex |
| ovl@14007000 | mediatek,mt8173-disp-ovl |
| pinctrl@1000b000 | mediatek,mt8163-pinctrl |
| power-controller@10006000 | mediatek,mt8163-scpsys |
| pwm@14014000 | mediatek,mt8173-disp-pwm; mediatek,mt6595-disp-pwm |
| pwrap@1000d000 | mediatek,mt8173-pwrap |
| rdma@14009000 | mediatek,mt8173-disp-rdma |
| serial@1100c000 | mediatek,mt8163-btif; mediatek,btif |
| syscon@10005000 | mediatek,mt8163-pctl-a-syscfg; syscon |
| syscon@14000000 | mediatek,mt8163-mmsys; syscon |
| thermal@1100b000 | mediatek,mt8163-thermal |
| timer@10200670 | mediatek,mt8163-cpuxgpt |
| usb-phy@11210800 | mediatek,mt8163-usb-phy |
| usb@11200000 | mediatek,mtk-musb |
| watchdog@10007000 | mediatek,mt6589-wdt |
| wifi@180f0000 | mediatek,mt8163-wifi; mediatek,wifi |

## What this narrows down

The pinned Linux 7.3 source scan found no exact `mediatek,mt8163` compatible
strings under the audited driver and binding paths. Several declared blocks use
other SoC compatible families, and some have shared upstream drivers. That is
an investigation lead only: compatible strings do not prove electrical
compatibility, register equivalence, clock/reset wiring, or stable operation.

The table does not provide the complete board description. Before writing a
public DTS, recover and verify address/size cells, interrupts, clocks, resets,
power domains, pin groups, regulators, chosen boot contract, RAM reservations,
panel timing, touch firmware provenance and storage boot policy. The observed
tree is not a substitute for those checks or for independent recovery.

/* SPDX-License-Identifier: MIT */
#include "features.h"
#include <stdio.h>

/* Evidence is from the installed system; a bound driver is not accepted support. */
static const struct p80h_feature features[] = {
 {"cpu","CPU/SMP","observed","MT8163 Cortex-A53","ARM64","aarch64; four cores","Stress all cores and verify topology."},
 {"dram","DRAM","planned","MT8163 memory","stock early init","Current Linux runs; free init absent","Establish geometry and FOSS initialization."},
 {"clocks","Clocks/pins","observed","MT8163","clk-mt8163 / pinctrl","Installed drivers bind; no exact 7.3 MT8163 support","Audit clocks, pins and power."},
 {"emmc","eMMC","observed","user + boot regions","mtk-msdc","GPT and boot copies verified; raw read ongoing","Finish private read; prove independent restore."},
 {"sd","microSD","observed","second MSDC","mtk-msdc","Controller binds; no card test","Verify read/write and hotplug."},
 {"usb-device","USB gadget","verified","MT8163 MUSB","musb-mtk / phy-mt8163","C binary link and 4 MiB boot read pass","Test sustained read and reconnect."},
 {"usb-host","USB OTG","observed","role connector","usb-conn-gpio / musb-hdrc","Drivers bind; keyboard path unverified","Test keyboard, mouse, storage and role return."},
 {"display","Display","observed","800x1280 panel","mediatek-drm / DSI","Framebuffer and screenshot observed","Verify modes and native framebuffer."},
 {"backlight","Backlight","observed","PWM panel","pwm-backlight","Driver binds","Verify range, sequencing and resume."},
 {"touch","Touch","observed","GSL1680, I2C0:0x40","silead_ts","DT declares silead,gsl1680; free firmware unknown","Establish free firmware, multitouch and rotation."},
 {"keys","Buttons","observed","keypad + PMIC","mt6779-keypad / mtk-pmic-keys","Both drivers bind","Verify every key and wake behavior."},
 {"power","PMIC","observed","MT6323 class","mt6397 / mt6323","PMIC and regulators bind","Audit rails and cold-start sequencing."},
 {"charger","Charger/gauge","observed","I2C 0x6a / PMIC gauge","bq2415x / mt6323","DT says TI BQ24156; survey says FAN5405 family","Identify silicon; validate voltage and termination."},
 {"cpufreq","CPU frequency","planned","MT8163 OPP","unverified","No accepted frequency/voltage path","Establish safe operating points."},
 {"thermal","Thermal","observed","MT8163 controller","mtk-thermal","Driver binds; values unverified","Validate temperatures and throttling."},
 {"wifi","Wi-Fi","blocked","CONSYS radio","mt-wifi / mt8163consys","Existing stack depends on vendor components","Find auditable driver and free firmware."},
 {"bluetooth","Bluetooth","blocked","CONSYS over BTIF","mtk_btif","BTIF binds; free stack unproven","Free firmware, pairing and traffic tests."},
 {"fm","FM radio","planned","CONSYS capability","none","Historical software only","Verify physical hardware first."},
 {"gpu","GPU","blocked","Sapphire Lite lead","no verified open driver","Historical name only","Find public interface and open implementation."},
 {"video","Video codec","planned","MT8163 codec","none verified","No decode/encode test","Identify blocks and free userspace."},
 {"audio","Audio","observed","AFE + MT6323","mt8163-afe / mt6323-codec","Drivers bind; sound not verified","Test playback, capture and suspend."},
 {"camera","Cameras","planned","front/rear sensors","none verified","Sensors not independently identified","Identify buses, sensors and firmware."},
 {"accelerometer","Motion","observed","DA280, I2C2:0x26","da280","DT says miramems,da280","Verify identity, orientation and calibration."},
 {"rtc","RTC","observed","PMIC RTC","mt6397-rtc","Driver binds","Test retention, alarm and wake."},
 {"watchdog","Watchdog/reset","observed","MT8163 WDT","mtk-wdt","Restricted reset returned existing Linux once","Prove recovery independent of installed stages."},
 {"suspend","Suspend","planned","platform PM","none complete","No acceptance evidence","Test repeated suspend/resume."},
 {"rom","ROM/first stage","planned","immutable ROM + payload","no new stage","Mask ROM is silicon code; first stage absent","Implement free writable payload and ROM transport."},
 {"secure","Secure monitor","planned","ARM exception levels","no replacement","Current early stages retained","Specify and replace writable monitor interfaces."},
 {"boot-manager","Boot manager","implemented","portable C policy","src/boot/menu.c","Selection and timeout logic tested","Add board display, storage and direct handoff."},
 {"alpine","Alpine","planned","plain Alpine default","no native install","Current installation preserved","Boot Alpine with independent recovery."}
};

int device_features_cli(void)
{
    for (size_t i = 0; i < sizeof(features) / sizeof(features[0]); i++) {
        const struct p80h_feature *f = &features[i];
        printf("%s [%s] %s — %s\n  evidence: %s\n  next: %s\n",
               f->id, f->state, f->name, f->hardware, f->evidence, f->acceptance);
        printf("  drivers: %s\n", f->driver);
    }
    return 0;
}

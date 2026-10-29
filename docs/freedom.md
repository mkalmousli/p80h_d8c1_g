# Stock-free installation contract

The finished installer must work without stock firmware files, stock kernels,
proprietary download agents, vendor userspace, or a pre-existing Android install.
Current use of an already installed daemon is transitional access, not fulfillment
of that contract. No new release is described as stock-free until this is tested.

Every writable boot stage and required loadable firmware component must have a
FOSS source path. This includes first-stage transport, DRAM/power initialization,
secure monitor interfaces, native boot manager and peripheral firmware.

## Physical boundary

The MT8163 mask ROM is fabricated inside the chip. USB access cannot overwrite
it. We can develop our own FOSS first-stage payload and ROM-compatible tooling,
but replacing the physical ROM requires different silicon. This boundary must
remain explicit in diagrams and release claims.

## Acceptance

Start from a device with no useful Android installation and use only public FOSS
release inputs and the hardware's ROM entry path. Install and cold-boot Alpine;
exercise every accepted feature, then demonstrate independent recovery.

Factory identity and calibration are data, not firmware source. Any required
calibration must have a documented hardware-local retrieval or free generation
path; no private donor dump may become a distribution requirement. Radio firmware,
touch firmware and GPU support remain potential blockers until audited.

The project cannot promise a fully free implementation exists for every component.
Unresolved dependencies stay visible rather than being hidden in binary assets.

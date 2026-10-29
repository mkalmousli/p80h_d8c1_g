# Visual identity

Original SVGs only: `mark.svg`, `tablet.svg`, `boot-chain.svg` in `site/assets/`.
Code and artwork are MIT licensed. No icon packs, stock artwork or external fonts.

| Token | Value | Purpose |
|---|---|---|
| background | `#101416` | Calm near-black canvas |
| surface | `#192024` | Raised surfaces |
| text | `#dce5e3` | Main content |
| muted | `#94a5a2` | Secondary content |
| accent | `#83c7a6` | Selection and navigation |
| line | `#2c373b` | Quiet structural strokes |

Use system monospace fonts, generous spacing and precise lines. Avoid glow,
scanlines, decorative animation, noisy textures, and color-only status signals.
Artwork must stay legible at actual tablet resolution. Boot touch targets: at
least 48 pixels; scale with screen density once measured. Preserve key navigation.

The site tablet drawing is a labeled concept, not evidence of running firmware.
Real captures require run ID, date, source revision, behavior verified and privacy
review. Never upload unreviewed desktop captures or raw device logs.

SVG remains the source; remotely generate raster firmware assets only if needed.
The static website needs no JavaScript, bundler, external font, or tracking service.

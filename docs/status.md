# Bring-up record

## 2026-10-29 / 001 / repository foundation

Hypothesis: a C-only host foundation can build independently in Codespaces, with
one structured progress record driving both documentation and the static site.

Falsifier: remote strict builds/tests fail, authored files exceed 100 lines, site
generation diverges from the progress record, or private inputs enter artifacts.

Change: new independent repository; no old source or device images imported.

Verification: pending remote builds, parser/menu tests, source policy and site checks.

Hardware: no connection established; no writes performed. Historical private
backups contain boot/identity regions but no stock `system` image. Exact matching
stock firmware and a fresh complete backup are required before factory reset.

## Required experiment record

Date / run / hypothesis / falsifier / one change / source commit / artifact SHA256 /
device observation / result / rollback. Sanitized results only; raw logs stay local.

# M14 source bundle layout

This folder preserves the module's C++ source, interfaces, numerical contract, tests, HLS build scripts, driver sources, standalone utilities, and a copy of the packaged IP. That IP component.xml is byte-identical to the active repository at vivado/ip_repo/m14_lm_head_v2.

The full original M14 acceptance package included raw board campaign files and assets that are intentionally not copied here. Some scripts under scripts/ and standalone/ need those omitted fixtures or a connected board. Use tools/verify_release.py for this release's file-integrity check; use docs/VALIDATION_SCOPE.md for the consolidated current board evidence.

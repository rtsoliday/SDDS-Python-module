# Agent Notes for SDDS-Python-module

This file contains a short tactical summary based on repository evidence. `../llm-wiki/scripts/refresh_wiki.py` rewrites only the machine-managed block.

<!-- BEGIN MACHINE:summary -->
## Quick start
- Repository-local guidance is sufficient: start with `AGENTS.md`, `README.md`, `docs/`, build/test/config files, and the source tree.
- The sdds Python module provides a high-level interface for creating, manipulating, and reading **Self Describing Data Sets (SDDS)** files, a file protocol designed for storing and transferring scientific data.
- Primary work areas: `Anaconda`, `demo`, `PyPI`, `sdds`.

## Read first
- `README.md`: Primary project overview and workflow notes
- `Anaconda/Makefile`: Build system entry point or dependency manifest
- `PyPI/Makefile`: Build system entry point or dependency manifest
- `sdds/Makefile`: Build system entry point or dependency manifest
- `LICENSE`: Repository configuration that affects local work

## Build and test
- Documented setup/build commands: `python -m pip install soliday.sdds`.
- Detected build systems: GNU Make.
- Unknown: no test workflow evidence was found in the inspected files.
- Likely run commands or operator entry points: `./SDDS-Python-module`.

## Operational warnings
- Local checkout layout appears significant; avoid casual changes to sibling-repo assumptions or relative paths.
- Platform-specific dependency setup matters; do not assume one platform's build recipe carries over unchanged.

## Compatibility constraints
- Cross-platform support exists, but platform-specific dependency setup matters.
- Build and runtime behavior likely depends on neighboring core toolkit checkouts.

## Related knowledge
- Repository-local documentation should be treated as authoritative.
- If a shared `llm-wiki/` directory is present in this workspace or parent folder, consult [the matching repo page](../llm-wiki/repos/SDDS-Python-module.md) for additional architectural context.
- If no shared wiki is present, continue using repository-local evidence only.
- If available, [the SDDS concept page](../llm-wiki/concepts/sdds.md) adds broader cross-repo context.
- If present in this workspace, [the cross-repo map](../llm-wiki/insights/cross-repo-map.md) helps explain related repositories.
<!-- END MACHINE:summary -->

## Human notes
Add durable repo-specific instructions here.

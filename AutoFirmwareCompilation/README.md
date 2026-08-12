# AutoFirmwareCompilation

`AutoFirmwareCompilation` is a small set of tools intended to simplify and speed up the firmware development workflow in `icub-firmware`.

The tool is a **first step toward a more automated firmware build process**, with the long-term goal of moving part of this workflow to CI and automatically generating firmware executables.

## Motivation

A typical firmware development workflow often starts with a change for a single board. However, since several boards share a significant amount of code, a change may require rebuilding multiple firmware targets even when only one board was directly modified.

In addition, every firmware release requires updating the corresponding version number and build date. Doing this manually means opening several project and source files, changing the metadata, compiling each firmware separately, and finally collecting the generated binaries.

These scripts reduce this manual work by allowing developers to:

- update firmware versions and build dates from a single interface;
- update multiple boards in the same session;
- build several firmware targets in sequence;
- collect the generated `.hex` files;
- inspect the compilation logs before publishing the generated binaries.

This is intentionally an initial implementation. Contributions, fixes, improvements, and ideas are very welcome.

## Location

The `AutoFirmwareCompilation` folder is expected to live inside the `icub-firmware` repository:

```text
icub-firmware/
├── AutoFirmwareCompilation/
│   ├── buildAllBoards.bat
│   ├── firmware_versions.yaml
│   └── update_versions.py
├── emBODY/
└── ...
```

The scripts also support the previous layout where `AutoFirmwareCompilation` and `icub-firmware` were sibling directories, but the layout above is the recommended one.

## Requirements

### Python

The version update script requires Python 3 and PyYAML:

```bash
pip install pyyaml
```

### Keil

The firmware build script uses Keil µVision (`UV4.exe`).

The script automatically checks the following common installation locations:

```text
C:\Keil_v5\UV4\UV4.exe
%LOCALAPPDATA%\Keil_v5\UV4\UV4.exe
```

## Updating firmware versions and build dates

Run:

```bash
python update_versions.py
```

The script reads the firmware definitions from `firmware_versions.yaml`, displays the current versions found in the source files, and lets you interactively select one or more boards to update.

For each selected board you can update:

- major version;
- minor version;
- build date;
- build time.

The source files are patched automatically and the corresponding values in `firmware_versions.yaml` are kept in sync.

To update the versions and immediately start the firmware build, use:

```bash
python update_versions.py --build
```

You can also specify a different YAML registry:

```bash
python update_versions.py --yaml path/to/firmware_versions.yaml
```

## Building all supported firmware

Run:

```bat
buildAllBoards.bat
```

The script builds the supported firmware targets sequentially using Keil µVision.

At the moment this includes firmware and required libraries for boards such as:

- EMS;
- MC2PLUS;
- MC4PLUS;
- AMC;
- AMC2C.

Some AMC targets depend on additional libraries such as HAL and IPAL, therefore those libraries are built as part of the process as well.

Compilation output is stored in log files. Please inspect the logs before publishing or distributing the generated firmware binaries.

The generated `.hex` files are copied to the corresponding folders under `icub-firmware-build`.

## Typical workflow

A typical workflow is:

```text
1. Modify the firmware code.
2. Run update_versions.py.
3. Select the firmware targets that need a new version/build date.
4. Update the versions and dates interactively.
5. Launch buildAllBoards.bat, or use update_versions.py --build.
6. Check the compilation logs.
7. Test and publish the generated firmware binaries.
```

This avoids manually opening all the affected source/project files just to update version metadata and rebuild related boards.

## Build errors

If a project path is incorrect or Keil cannot open a project, you may see an error such as:

```text
ERROR: EMS compilation failed with error code 15
```

In this case, check:

- that `icub-firmware` is in the expected location;
- that Keil µVision is installed;
- that the project files referenced by `buildAllBoards.bat` exist;
- the corresponding compilation log for additional details.

## Future work

This tool should be considered a first step toward a more automated firmware generation workflow.

A possible future direction is to move the firmware compilation and artifact generation into CI, reducing the amount of manual work required during firmware development and release preparation.

The current scripts are meant to make the existing workflow faster and less error-prone while providing a practical starting point for that automation.

Contributions, bug fixes, additional board support, and workflow improvements are very welcome.

## Previous version

This tool is an evolution of the original standalone repository:

https://github.com/valegagge/AutoFirmwareCompilation

That repository is expected to be archived/closed and should be considered obsolete once this version is integrated into `icub-firmware`.

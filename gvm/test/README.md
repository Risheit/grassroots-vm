# Grassroots Testing Suite

This directory contains the Grassroots testing suite and files.

## Test Documentation

> [!IMPORTANT]
> When changing test files, don't forget to update documentation as necessary!

Further documentation for different test files are located inside
the `docs/test/` directory.

## Test definitions

### `TEST_BUILD`

Defined only when compiled as part of a test.
To be used to expose functions only when testing.

### `TEST_STATIC`

Define to `static` only when not compiled as part of a test.
To be used to mark functions as static when not testing.

### `PREBUILT_DIR`

Defined to the location of `ext/prebuild` when compiled as part of a test.
To be used to access prebuilt files for testing.


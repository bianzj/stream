# STREAM

STREAM is a C++ land-surface and radiative-transfer simulation model. It
combines meteorological forcing, vegetation/soil properties, canopy radiation,
energy balance, photosynthesis, and optional satellite observation simulation.

The executable target is `stream`. Source code is organised as follows:

```text
base/       Core model, I/O, radiation, energy balance and state estimation
china/      Main program for the China/local configuration
beijing/    Alternative main program
global/     Alternative global main program
sensor/     Sensor-related declarations
target/     Legacy target declarations
```

## Build on Ubuntu

The project uses CMake and C++20. GDAL, HDF5 and their dependencies must be
available through the configured toolchain.

```bash
cmake -S . -B build
cmake --build build -j2
```

The current executable expects a metadata file. The metadata format and input
directory layout are kept compatible with the existing STREAM data package:

```bash
./build/stream /path/to/input.txt [output-directory]
```

The default `china/main.cpp` input path is local-machine specific, so passing a
metadata file explicitly is recommended.

## Optional state sources

LAI and soil moisture can independently come from measured/ERA5 data, the
retained model state, or the EnKF analysis:

```bash
export STREAM_LAI_SOURCE=observation
export STREAM_SOIL_MOISTURE_SOURCE=observation
```

Allowed values are `observation`, `model`, `crop_model`, and `assimilation`;
numeric aliases `0`, `1`, `3`, and `2` are also accepted. `crop_model` is a
generic temperature/soil-moisture crop prior that does not require cultivar or
irrigation maps. `assimilation` requires
`STREAM_ENKF_ENABLED=1`. Otherwise it safely falls back to measured/ERA5 data.

For a long time series with intermittent LAI/soil-moisture observations:

```bash
export STREAM_CROP_MODEL_ENABLED=1
export STREAM_LAI_SOURCE=crop_model
export STREAM_SOIL_MOISTURE_SOURCE=assimilation
export STREAM_ENKF_ENABLED=1
export STREAM_BALANCE_ENABLED=1
export STREAM_PRECIPITATION_FILE=/path/to/precipitation/{year}/{doy}.h5
```

On dates without valid LAI or soil-moisture observations, the crop prior and
water balance continue their forecast. Valid observations are used when they
are available.

## Optional water/carbon balance

The water and carbon balance is opt-in and remains disabled by default:

```bash
export STREAM_BALANCE_ENABLED=1
```

It provides a simple root-zone water bucket using latent heat as ET, optional
precipitation input, canopy interception/throughfall, infiltration, drainage,
and a leaf-carbon pool driven by Farquhar gross assimilation and respiration.
Daily products are written under the normal
`<output>/<year>/<DDD>/` directory, including ET, precipitation, runoff, GPP,
NPP, soil water and state LAI. Rainfall partition products include
interception, throughfall, infiltration and drainage. If soil moisture is
assimilated, its water correction is written separately rather than silently
treated as irrigation.

Precipitation is never inferred. To provide it, set
`STREAM_PRECIPITATION_FILE` to an HDF5 file pattern with a `data` dataset of
`[time, height, width]` values in millimetres per forcing step. `{year}` and
`{doy}` (or `{YYYY}` and `{DDD}`) are supported. Without a precipitation file,
the balance uses `P=0`.

## EnKF and read-only observations

The optional LAI/soil-moisture EnKF is disabled unless explicitly enabled:

```bash
export STREAM_ENKF_ENABLED=1
```

The read-only observation archive uses the CSV schema documented in
[`base/readme.md`](base/readme.md), with an example in
[`base/assimilation_observations.example.csv`](base/assimilation_observations.example.csv).

## Scope

The balance and EnKF are intentionally preparatory implementations. A
production water/carbon budget still needs calibrated multilayer soil
hydraulics, rainfall intensity, runoff routing, soil and litter respiration,
and site-specific canopy allocation parameters.

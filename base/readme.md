## Optional LAI/soil-moisture EnKF

The STREAM executable can use the LAI and soil-moisture products already
configured in the input metadata as observations for a small per-pixel
ensemble Kalman filter.  The filter is disabled by default and does not alter
the existing simulation unless it is explicitly enabled:

```bash
STREAM_ENKF_ENABLED=1 ./stream /path/to/input.txt
```

Useful optional settings are:

```text
STREAM_ENKF_ENSEMBLE_SIZE=20
STREAM_ENKF_LAI_OBS_ERROR=0.30
STREAM_ENKF_SM_OBS_ERROR=0.05
STREAM_ENKF_LAI_PROCESS_NOISE=0.05
STREAM_ENKF_SM_PROCESS_NOISE=0.01
STREAM_ENKF_SEED=20250906
```

The filter remains separate from the optional water/carbon balance.  It is not
run unless `STREAM_ENKF_ENABLED` is explicitly set.
The analysis is only applied when `STREAM_ENKF_ENABLED` is set to `1`, `true`,
or `TRUE`.

An optional, read-only archive can be loaded with
`STREAM_ASSIMILATION_FILE`.  Its CSV columns are:

```text
year,doy,hour,variable,value,uncertainty,row,col,window_hours,quality
```

The source used by the physical model can be selected independently for LAI
and soil moisture:

```text
STREAM_LAI_SOURCE=observation
STREAM_SOIL_MOISTURE_SOURCE=observation
```

Accepted values are `observation` (measured/ERA5), `model` (the retained
model state), `crop_model` (the generic crop prior), and `assimilation` (the
EnKF analysis). Numeric values `0`, `1`, `3`, and `2` are also accepted.
`STREAM_SM_SOURCE` is an alias for the soil
moisture setting. If `assimilation` is selected while EnKF is disabled, the
corresponding variable falls back to measured/ERA5 data.

The generic crop prior is enabled with `STREAM_CROP_MODEL_ENABLED=1` (it is
also enabled automatically by `STREAM_LAI_SOURCE=crop_model`). It treats crop
pixels as one robust functional type, uses a default sowing date and
temperature accumulation, and applies soil-moisture stress. The main settings
are `STREAM_CROP_SOWING_DOY`, `STREAM_CROP_BASE_TEMPERATURE_C`,
`STREAM_CROP_EMERGENCE_GDD`, `STREAM_CROP_PEAK_GDD`,
`STREAM_CROP_MATURITY_GDD`, `STREAM_CROP_MAX_LAI`, and
`STREAM_CROP_MIN_LAI`.

## Optional water and carbon balance

The balance is also disabled by default. Enable it only for a test run with:

```bash
STREAM_BALANCE_ENABLED=1 ./stream /path/to/input.txt
```

It integrates positive latent heat as evapotranspiration, optional
precipitation as input, a root-zone storage bucket and the Farquhar leaf
carbon fluxes. The default parameters are:

```text
STREAM_BALANCE_DT_SECONDS=3600
STREAM_BALANCE_ROOT_DEPTH_M=1.0
STREAM_BALANCE_SLA=0.02
STREAM_BALANCE_LEAF_CARBON_FRACTION=0.45
STREAM_BALANCE_LEAF_ALLOCATION=0.40
STREAM_BALANCE_LEAF_TURNOVER=0.01
STREAM_BALANCE_CANOPY_STORAGE_PER_LAI=0.20
STREAM_BALANCE_CANOPY_STORAGE_BASE=0.0
STREAM_BALANCE_FIELD_CAPACITY=0.35
STREAM_BALANCE_DRAINAGE_RATE=0.20
```

Precipitation is optional and is never inferred. Set
`STREAM_PRECIPITATION_FILE` to an HDF5 file pattern containing a `data`
dataset with `[time, height, width]` values in millimetres per forcing step.
The pattern supports `{year}` and `{doy}` (or `{YYYY}` and `{DDD}`). If a
directory is supplied, the default file is
`<directory>/<year>/<DDD>/precipitation.h5`. Without a file, `P=0` is used and
the output states this limitation.

When enabled, each processed day writes these one-band HDF5 products under
the normal `output/<year>/<DDD>/` directory:

```text
et_daily_sim.h5                  # mm/day
precipitation_daily_sim.h5       # mm/day
interception_daily_sim.h5         # mm/day retained by canopy
throughfall_daily_sim.h5          # mm/day reaching soil surface
infiltration_daily_sim.h5         # mm/day entering soil bucket
runoff_daily_sim.h5              # mm/day
drainage_daily_sim.h5             # mm/day leaving root zone
assimilation_water_increment_daily_sim.h5 # mm/day from SM correction
gpp_daily_sim.h5                 # g C m-2/day
plant_respiration_daily_sim.h5   # g C m-2/day
npp_daily_sim.h5                 # g C m-2/day
soil_water_sim.h5                # root-zone mm at end of day
lai_state_sim.h5                 # LAI at end of day
```

This remains a deliberately simple balance. It provides per-grid-cell
rainfall partitioning and bucket overflow runoff, but not routed streamflow.
Daily precipitation is in millimetres per forcing step. If ERA5 `tp` is used,
convert its metre units to millimetres first. A production water/carbon budget
still needs multilayer soil hydraulics, rainfall intensity, runoff routing,
litter and soil respiration, and calibrated canopy allocation parameters.

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
model state), and `assimilation` (the EnKF analysis). Numeric values `0`, `1`,
and `2` are also accepted. `STREAM_SM_SOURCE` is an alias for the soil
moisture setting. If `assimilation` is selected while EnKF is disabled, the
corresponding variable falls back to measured/ERA5 data.

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
runoff_daily_sim.h5              # mm/day
gpp_daily_sim.h5                 # g C m-2/day
plant_respiration_daily_sim.h5   # g C m-2/day
npp_daily_sim.h5                 # g C m-2/day
soil_water_sim.h5                # root-zone mm at end of day
lai_state_sim.h5                 # LAI at end of day
```

This is a deliberately simple preparatory balance. A production water/carbon
budget still needs rainfall partitioning, multilayer soil hydraulics,
infiltration/percolation, runoff parameterisation, litter and soil
respiration, and calibrated canopy allocation parameters.

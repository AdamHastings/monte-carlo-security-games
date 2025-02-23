# Artifacts for "Voluntary Investment, Mandatory Minimums, or Cyber Insurance: What minimizes losses?"
#### To appear at Usenix Security 2025

This repository contains the code and data necessary to reproduce all figures, regressions, and simulations used in the paper "Voluntary Investment, Mandatory Minimums, or Cyber Insurance: What minimizes losses?", to appear at Usenix Security 2025.
A older, stable version of this repository  can be found on Zenodo: 
[https://zenodo.org/records/14728686](https://zenodo.org/records/14728686)

## Reproducing Parameter Calculations

Scripts to reproduce parameter calculations are in `parameter-calcs/` and are written in Python

- To reproduce Figure 1, run `$ python3 parameter-calcs/scripts/ransom_regression_plot.py` ,which will produce outputs in `parameter-calcs/figures/ransom_regression/`.
- To reproduce Figure 2, run `$ python3 parameter-calcs/scripts/plot_erf.py`, which will produce outputs in `parameter-calcs/figures/erf/`.
- To reproduce our regression of the real-world attack rate, run `$ python3 parameter-calcs/scripts/curve_fit_attack_rate.py`, which will produce outputs in `parameter-calcs/figures/attackrate_fitting/`.
- To reproduce our regression of the real-world security posture, run `$ python3 parameter-calcs/scripts/curve_fit_posture.py`, which will produce outputs in ``parameter-calcs/figures/posture_fitting/`.
- To reproduce our regression on wealth estimation, including statistical tests, run `$ python3 parameter-calcs/scripts/fit_marketcap_revenue_curves.py`,
- To view our analytic optimization calculations, view `parameter-calcs/scripts/secinvestments.b` (may require a Mathematica license).

## Reproducing Simulator Outputs

To reproduce the simulator outputs, we recommend using the containerized application, which can be found on Zenodo: 
[https://zenodo.org/records/14728686](https://zenodo.org/records/14728686)

Re-running the simulator on all inputs may take a while.
**To reproduce outputs without re-running the simulator, download `artfiacts.tar.gz` from Zenodo, which contains the dataset used in the paper**


#### Baseline Model Behavior

To reproduce Figures 3--6, run 

```
cd simulator/scripts
python3 run_all.py ../logs/fullsize_short.csv
```

Results will be in `simulator/scripts/figures/fullsize_short/`.

To re-run the simulator and reproduce the dataset used above, run 

```
./simulator/run/release/run_games simulator/configs/fullsize_short.json
```


#### One-Way Sensitivity Analysis

To reproduce Figure 7, run 

```
cd simulator/scripts
python3 plot_sensitivity_analysis.py
```

Results will be in `simulator/scripts/figures/sensitivvity_analysis/`.

To re-run the simulator and reproduce the dataset used above, run each config file in `simulator/configs/sweeps/`, e.g.

```
./simulator/run/release/run_games simulator/configs/sweeps/MAX_ITERATIONS=500/sweep_INEQUALITY.json
```

#### Mandated Security Investments

To reproduce Figures 8--10, run 

```
cd simulator/scripts
python3 run_all.py ../logs/fullsize_short_MANDATORY_INVESTMENT=0.01.csv
python3 run_all.py ../logs/fullsize_short_MANDATORY_INVESTMENT=0.02.csv
python3 run_all.py ../logs/fullsize_short_MANDATORY_INVESTMENT=0.03.csv
python3 run_all.py ../logs/fullsize_short_MANDATORY_INVESTMENT=0.04.csv
python3 run_all.py ../logs/fullsize_short_MANDATORY_INVESTMENT=0.045csv
```

Results will be in `simulator/scripts/figures/`, e.g. `simulator/scripts/figures/fullsize_short_MANDATORY_INVESTMENT=0.01/`

To re-run the simulator and reproduce the datasets used above, run 

```
./simulator/run/release/run_games simulator/configs/fullsize_short_MANDATORY_INVESTMENT=0.01.json
./simulator/run/release/run_games simulator/configs/fullsize_short_MANDATORY_INVESTMENT=0.02.json
./simulator/run/release/run_games simulator/configs/fullsize_short_MANDATORY_INVESTMENT=0.03.json
./simulator/run/release/run_games simulator/configs/fullsize_short_MANDATORY_INVESTMENT=0.04.json
./simulator/run/release/run_games simulator/configs/fullsize_short_MANDATORY_INVESTMENT=0.05.json
```

#### Mandated Insurance

To reproduce Figures 11--12, run 

```
cd simulator/scripts
python3 run_all.py ../logs/fullsize_short_mandatory_insurance.csv"
```

Results will be in `simulator/scripts/figures/fullsize_short_mandatory_insurance/`

To re-run the simulator and reproduce this dataset, run

```
./simulator/run/release/run_games simulator/configs/fullsize_short_mandatory_insurance.json
```

#### Actuarially Fair Insurance

To reproduce Figures 13--14, run 

```
cd simulator/scripts
python3 run_all.py ../logs/fullsize_short_selfless_insurers.csv"
```

Results will be in `simulator/scripts/figures/fullsize_short_selfless_insurers/`

To re-run the simulator and reproduce this dataset, run

```
./simulator/run/release/run_games simulator/configs/fullsize_short_selfless_insurers.json
```

#### A Non-Closed Ecosystem

To reproduce Figures 15--16, run 

```
cd simulator/scripts
python3 run_all.py ../logs/fullsize_short_with_asset_growth_GROWTH_RATE=0.001.csv"
python3 run_all.py ../logs/fullsize_short_with_asset_growth_GROWTH_RATE=0.002.csv"
python3 run_all.py ../logs/fullsize_short_with_asset_growth_GROWTH_RATE=0.01.csv"
```

Results will be in `simulator/scripts/figures/`, e.g. `simulator/scripts/figures/fullsize_short_with_asset_growth_GROWTH_RATE=0.01/`

To re-run the simulator and reproduce this dataset, run

```
./simulator/run/release/run_games simulator/configs/fullsize_short_with_asset_growth_GROWTH_RATE=0.001.json
./simulator/run/release/run_games simulator/configs/fullsize_short_with_asset_growth_GROWTH_RATE=0.002.json
./simulator/run/release/run_games simulator/configs/fullsize_short_with_asset_growth_GROWTH_RATE=0.01.json
```
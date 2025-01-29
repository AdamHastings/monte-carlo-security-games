important_figs=(
    "figures/fullsize_short"
    "figures/fullsize_short_MANDATORY_INVESTMENT=0.01/"
    "figures/fullsize_short_MANDATORY_INVESTMENT=0.02/"
    "figures/fullsize_short_MANDATORY_INVESTMENT=0.03/"
    "figures/fullsize_short_MANDATORY_INVESTMENT=0.04/"
    "figures/fullsize_short_MANDATORY_INVESTMENT=0.05/"
    "figures/fullsize_short_selfless_insurers/"
    "figures/fullsize_short_mandatory_insurance/"
    "figures/sensitivity_analysis/"
    "figures/fullsize_short_with_asset_growth_GROWTH_RATE=0.001/"
    "figures/fullsize_short_with_asset_growth_GROWTH_RATE=0.002/"
    "figures/fullsize_short_with_asset_growth_GROWTH_RATE=0.01/"
    "../../parameter-calcs/figures/erf/erf.pdf"
    "../../parameter-calcs/figures/ransom_regression/ransom_regression.pdf"
)

for i in ${important_figs[@]}; do
    echo $i
    cp -r $i ~/insurance-paper/figures
done 
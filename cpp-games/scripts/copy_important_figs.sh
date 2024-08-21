important_figs=(
    "figures/fullsize_short/asset_flow_sankey.pdf"
    "figures/fullsize_short/canary_vars_p_attack.pdf"
    "figures/fullsize_short/canary_vars.pdf"
    "figures/fullsize_short/choices.pdf"
    "figures/fullsize_short/choices_pcts.pdf"
    "figures/fullsize_short/cumulative_assets.pdf"
    "figures/fullsize_short_MANDATORY_INVESTMENT=0.01/"
    "figures/fullsize_short_MANDATORY_INVESTMENT=0.02/"
    "figures/fullsize_short_MANDATORY_INVESTMENT=0.03/"
    "figures/fullsize_short_MANDATORY_INVESTMENT=0.04/"
    "figures/fullsize_short_MANDATORY_INVESTMENT=0.05/"
    "figures/fullsize_short_selfless_insurers/"
)

for i in ${important_figs[@]}; do
    echo $i
    cp -r $i ~/insurance-paper/figures
done 
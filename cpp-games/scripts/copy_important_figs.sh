important_figs=(
    "figures/fullsize_short/asset_flow_sankey.pdf"
    "figures/fullsize_short/canary_vars_p_attack.pdf"
    "figures/fullsize_short/canary_vars.pdf"
    "figures/fullsize_short/choices.pdf"
    "figures/fullsize_short/cumulative_assets.pdf"
)

for i in ${important_figs[@]}; do
    echo $i
    cp $i ~/insurance-paper/figures
done 
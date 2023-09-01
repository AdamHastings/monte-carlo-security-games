source tbb/latest/env/vars.sh

echo "y" | ./run_games configs/ows_ATTACKERS.json
echo "y" | ./run_games configs/ows_CAUGHT.json
echo "y" | ./run_games configs/ows_CLAIMS.json
echo "y" | ./run_games configs/ows_EFFICIENCY.json
echo "y" | ./run_games configs/ows_EFFORT.json
echo "y" | ./run_games configs/ows_INEQUALITY.json
echo "y" | ./run_games configs/ows_MANDATE.json
echo "y" | ./run_games configs/ows_PAYOFF.json
echo "y" | ./run_games configs/ows_PREMIUM.json
echo "y" | ./run_games configs/ows_TAX.json


for i in {1..999}
do
    echo "a" | ./run_games configs/ows_ATTACKERS.json
    echo "a" | ./run_games configs/ows_CAUGHT.json
    echo "a" | ./run_games configs/ows_CLAIMS.json
    echo "a" | ./run_games configs/ows_EFFICIENCY.json
    echo "a" | ./run_games configs/ows_EFFORT.json
    echo "a" | ./run_games configs/ows_INEQUALITY.json
    echo "a" | ./run_games configs/ows_MANDATE.json
    echo "a" | ./run_games configs/ows_PAYOFF.json
    echo "a" | ./run_games configs/ows_PREMIUM.json
    echo "a" | ./run_games configs/ows_TAX.json
done 
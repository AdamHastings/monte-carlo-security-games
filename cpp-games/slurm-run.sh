#!/bin/sh
#
# Simple "Hello World" submit script for Slurm.
#
# Replace <ACCOUNT> with your account name before submitting.
#
#SBATCH --account=buddy          # The account name for the job.
#SBATCH --job-name=run_games     # The job name.
#SBATCH -c 24                    # The number of cpu cores to use.
#SBATCH --time=1:00              # The time the job will take to run.
#SBATCH --mem-per-cpu=1gb        # The memory the job will use per cpu core.
#SBATCH -o output/slurm-%j.out # Output location

./run_games test_medium.json
 
# End of script

import pymc as pm
import numpy as np

# 1. THE DATA
# Let's say your historical average was 150 fruits per tree (The Prior)
# This year, your early samples show 180, 195, 175 (The Observed)
observed_harvest_samples = np.array([180, 195, 175, 185])

with pm.Model() as avocado_model:
    # 2. THE PRIOR (What we believe before the new data)
    # We use a Gamma distribution because it's always positive
    expected_yield = pm.Gamma("expected_yield", alpha=15, beta=0.1)

    # 3. THE LIKELIHOOD (How the data connects to the model)
    # We use Poisson because we are counting individual fruits
    observations = pm.Poisson("observations", mu=expected_yield, observed=observed_harvest_samples)

    # 4. THE INFERENCE (Updating the model)
    trace = pm.sample(1000, return_inferencedata=True)

# 5. THE RESULT
# This gives you the 'Posterior' - the new, updated belief of your yield.
print("Updated Yield Estimate (Mean):", trace.posterior["expected_yield"].mean().values)

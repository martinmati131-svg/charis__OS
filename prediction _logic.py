import pymc as pm
import numpy as np

# DATA: Number of hours your motors ran before needing maintenance in previous tests
# Let's say: 500hrs, 480hrs, 520hrs, 450hrs
past_failure_data = np.array([500, 480, 520, 450])

with pm.Model() as maintenance_model:
    # 1. THE PRIOR: Average life expectancy (Exponential distribution)
    # We assume the average failure rate (lam) is around 1/500
    lam = pm.Exponential("lam", lam=1.0/500)

    # 2. THE LIKELIHOOD: How our current hardware is actually performing
    observations = pm.Exponential("obs", lam=lam, observed=past_failure_data)

    # 3. POSTERIOR SAMPLING: Calculating the 'Real' health of your specific motor
    trace = pm.sample(1000, return_inferencedata=True)

# 4. PREDICTIVE LOGIC
# If the 'lam' increases, the 'Probability of Failure' in the next 24 hours spikes.
updated_failure_rate = trace.posterior["lam"].mean().values
print(f"Current Hazard Rate: {updated_failure_rate}")

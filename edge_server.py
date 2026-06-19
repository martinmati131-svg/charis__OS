from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
import random

app = FastAPI(title="Aura Edge Intelligence Gateway")

# Enable CORS so your mobile dashboard can access the API safely
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_methods=["*"],
    allow_headers=["*"],
)

@app.get("/api/telemetry")
def get_telemetry():
    """
    Exposes real-time system metrics. 
    In production, replace these placeholders with readings from your actual data logs.
    """
    # Simulating data coming from your Bayesian models and hardware logs
    try:
        with open('current_yield_estimate.txt', 'r') as f:
            yield_est = float(f.read().strip())
    except FileNotFoundError:
        yield_est = 182.5

    return {
        "system_status": "ONLINE",
        "sentinel_watchdog": "SECURE",
        "motor_hazard_rate": 0.0021,  # Output from your maintenance model
        "predicted_yield_mean": yield_est,
        "dsp_frequency_hz": 110.0
    }

if __name__ == "__main__":
    import uvicorn
    # Run the server locally on port 8000, visible to any device on the network
    uvicorn.run(app, host="0.0.0.0", port=8000)

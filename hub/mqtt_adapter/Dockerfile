FROM python:3.9-slim

WORKDIR /app

# Install dependencies
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Copy application code
COPY mqtt_adapter.py .

# Run the adapter
CMD ["python", "-u", "mqtt_adapter.py"]

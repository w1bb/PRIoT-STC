import json
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
from influxdb_client import InfluxDBClient, Point#, BucketRetentionRules
from influxdb_client.client.write_api import SYNCHRONOUS
import time
import os

# ==============================================================================

# MQTT (broker) settings
broker_address = os.getenv("BROKER_HOST", "localhost")
broker_port = int(os.getenv("BROKER_PORT", 1883))

# InfluxDB settings
influxdb_host = os.getenv("INFLUXDB_HOST", "localhost")
influxdb_port = int(os.getenv("INFLUXDB_PORT", 8086))
influxdb_token = os.getenv("INFLUXDB_TOKEN", "?")
influxdb_org = os.getenv("INFLUXDB_ORG", "-")
influxdb_bucket = os.getenv("INFLUXDB_BUCKET", "-")

# Create an InfluxDB client and wait for it to be ready
client = InfluxDBClient(
    url=f"http://{influxdb_host}:{influxdb_port}",
    token=influxdb_token,
    org=influxdb_org)
print("Waiting for InfluxDBClient to be ready...", end='')
# while client.ready().up is None:
#     print(".", end='')
#     time.sleep(1)

write_api = client.write_api(write_options=SYNCHRONOUS)

# Update bucket with retention policy
# retention_rule = BucketRetentionRules(type="expire", every_seconds=30*24*60*60)

# buckets_api = client.buckets_api()
# bucket = buckets_api.find_bucket_by_name(influxdb_bucket)
# if bucket:
#     buckets_api.update_bucket(bucket.id, retention_rules=[retention_rule])

# ==============================================================================

# Callback when connected to the MQTT broker
def on_connect(client, userdata, flags, reason_code, properties):
    print(f"Connected with result code {reason_code}")
    client.subscribe("wi/sh/temp_control/#")
    client.subscribe("wi/sh/request")

thermostat_value = False

# Callback when a message is received
def on_message(client, userdata, msg):
    try:
        global thermostat_value
        pi_timestamp = time.time()
        print(f"{pi_timestamp} :: Message from {msg.topic}: {msg.payload.decode()}")
        payload = json.loads(msg.payload.decode())

        if msg.topic == "wi/sh/request":
            if payload['request'] == 'time':
                timestamp = time.time()
                client.publish("wi/sh/vntp", f"{timestamp}")
                print(f"Responded with time = {timestamp}")
            elif payload['request'] == 'thermostat':
                name = payload['uid']
                thermostat_value = not thermostat_value
                client.publish(f"wi/sh/thermostat/{name}", f"{1 if thermostat_value else 0}")
                print(f"Responded with thermostat_value = {thermostat_value}")
        else:
            # Get the content to be written
            measurement = msg.topic.replace("/", ".")[19:]
            timestamp = pi_timestamp if 'timestamp' not in payload else payload['timestamp']
            points = []
            for key, value in payload.items():
                if key == 'timestamp':
                    continue
                if isinstance(value, (int, float)):
                    points.append(
                        Point(measurement)
                            .field(key, value)
                            .time(int(timestamp * 1e9)))

            # Write the actual changes to the DB
            if points:
                write_api.write(
                    bucket=influxdb_bucket,
                    record=points)
                print(f"Data written to InfluxDB: {points}")
    except Exception as e:
        print(f"Error processing message: {e}")

# ==============================================================================

# Create MQTT client instance
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message

# Connect to the MQTT broker
client.connect(broker_address, broker_port, 60)
client.loop_forever()

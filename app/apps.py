# from django.apps import AppConfig


# class AppConfig(AppConfig):
#     default_auto_field = 'django.db.models.BigAutoField'
#     name = 'app'

# app/apps.py
# app/apps.py
from django.apps import AppConfig
import threading
import paho.mqtt.client as mqtt
from django.utils import timezone
import os
# from .models import SensorData

class AppAppConfig(AppConfig):
    default_auto_field = 'django.db.models.BigAutoField'
    name = 'app'
    if os.environ.get('RUN_MAIN') == 'true':
        def ready(self):
            from .models import SensorData  # Import here to avoid AppRegistryNotReady error

            def on_message(client, userdata, message):
                payload = message.payload.decode()
                print(f"{timezone.now()}-{message.topic}: {payload}")
                try:
                    data_value = float(payload)
                    SensorData.objects.create(
                        dataName=message.topic, 
                        dataValue=data_value,
                        date=timezone.now() + timezone.timedelta(hours=3.5))
                except ValueError:
                    # Handle case where payload is not a valid float
                    pass

            def start_mqtt_client():
                mqtt_client = mqtt.Client()
                mqtt_client.on_message = on_message
                mqtt_client.username_pw_set('admin', '123')
                # mqtt_client.connect("192.168.63.248", 1883, 60)
                mqtt_client.connect("192.168.183.248", 1883, 60)

                mqtt_client.subscribe("Temperature")
                mqtt_client.subscribe("Humidity")
                mqtt_client.subscribe("Moisture")
                mqtt_client.subscribe("GasValue")
                mqtt_client.subscribe("PumpState")
                mqtt_client.subscribe("WaterHeight")
                mqtt_client.subscribe("FanState-1")
                mqtt_client.subscribe("FanState-2")
                mqtt_client.subscribe("HeaterState")
                mqtt_client.loop_forever()

            thread = threading.Thread(target=start_mqtt_client)
            thread.start()


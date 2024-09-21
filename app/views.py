# views.py in your 'app' directory
from django.shortcuts import render
from .models import SensorData
import json
from django.db.models import Avg
from datetime import date


def dashboard(request):
    # Fetch the latest entry for 'gasvalue'
    latest_temperature = SensorData.objects.filter(dataName='Temperature').order_by('-date').first()
    latest_humidity = SensorData.objects.filter(dataName='Humidity').order_by('-date').first()
    latest_moisture = SensorData.objects.filter(dataName='Moisture').order_by('-date').first()
    latest_gasvalue = SensorData.objects.filter(dataName='GasValue').order_by('-date').first()
    latest_pumpState = SensorData.objects.filter(dataName='PumpState').order_by('-date').first()
    latest_waterHeight = SensorData.objects.filter(dataName='WaterHeight').order_by('-date').first()
    latest_fanState_1 = SensorData.objects.filter(dataName='FanState-1').order_by('-date').first()
    latest_fanState_2 = SensorData.objects.filter(dataName='FanState-2').order_by('-date').first()
    latest_heaterState = SensorData.objects.filter(dataName='HeaterState').order_by('-date').first()

    # Fetch the latest 12 sensor data entries for 'gasvalue'
    gasvalue_data = SensorData.objects.filter(dataName='GasValue').order_by('-date')[:12]
    gasvalue_data_list = list(gasvalue_data.values('dataValue', 'date'))
    
    # Fetch the latest 12 sensor data entries for 'temperature'
    temperature_data = SensorData.objects.filter(dataName='Temperature').order_by('-date')[:12]
    temperature_data_list = list(temperature_data.values('dataValue', 'date'))

    temperatures_labels_list=[data['date'].strftime('%H:%M') for data in temperature_data_list]
    temperatures_labels_list.reverse()

    temperatures_values_list=[data['dataValue'] for data in temperature_data_list]
    temperatures_values_list.reverse()

    gasvalue_labels_list = [data['date'].strftime('%H:%M') for data in gasvalue_data_list]
    gasvalue_labels_list.reverse()

    gasvalue_values_list = [data['dataValue'] for data in gasvalue_data_list]
    gasvalue_values_list.reverse()

    chart_data = {
        'temperatures_labels': temperatures_labels_list,
        'temperatures_values': temperatures_values_list,

        'gasvalue_labels': gasvalue_labels_list,
        'gasvalue_values': gasvalue_values_list,
    }
    chart_data_json = json.dumps(chart_data)
    print(chart_data['gasvalue_values'])
    print(chart_data['gasvalue_labels'])
    # mean humidity for today
    today = date.today()
    humidity_mean = SensorData.objects.filter(dataName='Humidity', date__date=today).aggregate(Avg('dataValue'))['dataValue__avg']
    if humidity_mean is None:
        humidity_mean = 0

    humidity_mean_data = {'humidity_mean': [100-humidity_mean,humidity_mean]}
    humidity_mean_json = json.dumps(humidity_mean_data)

    # log status
    log_status = SensorData.objects.filter(dataName__in=['PumpState', 'FanState-1', 'FanState-2', 'HeaterState'], date__date=today).order_by('-date')[:10]
    
    # mean temperature for each month
    monthly_data = {month: {'sum': 0, 'count': 0} for month in range(1, 13)}
        
    # Query for all temperature entries in the current year
    temperature_entries = SensorData.objects.filter(
        dataName="Temperature",
        date__year=today.year
    )
        
    # Sum the temperatures and count the entries for each month
    for entry in temperature_entries:
        month = entry.date.month
        monthly_data[month]['sum'] += entry.dataValue
        monthly_data[month]['count'] += 1
    
        
    # Calculate the average temperature for each month
    average_temperatures_by_month = [
        (monthly_data[month]['sum'] / monthly_data[month]['count']) if monthly_data[month]['count'] > 0 else None
        for month in range(1, 13)
    ]
    average_temperatures_by_month = [0 if x is None else x for x in average_temperatures_by_month]
    average_temperatures_by_month_data = {'temperatures_mean': average_temperatures_by_month}
    average_temperatures_by_month_json = json.dumps(average_temperatures_by_month_data)

    context = {
        'latest_temperature': latest_temperature,
        'latest_humidity': latest_humidity,
        'latest_moisture': latest_moisture,
        'latest_gasvalue': latest_gasvalue,
        'latest_pumpState': latest_pumpState,
        'humidity_mean_json': humidity_mean_json,
        'chart_data_json': chart_data_json,
        'log_status': log_status,
        'average_temperatures_by_month_json': average_temperatures_by_month_json,
        'latest_fanState_1': latest_fanState_1,
        'latest_fanState_2': latest_fanState_2,
        'latest_heaterState': latest_heaterState,
        'latest_waterHeight': latest_waterHeight,
    }


    return render(request, 'dashboard.html', context)
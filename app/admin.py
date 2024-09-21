from django.contrib import admin
from .models import SensorData

admin.site.register(SensorData)
class SensorDataAdmin(admin.ModelAdmin):
    list_display = ('dataName', 'dataValue', 'date')
    readonly_fields = ('date',)

from django.db import models

class SensorData(models.Model):
    id = models.IntegerField(primary_key=True)
    dataName = models.CharField(max_length=100)
    dataValue = models.FloatField()
    date = models.DateTimeField()


    def __str__(self):
        return f"{self.id}-{self.dataName.upper()}"
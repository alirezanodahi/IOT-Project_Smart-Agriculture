# Generated by Django 4.1.7 on 2024-06-18 11:30

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('app', '0001_initial'),
    ]

    operations = [
        migrations.AlterField(
            model_name='sensordata',
            name='date',
            field=models.DateTimeField(),
        ),
    ]

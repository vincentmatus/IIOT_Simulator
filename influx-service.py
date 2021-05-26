
#import sched, time
#from time import sleep
import json
import schedule
import time
import datetime

from influxdb import InfluxDBClient

#s = sched.scheduler(time.time, time.sleep)
client = InfluxDBClient(host='localhost', port=8086)
client.switch_database('telegraf')


def oee_calandinsert():

        print("Querying")
        #Querying data
        results = client.query('SELECT * FROM "mqtt_consumer";')
        points = results.get_points()
        print(results)
        worktime=0
        timeneed=1320
        totalproduct=0
        badproduct=0
        for point in points:
                worktime+=point['on']
        stoptime=timeneed-worktime
        avgtime=0.2
        points = results.get_points()
        for point in points:
                totalproduct+=point['tot']

        points = results.get_points()
        for point in points:
                badproduct+=point['bad']

        print("worktime = ",worktime)
        print("totalproduct = ",totalproduct)
        print("badproduct = ",badproduct)

        ava = (timeneed-stoptime)/timeneed
        per = (avgtime*totalproduct)/worktime
        qua = (totalproduct-badproduct)/totalproduct
        oee = (ava*per*qua)
        #Inserting data
        json_body = [
                {
                "measurement": "oee",
                "tags": {
                },
                "time": time.time_ns(),
                "fields": {
                        "Ava": ava,
                        "Per": per,
                        "Qua": qua,
                        "OEE": oee
                }
                }
        ]

        print(json_body)
        client.write_points(json_body)
        points = results.get_points()
        client.switch_database('uRTUlog')
        #results.keys()[0][0]="log"
        #print(results.keys()[0][0])
        #client.write(results)
        for point in points:
                json_body2 = [
                        {
                        "measurement": "log",
                        "tags": {
                        },
                        "time": time.time_ns(),
                        "fields":{
                                "bad": point['bad'],
                                "host": point['host'],
                                "ipc": point['ipc'],
                                "on": point['on'],
                                "topic": point['topic'],
                                "tot": point['tot'],
                                "vcl": point['vcl']
                        }
                        }
                ]
                #print(json_body2)
                client.write_points(json_body2)
        client.switch_database('telegraf')
        client.drop_measurement(measurement='mqtt_consumer')
        #print("point =",point)
        #print(type(point))
        #print(json.dumps(point))
        #client.write(json.dumps(point))
        #print(results)
        print("Workdone")





#s.enter(60,1, oee_calandinsert, (s,))
#s.run()

schedule.every().day.at("00:00").do(oee_calandinsert)

while True:
    schedule.run_pending()
    print(datetime.datetime.now())
    time.sleep(1)



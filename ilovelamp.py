#!/usr/bin/env python

import splunk.Intersplunk as si
import splunk.mining.dcutils as dcu

import json
import socket
import csv
import paho.mqtt.publish as publish
import sys
import splunk.search as search
import splunk.entity as entity
from splunk.rest import simpleRequest
from subprocess import call


DEFAULT_TOPIC = "test"
DEFAULT_MESSAGE = 0
        
def sendData(results, settings):
    keywords, argvals = si.getKeywordsAndOptions()

    server_topic = DEFAULT_TOPIC
    server_message = DEFAULT_MESSAGE

    if argvals.get('topic'):
        server_topic = argvals.get('topic')
    if argvals.get('message'):
        try:
            server_message = int(argvals.get('message'))
        except Exception, e:            
            si.generateErrorResults('Invalid value for message')
            sys.exit(-1)

    publish.single(server_topic, server_message, hostname="localhost")
    return results

settings = {}

results = si.readResults(None, settings)

try:
    results = sendData(results, settings)
except Exception, e:
    si.generateErrorResults(str(e))
    sys.exit(-1)
    
si.outputResults(results)



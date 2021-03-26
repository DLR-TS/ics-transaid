#!/usr/bin/python3
#https://github.com/slackapi/python-slackclient
#https://stackoverflow.com/questions/34852104/can-we-send-message-to-user-in-slack-using-python-script
#---------------------------------------------------------------------------------------------------------------------------------------------

#transaidbot app in slack
#OAuth Access Token
#xoxp-418837038352-764810715174-889771133973-53a9b6919bc0f5625764c1a9a2c45c23##

#Bot User OAuth Access Token
#xoxb-418837038352-889761395008-g5dPXeS6szsEW8T7Wj8H1rC3

import os, sys, errno
import datetime
import requests

wdir = os.path.dirname(os.path.realpath(__file__))

folder = os.path.basename(wdir)

temp = os.path.normpath(os.getcwd() + os.sep + os.pardir)
index = temp.rfind("/") + 1
folder_up = temp[index:]

date_time = datetime.datetime.now().strftime("%m/%d/%Y, %H:%M:%S")

# slack access bot token
slack_token = "xoxb-418837038352-889761395008-g5dPXeS6szsEW8T7Wj8H1rC3"

message = folder_up + " - " + folder + " simulation finished at : " + date_time

data = {
    'token': slack_token,
    'channel': 'UNGPUM154',    # User ID. 
    'as_user': True,
    'text': message
}

requests.post(url = 'https://slack.com/api/chat.postMessage', data = data)

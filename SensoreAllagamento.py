#!/usr/bin/python

#Librerie
import sys
import RPi.GPIO as GPIO
import time
# Import smtplib for the actual sending function
import smtplib
# Import the email modules we'll need
from email.mime.text import MIMEText

####################
#Configurazione    #
####################

#Pin dei ingresso sensore
waterControlPin=2

#Paramentri email.
sender="lucagiuliodori@gmail.com"
recipient="lucagiuliodori@gmail.com"

#Modalita' di indicazione del pin (numerazione GPIO)
GPIO.setmode(GPIO.BCM)

####################
#Programma         #
####################
print ("Iniziato")

def main(argv):

    GPIO.setup(waterControlPin,GPIO.IN)

    prevStaWaterControl=GPIO.input(waterControlPin)
		
    while 1:
        #Check pin.
        if(GPIO.input(waterControlPin)==1):
            print ("1")
            if(prevStaWaterControl==0):
                #Send email
                #Crea il messaggio
                msg=MIMEText("Rilevato allagamento!!!")
                msg['Subject']="Avviso allagamento"
                msg['From']=sender
                msg['To']=recipient
                #Invia il messaggio.
                
                #msg = 'Why,Oh why!'
                username = 'lucagiuliodori@gmail.com'
                password = 'gldlcug157d'
                server = smtplib.SMTP('smtp.gmail.com:587')
                server.ehlo()
                server.starttls()
                server.login(username,password)
                server.sendmail(sender,recipient,msg)
                server.quit()
                
                #Set prevStaWaterControl.
                prevStaWaterControl=1
        else:
            print ("0")
            if(prevStaWaterControl==1):
                #Set prevStaWaterControl.
                prevStaWaterControl=0

    print ("Finito")
	
main(None)

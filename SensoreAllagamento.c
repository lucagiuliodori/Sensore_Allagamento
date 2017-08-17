#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<time.h>

typedef enum _RET_ERR
{
	RET_OK=-1,
	RET_ERR_EXPORT=-2,
	RET_ERR_DIRECTION=-3,
	RET_ERR_READ=-4,
	RET_ERR_UNEXPORT=-5,
}te_RET_ERR;

#define DIRECTION_IN "in"
#define DIRECTION_OUT "out"

#define DELAY_RESEND 300 //Secondi di attesa prima del reinvio del messaggio.

char pathSep[]="/";
char gpioNum[]="2";
char gpioName[]="gpio2";
char pathGpioControl[]="/sys/class/gpio/";
char fileNameExport[]="export";
char fileNameUnexport[]="unexport";
char fileNameDirection[]="direction";
char fileNameValue[]="value";
char currPath[1024];

bool isAlarm=false;

bool exportPin(void);
bool unexportPin(void);
bool setDirectionPin(char *dirPin);
bool readPin(unsigned char *value);
void sendAlarmOn(void);
void sendAlarmOff(void);
void sendAlarm(char *fileName,char *strToVis);


te_RET_ERR main(int argc, char **argv)
{
	char valPin;
	char *pPosChar;
	int posChar;
	time_t start_time;

	//Set curr path.
	pPosChar=strrchr(argv[0],pathSep[0]);
	posChar=(int)(pPosChar-argv[0]);
	strncpy(currPath,argv[0],posChar+1);
	
	//Export del PIN.
	if(!exportPin())
		return RET_ERR_EXPORT;
	
	//Settaggio direzione.
	if(!setDirectionPin(DIRECTION_IN))
		return RET_ERR_DIRECTION;
	
	//Lettura del pin.
	while(1)
	{
		if(!readPin(&valPin))
			return RET_ERR_READ;
		
		if(isAlarm)
		{
			if(valPin)
			{
				if(difftime(time(NULL),start_time)>DELAY_RESEND)
				{
					sendAlarmOn();
					isAlarm=true;
					start_time=time(NULL);
				}
			}
			else
			{
				sendAlarmOff();
				isAlarm=false;
			}
		}
		else
		{
			if(valPin)
			{
				sendAlarmOn();
				isAlarm=true;
				start_time=time(NULL);
			}
		}
			
	}
	
	//Unexport del PIN.
	if(!unexportPin())
		return RET_ERR_UNEXPORT;

	return RET_OK;
}

void sendAlarmOn(void)
{
	sendAlarm("mail_allagamento_rilevato.txt","Invio e-mail allagamento rilevalto.\n");
	sendAlarm("sms_mail_allagamento_rilevato.txt","Invio SMS allagamento rilevalto.\n");
}

void sendAlarmOff(void)
{
	sendAlarm("mail_allagamento_nonrilevato.txt","Invio e-mail allagamento NON rilevalto.\n\n");
	sendAlarm("sms_mail_allagamento_nonrilevato.txt","Invio SMS allagamento NON rilevalto.\n\n");
}

void sendAlarm(char *fileName,char *strToVis)
{
	time_t rawtime;
	struct tm *timeinfo;
	char commandMail[1024]={"\0"};
	
	//Preparazione stringhe percorsi.
	strcat(commandMail,"sudo ssmtp lucagiuliodori@gmail.com < ");
	strcat(commandMail,currPath);
	strcat(commandMail,fileName);
	
	system(commandMail);

	time(&rawtime);
	timeinfo=localtime(&rawtime);
	printf("%s",asctime (timeinfo));
	printf(strToVis);
}

bool readPin(unsigned char *value)
{
	FILE *fd;
	char pinValue;
	char pathFileNameValue[sizeof(pathGpioControl)+sizeof(gpioName)+sizeof(fileNameValue)+1]={"\0"};
	
	//Preparazione stringhe percorsi.
	strcat(pathFileNameValue,pathGpioControl);
	strcat(pathFileNameValue,gpioName);
	strcat(pathFileNameValue,pathSep);
	strcat(pathFileNameValue,fileNameValue);

	//Set direzione del pin.
	fd=fopen(pathFileNameValue, "r");
	if(fd==NULL)
	{
		printf("Errore lettura del PIN!\n");
		return false;
	}
	else
	{
		//Lettura file.
		fscanf(fd,"%c",&pinValue);
		//Chiusura file.
		fclose(fd);
		
		//Conversione valore.
		*value=pinValue-'0';
		
		//printf("Lettura del PIN eseguita.\n");
		return true;
	}
}

bool setDirectionPin(char *dirPin)
{
	time_t rawtime;
	struct tm *timeinfo;
	FILE *fd;
	char pathFileNameDirection[sizeof(pathGpioControl)+sizeof(gpioName)+sizeof(fileNameDirection)+1]={"\0"};
	
	//Preparazione stringhe percorsi.
	strcat(pathFileNameDirection,pathGpioControl);
	strcat(pathFileNameDirection,gpioName);
	strcat(pathFileNameDirection,pathSep);
	strcat(pathFileNameDirection,fileNameDirection);
	
	//Stampa data e ora.
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	printf("%s",asctime (timeinfo));
	//Set direzione del pin.
	fd=fopen(pathFileNameDirection, "w");
	if(fd==NULL)
	{
		printf("Errore settaggio direzione del PIN!\n");
		return false;
	}
	else
	{
		//Scrittura su file.
		fprintf(fd,"%s",dirPin);
		//Chiusura file.
		fclose(fd);
		
		printf("Settaggio direzione del PIN eseguito.\n");
		return true;
	}
}

bool exportPin(void)
{
	time_t rawtime;
	struct tm *timeinfo;
	FILE *fd;
	char pathFileNameExport[sizeof(pathGpioControl)+sizeof(fileNameExport)]={"\0"};
	
	//Preparazione stringhe percorsi.
	strcat(pathFileNameExport,pathGpioControl);
	strcat(pathFileNameExport,fileNameExport);
	
	//Stampa data e ora.
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	printf("%s",asctime (timeinfo));
	//Esport del pin.
	fd=fopen(pathFileNameExport, "w");
	if(fd==NULL)
	{
		printf("Errore export del PIN!\n");
		return false;
	}
	else
	{
		//Scrittura su file.
		fprintf(fd,"%s",gpioNum);
		//Chiusura file.
		fclose(fd);
		
		printf("Export del PIN eseguito.\n");
		return true;
	}
}

bool unexportPin(void)
{
	time_t rawtime;
	struct tm *timeinfo;
	FILE *fd;
	char pathFileNameUnexport[sizeof(pathGpioControl)+sizeof(fileNameUnexport)]={"\0"};

	strcat(pathFileNameUnexport,pathGpioControl);
	strcat(pathFileNameUnexport,fileNameUnexport);

	//Stampa data e ora.
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	printf("%s",asctime (timeinfo));
	//Esport del pin.
	fd=fopen(pathFileNameUnexport, "w");
	if(fd==NULL)
	{
		printf("Errore unexport del PIN!\n");
		return false;
	}
	else
	{
		//Scrittura su file.
		fprintf(fd,"%s",gpioNum);
		//Chiusura file.
		fclose(fd);
		
		printf("Unexport del PIN eseguito.\n");
		return true;
	}
}

#include <SoftwareSerial.h>
#include <SD.h>
#include <SPI.h>
#include <CStringBuilder.h>


SoftwareSerial gpsSerial = SoftwareSerial(2,3);
SoftwareSerial gsmSerial = SoftwareSerial(4,5);


String GSMId = "";

static char fileName[18];
char dt[15]; // datum a cas souboru
#define chipSelect 8

const unsigned char NMEA_HEADER[] = { '$' , 'G', 'P', 'R', 'M', 'C', ',' };

unsigned int hour;
unsigned int min;
unsigned int sec;
double lat;
double lon;
String verHem;
String horHem;
unsigned int day;
unsigned int month;
unsigned int year;
bool isDecoded;

void decodeLine(String line) {
	int param = 0;
	unsigned int to;
	isDecoded = true;

	for ( unsigned int from = 0; param<10 && from<line.length(); from= to+1 ) {
		to= line.indexOf(',', from);
		String parameter= line.substring(from,to);

    	char* c= parameter.c_str();
    	if ( param==1 ) {
			if ( sscanf(c,"%2d%2d%2d.",&hour,&min,&sec)==3 ) {
			}
    	}

    	else if ( param==2 ) {
    		String validity = String(c);
    		if (validity != "A") {
    			isDecoded = false;
    			break;
    		}
    	}

    	else if ( param==3 ) {
			lat= String(parameter.substring(0,2)).toDouble() + String(parameter.substring(2,4)+parameter.substring(5,10)).toDouble()/6000000;
		}

    	else if ( param==4 ) {
			verHem= String(c);
			if (verHem.equals("N") ) {
			}
			else if (verHem.equals("S")) {
				lat = -lat;
			}
    		else {
    			isDecoded = false;
    			break;
    		}
    	}

    	else if ( param==5 ) {
			lon= String(parameter.substring(0,3)).toDouble() + String(parameter.substring(3,5)+parameter.substring(6,11)).toDouble()/6000000;
    	}

    	else if ( param==6 ) {
    		horHem= String(c);
    		if (horHem.equals("E") ) {
    		}
    		else if (horHem.equals("W")) {
    			lon = -lon;
    		}
    		else {
    			isDecoded = false;
    			break;
    		}
    	}

    	else if ( param==9 ) {
    		if ( sscanf(c,"%2d%2d%2d",&day,&month,&year)==3 ) {
    		}
    	}
		param++;
    }
}

bool calcChecksum (char* line, char checksum[]) {
	int val[] = {0,0,0,0,0,0,0,0};
	unsigned char result1 = 0;
	unsigned char result2 = 0;
	char ptr1[2], ptr2[2];

	for (int i = 0; line[i] != 0; i++) {
		if ( line[i] & 128 ) val[0]++;
		if ( line[i] & 64 ) val[1]++;
		if ( line[i] & 32 ) val[2]++;
		if ( line[i] & 16 ) val[3]++;
		if ( line[i] & 8 ) val[4]++;
		if ( line[i] & 4 ) val[5]++;
		if ( line[i] & 2 ) val[6]++;
		if ( line[i] & 1 ) val[7]++;
	}

	if ( val[0]/2*2 != val[0] ) result1 |= 8;
	if ( val[1]/2*2 != val[1] ) result1 |= 4;
	if ( val[2]/2*2 != val[2] ) result1 |= 2;
	if ( val[3]/2*2 != val[3] ) result1 |= 1;
	sprintf(ptr1,"%X",result1);

	if ( val[4]/2*2 != val[4] ) result2 |= 8;
	if ( val[5]/2*2 != val[5] ) result2 |= 4;
	if ( val[6]/2*2 != val[6] ) result2 |= 2;
	if ( val[7]/2*2 != val[7] ) result2 |= 1;
	sprintf(ptr2,"%X",result2);

	if ( checksum[0] == ptr1[0] && checksum[1] == ptr2[0] ) {

		return true;
	}
	return false;
}

bool processGPS() {
	static int position = 0;
	int readCheckSum = -1;
	char checksum[2];
	boolean result = false;
	char line[82];

	while ( gpsSerial.available() ) {

		byte c = gpsSerial.read();
		if ( position < 7 ) {
			if ( char(c) == NMEA_HEADER[position] ) {
				if ( position>0 ) {
					line[position-1] = c;
				}
				position++;
			}
			else
				position = 0;
		}

		else {
			if(readCheckSum == -1) {
				if ( position < sizeof(line) ) {
					line[position-1] = c;
				}
				if ( c=='*' ) {
					line[position-1] = 0;
					Serial.println("line: "+String(line));

					readCheckSum = 0;
				}
				position++;
			}

			else {
				checksum[readCheckSum++] = c;
				if (readCheckSum > 1) {
					if (calcChecksum(line, checksum)) {
						decodeLine(String(line));
						result = isDecoded;
					}
					position = 0;
					readCheckSum = -1;
				}
			}
		}
	}
	return result;
}

void dateTime(uint16_t* date, uint16_t* time) {
	*date = FAT_DATE(year, month, day);
	*time = FAT_TIME(hour, min, sec);
}

void delay() {
	delay(500);
}

void readIMEI() {
	delay(1000);
	while (gsmSerial.available()) {
		char c = gsmSerial.read();
		if ( isdigit(c)!=0 ) {
			GSMId += String(c);
		}
	}
}

String doubleDigitFix(int i) {
	if(i < 10) {
		return String("0" + String(i));
	}
	return String(i);
}

void gsmSendHttp() {
	gpsSerial.end();
	gsmSerial.begin(9600);
	delay();

	if ( GSMId.length()==0 ) {
		gsmSerial.println(F("AT+GSN"));
		readIMEI();

		gsmSerial.println(F("AT+CIPMUX=0"));
		delay();

		gsmSerial.println(F("AT+CSTT=\"sazkamobil\",\"\",\"\""));
		delay();

		gsmSerial.println(F("AT+CIICR"));
		delay();

		gsmSerial.println(F("AT+CIFSR"));
		delay();
	}

	if( GSMId.length()>0 ) {

		gsmSerial.println(F("AT+CIPSTART=\"TCP\",\"brazda.eu\",80")); //Zahaji spojeni se serverem. muze byt UDP nebo TCP
		delay();

		char httpData[78];
		sprintf(httpData,"id=%s&d=%s&data=%s;%s;%04d%02d%02d;%02d%02d%02d;",
				GSMId.c_str(),dt,String(lon,6).c_str(),String(lat,6).c_str(),
				year+2000,month,day,hour,min,sec);

		char buff[226];
		CStringBuilder sb(buff, sizeof(buff));
		sb.print(F("POST /GpsServlet/WriteGpsPosition HTTP/1.0\r\n"));
		sb.print(F("Host: brazda.eu\r\n"));
		sb.print(F("Accept: *"));
		sb.print(F("/"));
		sb.print(F("*\r\n"));
		sb.print(F("Content-Length: "));
		sb.printf("%d",strlen(httpData));
		sb.print(F("\r\n"));
		sb.print(F("Content-Type: application/x-www-form-urlencoded\r\n"));
		sb.print(F("\r\n"));
		sb.printf("%s",httpData);

		gsmSerial.print(F("AT+CIPSEND="));
		gsmSerial.println(sb.length());
		delay();

		gsmSerial.print(buff);
		delay();

		gsmSerial.println(F("AT+CIPCLOSE"));
		delay(4000);
	}
	gsmSerial.end();
	gpsSerial.begin(9600);
}

void setup() {
	fileName[0]= 0;
	Serial.begin(9600);
	pinMode(10, OUTPUT);
	SD.begin(chipSelect);
	gpsSerial.begin(9600);
	delay(1000);
}

void sendOnSD(){
	delay(1000);
	if ( String(fileName).length()==0 ) {
		char path[7];
		SdFile::dateTimeCallback(dateTime);
		sprintf(path,"%02d%02d%02d",year,month,day);
		if ( !SD.exists(String(path)) ) {
			delay(1000);
			SD.mkdir(path);
		}
		sprintf(fileName,"%s/%02d%02d%02d.txt",path,hour,min,sec);
		sprintf(dt,"%4d%02d%02d%02d%02d%02d",(year+2000),month,day,hour,min,sec);
	}

	File myFile = SD.open(fileName,FILE_WRITE);
	myFile.print(F("<trkpt lon=\""));
	myFile.print(String(lon,6));
	myFile.print(F("\" lat=\""));
	myFile.print(String(lat,6));
	myFile.println(F("\">"));
	myFile.print(F("<time>"));
	myFile.print(String(year+2000));
	myFile.print(F("-"));
	myFile.print(doubleDigitFix(month));
	myFile.print(F("-"));
	myFile.print(doubleDigitFix(day));
	myFile.print(F("T"));
	myFile.print(doubleDigitFix(hour));
	myFile.print(F(":"));
	myFile.print(doubleDigitFix(min));
	myFile.print(F(":"));
	myFile.print(doubleDigitFix(sec));
	myFile.println(F(".000Z</time></trkpt>"));
	myFile.close();
}

void loop() {
	if (processGPS()) {
		sendOnSD();
	  gsmSendHttp();
	}
}

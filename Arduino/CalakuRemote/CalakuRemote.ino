#include "Pummer.h"
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Structs.h>
// para pruebas

#define id 9 // duemi = 0 com5 | uno  = 1 com6 | mega = 2 com3

// -------

int const  calaknumb = 9;

//Declaremos los pines CE y el CSN
#define CE_PIN 7
#define CSN_PIN 8

//Variable con la dirección del canal por donde se va a transmitir
byte direccion[5] = { 'c','a','n','a','l' };

//creamos el objeto radio (NRF24L01)
RF24 radio(CE_PIN, CSN_PIN);

// creamos los objetos de los structs
DATOS rec[calaknumb], tempms, send; //variables para los datos a recivir, variable para almacnar lo recivido temporalmente y la de envio
LED led;

TMP timer1, timer2[calaknumb];

//el objeto pummer (el cual es utilizado para hacer los crossfades
Pummer pum(led.rp, led.gp, led.bp);

int msgn = 0;
int piezopin = 0, thr = 2;

void setup()
{	
	initMsgs();
	initPins();
	//inicializamos el NRF24L01 
	radio.begin();

	// inhabilitar los callbacks de ack
	radio.setAutoAck(false);

	//radio.enableDynamicAck();
	radio.setPALevel(RF24_PA_HIGH);
	//inicializamos el puerto serie
	Serial.begin(9600);
	
	//Abrimos un canal de escritura y lectura en el mismo addres con diferente pipe
	radio.openWritingPipe(direccion);
	radio.openReadingPipe(1, direccion);

	//establecemos la id desde la que se envia el mensaje
	send.ms.sender = id;

	timer1.antes = 1;

	//comenzamos a escuchar
	radio.startListening();
	Serial.println(F("empezando..."));
}

void loop()
{
	knocksens();
	checkIn();
	ligthCtrl();
}

void knocksens() {
	int val = analogRead(piezopin);
	timer1.ahora = millis();
	if ((val >= thr)&&((timer1.ahora-timer1.antes)>500)) {
		Serial.print("knock! ");
		Serial.println(val);
			sendMSG();
			timer1.antes = millis();
	}
}

void checkIn() {
	if (radio.available()) {
		// Variable for the received timestamp
		while (radio.available()) {                          // While there is data ready
			radio.read(&tempms, sizeof(tempms));             // Get the payload
		}
		Serial.print(F("recibiendo de "));
		Serial.println(tempms.ms.sender);
		for (int i = 0;i < calaknumb;i++) {		
			if (rec[i].ms.fin) {
				Serial.println(rec[i].ms.fin);
				rec[i] = tempms;
				rec[i].ms.fin = false;
				rec[i].ms.started = false;
				rec[i].ms.momento = millis();
				//debugeo
				Serial.print(i);
				Serial.print("  ");
				Serial.print(rec[i].ms.fin);
				Serial.print("  ");
				Serial.print(rec[i].ms.started);
				Serial.print(" sender: ");
				Serial.print(rec[i].ms.sender);
				Serial.print(" distancia: ");
				Serial.print(abs(rec[i].ms.sender - id));
				Serial.print(" | ");
				Serial.print(rec[i].ms.r);
				Serial.print("  ");
				Serial.print(rec[i].ms.g);
				Serial.print("  ");
				Serial.print(rec[i].ms.b);
				Serial.print("  ");
				Serial.println(rec[i].ms.momento);
				//----
				msgn += 1;
				Serial.println(msgn);
				break;
			}
		}
	}

}

void ligthCtrl() {

		for (int i = 0;i < calaknumb;i++) {
			if (!rec[i].ms.fin && !rec[i].ms.started)
			{ // PASO 1 si no ha acabado y no a empezado
				if ((millis() - rec[i].ms.momento) > (abs(rec[i].ms.sender - id))*led.off) { // si el tiempo que ha pasado desde la recepcion es el establecido para comenzar
					rec[i].ms.started = true; // inicimaos este y paramos todos los demas que esten empezados
					for (int a = 0;a < calaknumb;a++) {
						if ((!rec[a].ms.fin)&&(rec[a].ms.started)&&(i!=a)) {
							rec[a].ms.fin = true;
							rec[a].ms.started = false;
						}
					}
					Serial.println(" paso1 ");
					// establecemos el objetivo en el fader
					pum.goal(rec[i].ms.r, rec[i].ms.g, rec[i].ms.b, led.up);
					rec[i].ms.momento = millis();
				}
			}
			else if (rec[i].ms.started && !rec[i].ms.fin) {
				int lapso = millis() - rec[i].ms.momento;
				boolean donn = pum.done();
				if (lapso < led.up)
				{ //PASO 2 si ha empezado.. y si aun no han pasado los x ms de subida
					//Serial.println(" paso2 ");
					pum.loop();
				}

				else if ((lapso > led.up) && (lapso <led.up + led.on))
				{ //PASO 2.5 si ha empezado.. y si han pasado los x ms de subida pero aun no ha llegado a la bajada
					//Serial.println(" paso2.5 ");
					pum.goal(0,0,0,led.down);
				}
				
				else if (lapso > led.up + led.on && !donn)
				{ //PASO 3 si ya ha empezado.. ha pasado el tiempo de subida y el de on, ahora empiza la bajada
					//Serial.println(" paso3 ");
					pum.loop();
				}

				else if (lapso > led.up + led.on + led.down)
				{ //PASO 4 si ya ha pasado el tiempo de subida, on, y bajada establecemos el flag acabado a true
					Serial.println(" paso4 ");
					rec[i].ms.fin = true;
				}
			}
		}
}

void initMsgs() {
	for (int i = 0;i < calaknumb;i++) {
		rec[i].ms.started = false;
		rec[i].ms.fin = true;
	}
}

void initPins() {
	pinMode(led.rp, OUTPUT);
	pinMode(led.gp, OUTPUT);
	pinMode(led.bp, OUTPUT);
}


void sendMSG() {
	radio.stopListening();
	Serial.print(F("mandando "));
	Serial.println(id);
	send.ms.r = random(0, 255);
	send.ms.g = random(0, 255);
	send.ms.b = random(0, 255);
	if (!radio.write(&send, sizeof(send), true)) { //write
		Serial.println(F("failed"));
	}
	//radio.startWrite(&send, sizeof(send),true); // startwrite 
	radio.startListening();
	Serial.println(F("ya"));
}

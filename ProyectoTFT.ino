// Paint example specifically for the TFTLCD breakout board.
// If using the Arduino shield, use the tftpaint_shield.pde sketch instead!
// DOES NOT CURRENTLY WORK ON ARDUINO LEONARDO

#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>
#include <SPI.h>
#include <SD.h>

#if defined(__SAM3X8E__)
#undef __FlashStringHelper::F(string_literal)
#define F(string_literal) string_literal
#endif

File dafile;

// change this to match your SD shield or module;
//     Arduino Ethernet shield: pin 4
//     Adafruit SD shields and modules: pin 10
//     Sparkfun SD shield: pin 8
const int chipSelect = 10;




// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7

// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).
//   D0 connects to digital pin 22
//   D1 connects to digital pin 23
//   D2 connects to digital pin 24
//   D3 connects to digital pin 25
//   D4 connects to digital pin 26
//   D5 connects to digital pin 27
//   D6 connects to digital pin 28
//   D7 connects to digital pin 29

// For the Arduino Due, use digital pins 33 through 40
// (on the 2-row header at the end of the board).
//   D0 connects to digital pin 33
//   D1 connects to digital pin 34
//   D2 connects to digital pin 35
//   D3 connects to digital pin 36
//   D4 connects to digital pin 37
//   D5 connects to digital pin 38
//   D6 connects to digital pin 39
//   D7 connects to digital pin 40

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

#define TS_MINX 120
#define TS_MINY 90
#define TS_MAXX 890
#define TS_MAXY 910

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
// optional
#define LCD_RESET A4

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define AZUL_CADETE   0x5F9EA0


Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define BOXSIZE 15
#define PENRADIUS 3
int btnTamaniox = 30 + 40;
int btnTamanioy = 30;

int BTNX = 30 * 5 - 40;
int BTNY = 30 * 5 + 10;


int oldcolor, currentcolor;

int16_t oldposx = 0, currentposx = 0;
int16_t oldposy = 0, currentposy = 0;

boolean msg = true;
boolean btnInicio = false;
boolean PaintDraw = false;
boolean setPaint = false;

void setup(void) {

	//------------------------------------------------------
	//---------------Iniciando la pantalla tft -------------
	//------------------------------------------------------
	Serial.begin(9600);
	Serial.println(F("Paint!"));
	//----------------------------------------
#ifdef USE_Elegoo_SHIELD_PINOUT
	Serial.println(F("Using Elegoo 2.8\" TFT Arduino Shield Pinout"));
#else
	Serial.println(F("Using Elegoo 2.8\" TFT Breakout Board Pinout"));
#endif

	Serial.print("TFT size is ");
	Serial.print(tft.width()); Serial.print("x");
	Serial.println(tft.height());


	tft.reset();
	uint16_t identifier = 0x9341;
	tft.begin(identifier);
	tft.setRotation(1);

	//---------------------------------------------------
	//---------------Iniciando la tarjeta sd-------------
	//---------------------------------------------------

	Serial.print("Inicializando sd card... ");
	pinMode(chipSelect, OUTPUT);
	delay(2000);
	if (!SD.begin(chipSelect)) {
		Serial.print("Fallo la lectura de tarjeta bro ");
		delay(2000);
		return;
	}

	Serial.print("SD inicializada bro maquina si sale con 100 :v");

	reiniciarArchivo();
	//----------------------------------------------------
	//---------------Terminando la tarjeta sd-------------
	//----------------------------------------------------



}

#define MINPRESSURE 10
#define MAXPRESSURE 1000

void loop()
{

	if (!btnInicio) {

		if (msg) {
			testText();
			msg = false;
		}

		leerBtnInicio();

	}


	if (PaintDraw) {

		if (setPaint) {
			paint();
			setPaint = false;
		}

		canvas();
	}


}


void paint() {

	tft.setRotation(0);

	tft.fillScreen(WHITE);

	tft.fillRect(BOXSIZE * 0, 0, BOXSIZE * 3, BOXSIZE * 2, RED);
	tft.fillRect(BOXSIZE * 3, 0, BOXSIZE * 3, BOXSIZE * 2, BLACK);
	tft.fillRect(BOXSIZE * 6, 0, BOXSIZE * 3, BOXSIZE * 2, BLUE);
	tft.fillRect(BOXSIZE * 9, 0, tft.width() - BOXSIZE * 9, BOXSIZE * 2, CYAN);

	tft.setRotation(2);
	tft.setTextColor(BLACK);
	tft.setCursor(0, tft.height() - 45);
	tft.setTextSize(2);
	tft.println("");
	tft.println("IMPRMIR");
	tft.setRotation(0);


	currentcolor = RED;

	pinMode(13, OUTPUT);
}

unsigned long testText() {

	tft.fillScreen(BLACK);
	unsigned long start = micros();
	tft.setCursor(0, 0);
	tft.setTextColor(WHITE);
	tft.setTextSize(4);
	tft.println("   Grupo 6");
	tft.setTextSize(1.5);
	tft.println("");
	tft.println("Nombre: Edgar Jonathan Arrecis Martinez ");
	tft.println("Carnet: 201602633");
	tft.println("");
	tft.println("Nombre: Bryan Moises González Fuentes");
	tft.println("Carnet: 201602907");
	tft.println("");
	tft.println("Nombre: Carlos David Ramirez Altan ");
	tft.println("Carnet: 201213132");
	tft.println("");
	tft.println("Nombre: Romario David Castillo Echeverría ");
	tft.println("Carnet: 201314064");
	tft.println("");
	tft.setTextSize(2);
	tft.println("     Crear Dibujo     ");


	tft.fillRect(BTNX, BTNY, btnTamaniox, btnTamanioy, AZUL_CADETE);
	tft.drawRect(BTNX, BTNY, btnTamaniox, btnTamanioy, WHITE);

	tft.setCursor(BTNX, BTNY + 10);
	tft.setTextSize(1.5);
	tft.println(" Presioname");
	pinMode(13, OUTPUT);

	return micros() - start;
}

void leerBtnInicio() {

	digitalWrite(13, HIGH);
	TSPoint p = ts.getPoint();
	digitalWrite(13, LOW);


	// if sharing pins, you'll need to fix the directions of the touchscreen pins
	//pinMode(XP, OUTPUT);
	pinMode(XM, OUTPUT);
	pinMode(YP, OUTPUT);
	//pinMode(YM, OUTPUT);

	// we have some minimum pressure we consider 'valid'
	// pressure of 0 means no pressing!

	if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {

		// scale from 0->1023 to tft.width
		p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
		p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);

		if (p.y < BTNY) {

			if (p.x < BTNX) {
				tft.drawRect(BTNX, BTNY, btnTamaniox, btnTamanioy, BLUE);
				btnInicio = true;
				PaintDraw = true;
				setPaint = true;
			}
		}

	}


}

void canvas() {

	digitalWrite(13, HIGH);
	TSPoint p = ts.getPoint();
	digitalWrite(13, LOW);

	// if sharing pins, you'll need to fix the directions of the touchscreen pins
	//pinMode(XP, OUTPUT);
	pinMode(XM, OUTPUT);
	pinMode(YP, OUTPUT);
	//pinMode(YM, OUTPUT);

	// we have some minimum pressure we consider 'valid'
	// pressure of 0 means no pressing!

	if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {

		
		//Serial.print("X = "); Serial.print(p.x);
		//Serial.print("\tY = "); Serial.print(p.y);
		//Serial.print("\tPressure = "); Serial.println(p.z);
		

		if (p.y < (TS_MINY - 5)) {
			limpiarPantalla();
		}

		// scale from 0->1023 to tft.width
		p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
		p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);

		
		//Serial.print("("); Serial.print(p.x);
		//Serial.print(", "); Serial.print(p.y);
		//Serial.println(")");
		

		if (p.y < BOXSIZE * 2) {
			oldcolor = currentcolor;


			if (p.x < BOXSIZE * 3) {
				currentcolor = RED;
				tft.drawRect(BOXSIZE * 0, 0, BOXSIZE * 3, BOXSIZE * 2, AZUL_CADETE);
			}
			else if (p.x < BOXSIZE * 6) {
				currentcolor = BLACK;
				tft.drawRect(BOXSIZE * 3, 0, BOXSIZE * 3, BOXSIZE * 2, AZUL_CADETE);
			}
			else if (p.x < BOXSIZE * 9) {
				currentcolor = BLUE;
				tft.drawRect(BOXSIZE * 6, 0, BOXSIZE * 3, BOXSIZE * 2, AZUL_CADETE);
			}
			else if (p.x < BOXSIZE * 12) {

				//  currentcolor = CYAN;
				//  tft.drawRect(BOXSIZE * 9, 0, tft.width() - BOXSIZE * 9, BOXSIZE * 2, AZUL_CADETE);

				limpiarPantalla();
				leerPuntos();

			}

			if (oldcolor != currentcolor) {

				if (oldcolor == RED)  tft.fillRect(BOXSIZE * 0, 0, BOXSIZE * 3, BOXSIZE * 2, RED);
				if (oldcolor == YELLOW) tft.fillRect(BOXSIZE * 3, 0, BOXSIZE * 3, BOXSIZE * 2, BLACK);
				if (oldcolor == GREEN)  tft.fillRect(BOXSIZE * 6, 0, BOXSIZE * 3, BOXSIZE * 2, BLUE);

				//if (oldcolor == CYAN) {
				//	tft.fillRect(BOXSIZE * 9, 0, tft.width() - BOXSIZE * 9, BOXSIZE * 2, CYAN);
				//	tft.setRotation(2);
				//	tft.setTextColor(RED);
				//	tft.setCursor(0, tft.height() - 45);
				//	tft.setTextSize(2);
				//	tft.println("");
				//	tft.println("IMPRMIR");
				//	tft.setRotation(0);
				//}
			}


		}



		if (((p.y - PENRADIUS) > BOXSIZE * 2) && ((p.y + PENRADIUS) < tft.height())) {

			currentposx = p.x;
			currentposy = p.y;
			oldposx = currentposx;
			oldposy = currentposy;

			tft.fillCircle(p.x, p.y, PENRADIUS, currentcolor);

			//escrib
			if (((oldposx!=currentposx)&&(oldposy!=currentcolor))) {
				escribirPunto(p.x, p.y);
			}

			
		}


	}

}


void escribirPunto(int16_t x, int16_t y) {

	dafile = SD.open("prueba.txt", FILE_WRITE);


	if (dafile) {



		dafile.print(x);
		dafile.print(",");
		dafile.print(y);
		dafile.print(",");

		Serial.print("Escribio: ");
		Serial.print(x);
		Serial.print(",");
		Serial.print(y);
		Serial.print(",");


		if (currentcolor == RED) { dafile.print("R"); Serial.println("R"); }    //Rojo
		if (currentcolor == YELLOW) { dafile.print("N"); Serial.println("N"); } //Negro
		if (currentcolor == GREEN) { dafile.print("A"); Serial.println("A"); } //Azul

		dafile.println();

		dafile.close();


	}
	else {
		// if the file didn't open, print an error:
		Serial.println("error opening prueba.txt");
	}


}


void leerPuntos() {


	dafile = SD.open("prueba.txt", FILE_WRITE);

	if (dafile) {

		//*******************************************************
		//*************EMPEZAMOS A LEER EL ARCHIVO **************
		//*******************************************************
		Serial.println("prueba.txt->");

		while (dafile.available()) {
			Serial.write(dafile.read());
		}

		dafile.close();


		//*********************************************************
		//*************TERMINAMOS A LEER EL ARCHIVO ***************
		//*********************************************************


		eliminarFile();
		crearFile();
	}
	else {
		// if the file didn't open, print an error:
		Serial.println("error opening prueba.txt");
	}
}

void reiniciarArchivo() {

	// Check to see if the file exists: 
	if (SD.exists("prueba.txt")) {
		eliminarFile();
	}
	else {
		crearFile();
	}

}



void eliminarFile() {
	// delete the file:
	Serial.println("Removing prueba.txt...");
	SD.remove("prueba.txt");
}


void crearFile() {
	// open a new file and immediately close it:
	Serial.println("Creating prueba.txt...");
	dafile = SD.open("prueba.txt", FILE_WRITE);
	dafile.close();
}


void limpiarPantalla() {
	Serial.println("erase");
	// press the bottom of the screen to erase 
	tft.fillRect(0, BOXSIZE * 2, tft.width(), tft.height() - BOXSIZE * 2, WHITE);
}
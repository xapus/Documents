#ifndef structs_h
#define structs_h

struct TMP {
	unsigned long antes, ahora;
};

struct RGB {
	byte r = 0;
	byte g = 0;
	byte b = 0;
};

struct LED {
	//int r = 0, g = 0, b = 0;	// color actual
	RGB color;
	byte cr, cg, cb;
	int rp = 3, gp = 5, bp = 6; // pines
	int up = 500, down = 500; //tiempos de subida (deberian ser iguales para evitar complicar codigo)
	int off = 500, on = 100; //coeficientes de tiempo off y on
};

struct MSG {
	int sender, thr;
	unsigned long momento;
	byte r, g, b;
	bool started, fin;
};

union DATOS {
	MSG ms{				// revisar declaracion
		ms.fin = true
	};					//	---
	byte b[sizeof(MSG)];
};


#endif
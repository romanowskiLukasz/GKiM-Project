#include <exception>
#include <string.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <vector>
#include <algorithm>
#include <exception>
#include <string.h>

using namespace std;

SDL_Window* window = NULL;
SDL_Surface* screen = NULL;

#define szerokosc 512
#define wysokosc 340

#define tytul "GKiM2020 - Projekt Gr.8"


void setPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B);
SDL_Color getPixel (int x, int y);

void czyscEkran(Uint8 R, Uint8 G, Uint8 B);

int dodajKolor(SDL_Color kolor);
bool porownajKolory(SDL_Color k1, SDL_Color k2);
int sprawdzKolor(SDL_Color kolor);
vector<int> odcienieSzarosci(bool dithering);
vector<int> paletaNarzucona(bool dithering);
vector<int> paletaDedykowana(bool dithering);
static inline int pixel_distance(SDL_Color p1, SDL_Color p2);
static inline int closestMatch(SDL_Color pixel, const vector<SDL_Color>& clut);
vector<SDL_Color> medianCut(int palette_size);
int getCode(vector<vector<int>> Vseq, vector<int> v1);
vector<int> LZW_compression(vector<int> data);
vector<int> LZW_decompression(vector<int> data_comp);
void setPixelSurface(SDL_Surface* sfc,int x, int y, Uint8 R, Uint8 G, Uint8 B);
void zapiszBMP(char const* nazwa);

void Funkcja1();
void Funkcja2();
void Funkcja3();
void Funkcja4();
void Funkcja5();
void Funkcja6();
void Funkcja7();
void Funkcja8();
void Funkcja9();


SDL_Color paleta[(szerokosc/2)*(wysokosc/2)];
int ileKolorow = 0;
vector<SDL_Color> DColors ={};

vector<SDL_Color> skalaSzarosci
{
    {0,0,0},
    {17,17,17},
    {34,34,34},
    {51,51,51},
    {68,68,68},
    {85,85,85},
    {102,102,102},
    {119,119,119},
    {136,136,136},
    {153,153,153},
    {170,170,170},
    {187,187,187},
    {204,204,204},
    {221,221,221},
    {238,238,238},
    {255,255,255}
};

vector<SDL_Color> NPaleta
{
    {0,0,0},
    {0,0,255},
    {0,100,1},
    {30,144,255},
    {0,128,0},
    {135,206,250},
    {0,255,0},
    {0,255,255},
    {255,0,0},
    {255,0,255},
    {255,140,0},
    {255,105,180},
    {255,165,0},
    {255,192,203},
    {255,255,0},
    {255,255,255}
};

void Funkcja1() {
    Uint16 BWidth = szerokosc/2;
    Uint16 BHeight = wysokosc/2;
    bool Dithering = false;
    int Colormode = 0;
    int LData = 0;
    Uint8 options;
    char identyfikator[] = "krzm";
    ofstream wyjscie("obraz16.krzm", ios::binary);
    vector<int> dane_palety;
    cout << "Zapisywanie obrazu do pliku obraz16.krzm\n";
    cout << "Jaki tryb? ([0] skala szarosci | [1] paleta narzucona | [2] paleta dedykowana)\n> ";
    cin >> Colormode;
    cout << "Zastosowac dithering? ([0] nie | [1] tak)\n> ";
    cin >> Dithering;
    options |= (Colormode << 4);
    options |= Dithering;

    wyjscie.write((char*)&identyfikator, sizeof(char) * 4);
    wyjscie.write((char*)&options, sizeof(Uint8));
    wyjscie.write((char*)&BHeight, sizeof(Uint16));
    wyjscie.write((char*)&BWidth, sizeof(Uint16));

    switch (Colormode){
    case 0:
    {
        dane_palety = odcienieSzarosci(Dithering);
        break;
    }
    case 1:
    {
        dane_palety = paletaNarzucona(Dithering);
        break;
    }
    case 2:
    {
        dane_palety = paletaDedykowana(Dithering);
        break;
    }
    default:
        break;
    }



    cout << "Kompresuje..." << endl;
    vector<int> Data = LZW_compression(dane_palety);
    LData = Data.size();

    wyjscie.write((char*)&LData, sizeof(int));
    for(int i =0; i < LData; i++)
        wyjscie.write((char*)&Data[i], sizeof(Uint16));

    if(Colormode == 2)
        for(int i =0; i < 16; i++){
            wyjscie.write((char*)&DColors[i].r, sizeof(Uint8));
            wyjscie.write((char*)&DColors[i].g, sizeof(Uint8));
            wyjscie.write((char*)&DColors[i].b, sizeof(Uint8));
        }

    cout << "Zapisano." << endl << endl;
    SDL_UpdateWindowSurface(window);
}

void Funkcja2() {

    Uint16 BWidth = 0;
    Uint16 BHeight = 0;
    int Dithering = 0;
    int Colormode = 0;
    Uint8 options;
    char identyfikator[] = "    ";
    ifstream wejscie("obraz16.krzm", ios::binary);
    vector<int> dane_palety;
    vector<int> Data;
    int slowoLZW = 0;
    int LData = 0;
    SDL_Color kolor;

    cout << "Odczytywanie pliku obraz16.krzm" << endl;
    wejscie.read((char*)&identyfikator, sizeof(char)*4);
    wejscie.read((char*)&options, sizeof(Uint8));
    Colormode = (options >> 4);
    Dithering = (options & 00000001);

    wejscie.read((char*)&BHeight, sizeof(Uint16));
    wejscie.read((char*)&BWidth, sizeof(Uint16));
    wejscie.read((char*)&LData, sizeof(int));
    for(int i = 0; i<LData; i++){
        wejscie.read((char*)&slowoLZW, sizeof(Uint16));
        Data.push_back(slowoLZW);
    }
    cout << "Wysokosc obrazka: " << BHeight << endl;
    cout << "Szerokosc obrazka: " << BWidth << endl;
    cout << "Tryb obrazu: " << Colormode << endl;
    cout << "Dithering: " << Dithering << endl;

    if(Colormode == 2)
        for(int i = 0; i < 16; i++){
            wejscie.read((char*)&DColors[i].r, sizeof(Uint8));
            wejscie.read((char*)&DColors[i].g, sizeof(Uint8));
            wejscie.read((char*)&DColors[i].b, sizeof(Uint8));
    }

    dane_palety = LZW_decompression(Data);
    int k = 0;
    if(Colormode == 0){
        for (int y = 0; y < BHeight; y++) {
            for (int x = 0; x < BWidth; x++) {
                kolor = skalaSzarosci[dane_palety[k++]];
                setPixel(x + BWidth,y + BHeight, kolor.r, kolor.g, kolor.b);
            }
        }
    }else if(Colormode == 1){
        for (int y = 0; y < BHeight; y++) {
            for (int x = 0; x < BWidth; x++) {
                kolor = NPaleta[dane_palety[k++]];
                setPixel(x + BWidth,y + BHeight, kolor.r, kolor.g, kolor.b);
            }
        }
    }else{
        for (int y = 0; y < BHeight; y++) {
            for (int x = 0; x < BWidth; x++) {
                kolor = DColors[dane_palety[k++]];
                setPixel(x + BWidth,y + BHeight, kolor.r, kolor.g, kolor.b);
            }
        }
    }
    zapiszBMP("plik.bmp");
    SDL_UpdateWindowSurface(window);
}

void Funkcja3() {

    SDL_UpdateWindowSurface(window);
}

void Funkcja4() {

    SDL_UpdateWindowSurface(window);
}

void Funkcja5() {

    SDL_UpdateWindowSurface(window);
}

void Funkcja6() {

    SDL_UpdateWindowSurface(window);
}

void Funkcja7() {

    SDL_UpdateWindowSurface(window);
}

void Funkcja8() {

    //...

    SDL_UpdateWindowSurface(window);
}

void Funkcja9() {

    //...

    SDL_UpdateWindowSurface(window);
}

vector<int> paletaNarzucona(bool dithering)
{
     vector<int> dane_palety;

    if (dithering)
    {
        SDL_Color kolor;
        SDL_Color starykolor;

        float bledy[3][(szerokosc / 2) + 2][(wysokosc / 2) + 2];
        memset(bledy, 0, sizeof(bledy));

        int bladr = 0;
        int bladg = 0;
        int bladb = 0;
        int przesuniecie = 1; // nie wyjsc nizej (-1) bledow

        int R, G, B;

        for (int y = 0; y < wysokosc / 2; y++) {
            for (int x = 0; x < szerokosc / 2; x++) {
                starykolor = getPixel(x, y);

                R = starykolor.r;
                G = starykolor.g;
                B = starykolor.b;

                R += bledy[0][x + przesuniecie][y];
                G += bledy[1][x + przesuniecie][y];
                B += bledy[2][x + przesuniecie][y];

                if (R > 255) R = 255;
                if (G > 255) G = 255;
                if (B > 255) B = 255;
                if (R < 0) R = 0;
                if (G < 0) G = 0;
                if (B < 0) B = 0;

                starykolor.r = R;
                starykolor.g = G;
                starykolor.b = B;

                kolor = NPaleta[closestMatch(starykolor, NPaleta)];

                bladr = starykolor.r - kolor.r;
                bladg = starykolor.g - kolor.g;
                bladb = starykolor.b - kolor.b;

                int index_palety = closestMatch(kolor, NPaleta);
                dane_palety.push_back(index_palety);
                setPixel(x + szerokosc / 2, y, kolor.r, kolor.g, kolor.b);

                bledy[0][x + przesuniecie + 1][y] += (bladr * 7.0 / 16.0);
                bledy[0][x + przesuniecie + 1][y + 1] += (bladr * 1.0 / 16.0);
                bledy[0][x + przesuniecie][y + 1] += (bladr * 5.0 / 16.0);
                bledy[0][x + przesuniecie - 1][y + 1] += (bladr * 3.0 / 16.0);

                bledy[1][x + przesuniecie + 1][y] += (bladg * 7.0 / 16.0);
                bledy[1][x + przesuniecie + 1][y + 1] += (bladg * 1.0 / 16.0);
                bledy[1][x + przesuniecie][y + 1] += (bladg * 5.0 / 16.0);
                bledy[1][x + przesuniecie - 1][y + 1] += (bladg * 3.0 / 16.0);

                bledy[2][x + przesuniecie + 1][y] += (bladb * 7.0 / 16.0);
                bledy[2][x + przesuniecie + 1][y + 1] += (bladb * 1.0 / 16.0);
                bledy[2][x + przesuniecie][y + 1] += (bladb * 5.0 / 16.0);
                bledy[2][x + przesuniecie - 1][y + 1] += (bladb * 3.0 / 16.0);

            }
        }
    }
    else
    {
        SDL_Color kolor;
        for (int y = 0; y < wysokosc / 2; y++) {
            for (int x = 0; x < szerokosc / 2; x++) {
                kolor = getPixel(x, y);

                int index_palety = closestMatch(kolor, NPaleta);
                kolor = NPaleta[index_palety];
                dane_palety.push_back(index_palety);
                setPixel(x, y + wysokosc/2, kolor.r, kolor.g, kolor.b);
            }
        }
    }
    return dane_palety;
}

vector<int> odcienieSzarosci(bool dithering)
{
    vector<int> dane_palety;

    if (dithering)
    {
        SDL_Color kolor;
        SDL_Color starykolor;
        int BW = 0;

        float bledy[(szerokosc / 2) + 2][(wysokosc / 2) + 2];
        memset(bledy, 0, sizeof(bledy));

        int blad = 0;
        int przesuniecie = 1; // nie wyjsc nizej (-1) bledow

        for (int y = 0; y < wysokosc / 2; y++) {
            for (int x = 0; x < szerokosc / 2; x++) {
                starykolor = getPixel(x, y);

                BW = 0.229 * starykolor.r + 0.587 * starykolor.g + 0.114 * starykolor.b;
                BW += bledy[x + przesuniecie][y];

                if (BW > 255) BW = 255;
                if (BW < 0) BW = 0;

                starykolor.r = BW;
                starykolor.g = BW;
                starykolor.b = BW;

                kolor = skalaSzarosci[closestMatch(starykolor,skalaSzarosci)];
                blad = starykolor.r - kolor.r;
                int index_palety = closestMatch(kolor,skalaSzarosci);
                dane_palety.push_back(index_palety);

                setPixel(x + szerokosc / 2, y, kolor.r, kolor.g, kolor.b);

                bledy[x + przesuniecie + 1][y] += (blad * 7.0 / 16.0);
                bledy[x + przesuniecie + 1][y + 1] += (blad * 1.0 / 16.0);
                bledy[x + przesuniecie][y + 1] += (blad * 5.0 / 16.0);
                bledy[x + przesuniecie - 1][y + 1] += (blad * 3.0 / 16.0);
            }
        }
    }
    else
    {
        SDL_Color kolor;
        int BW = 0;
        for (int y = 0; y < wysokosc / 2; y++) {
            for (int x = 0; x < szerokosc / 2; x++) {
                kolor = getPixel(x, y);
                BW = 0.229 * kolor.r + 0.587 * kolor.g + 0.114 * kolor.b;
                kolor.r = BW;
                kolor.g = BW;
                kolor.b = BW;

                int index_palety = closestMatch(kolor, skalaSzarosci);
                kolor = skalaSzarosci[index_palety];
                dane_palety.push_back(index_palety);

                setPixel(x, y + wysokosc / 2, kolor.r, kolor.g, kolor.b);
            }
        }
    }
    return dane_palety;
}

vector<int> paletaDedykowana(bool dithering)
{
    vector<int> dane_palety;
    Uint16 szerokoscObrazka = szerokosc / 2;
    Uint16 wysokoscObrazka = wysokosc / 2;
    DColors = medianCut(16);

    if (dithering)
    {
        SDL_Color kolor;
        SDL_Color starykolor;

        float bledy[3][(szerokosc / 2) + 2][(wysokosc / 2) + 2];
        memset(bledy, 0, sizeof(bledy));

        int bladr = 0;
        int bladg = 0;
        int bladb = 0;
        int przesuniecie = 1; // nie wyjsc nizej (-1) bledow

        int R, G, B;
        for (int y = 0; y < wysokosc / 2; y++) {
            for (int x = 0; x < szerokosc / 2; x++) {

                starykolor = getPixel(x,y);
                R = starykolor.r;
                G = starykolor.g;
                B = starykolor.b;

                R += bledy[0][x + przesuniecie][y];
                G += bledy[1][x + przesuniecie][y];
                B += bledy[2][x + przesuniecie][y];

                if (R > 255) R = 255;
                if (G > 255) G = 255;
                if (B > 255) B = 255;
                if (R < 0) R = 0;
                if (G < 0) G = 0;
                if (B < 0) B = 0;

                starykolor.r = R;
                starykolor.g = G;
                starykolor.b = B;

                kolor = DColors[closestMatch(starykolor, DColors)];
                bladr = starykolor.r - kolor.r;
                bladg = starykolor.g - kolor.g;
                bladb = starykolor.b - kolor.b;

                int index_palety = closestMatch(kolor, DColors);
                dane_palety.push_back(index_palety);

                setPixel(x + szerokosc / 2, y, kolor.r, kolor.g, kolor.b);

                bledy[0][x + przesuniecie + 1][y] += (bladr * 7.0 / 16.0);
                bledy[0][x + przesuniecie + 1][y + 1] += (bladr * 1.0 / 16.0);
                bledy[0][x + przesuniecie][y + 1] += (bladr * 5.0 / 16.0);
                bledy[0][x + przesuniecie - 1][y + 1] += (bladr * 3.0 / 16.0);

                bledy[1][x + przesuniecie + 1][y] += (bladg * 7.0 / 16.0);
                bledy[1][x + przesuniecie + 1][y + 1] += (bladg * 1.0 / 16.0);
                bledy[1][x + przesuniecie][y + 1] += (bladg * 5.0 / 16.0);
                bledy[1][x + przesuniecie - 1][y + 1] += (bladg * 3.0 / 16.0);

                bledy[2][x + przesuniecie + 1][y] += (bladb * 7.0 / 16.0);
                bledy[2][x + przesuniecie + 1][y + 1] += (bladb * 1.0 / 16.0);
                bledy[2][x + przesuniecie][y + 1] += (bladb * 5.0 / 16.0);
                bledy[2][x + przesuniecie - 1][y + 1] += (bladb * 3.0 / 16.0);
            }
        }
    }
    else
    {
        for (int y = 0; y < wysokoscObrazka; y++) {
            for (int x = 0; x < szerokoscObrazka; x++) {
                SDL_Color kolor = getPixel(x,y);

                int index_palety = closestMatch(kolor, DColors);
                kolor = DColors[index_palety];
                dane_palety.push_back(index_palety);

                setPixel(x, y + wysokosc / 2, kolor.r, kolor.g, kolor.b);
            }
        }
    }
    return dane_palety;
}

static inline int pixel_distance(SDL_Color p1, SDL_Color p2) {
    int r1 = p1.r;
    int g1 = p1.g;
    int b1 = p1.b;

    int r2 = p2.r;
    int g2 = p2.g;
    int b2 = p2.b;
    return abs(r1 - r2) + abs(g1 - g2) + abs(b1 - b2);
}

static inline int closestMatch(SDL_Color pixel, const vector<SDL_Color>& clut) {
    int idx = 0;
    int current_distance = INT_MAX;
    for (int i = 0; i < clut.size(); ++i) {
        int dist = pixel_distance(pixel, clut.at(i));
        if (dist < current_distance) {
            current_distance = dist;
            idx = i;
        }
    }
    return idx;
}

vector<SDL_Color> medianCut(int palette_size)
{
    SDL_Color kolor;
    ileKolorow = 0;
    Uint16 szerokoscObrazka = szerokosc / 2;
    Uint16 wysokoscObrazka = wysokosc / 2;

    kolor = getPixel(0, 0);
    int lower_red = kolor.r,
        lower_green = kolor.g,
        lower_blue = kolor.b;
    int upper_red = 0,
        upper_green = 0,
        upper_blue = 0;

    for (int y = 0; y < wysokosc / 2; y++) {
        for (int x = 0; x < szerokosc / 2; x++) {
            kolor = getPixel(x, y);
            sprawdzKolor(kolor);
        }
    }

    if(ileKolorow<=palette_size){
            palette_size = ileKolorow;
    }

    for (int i = 0; i < ileKolorow; i++){
        lower_red = std::min(lower_red, (int)paleta[i].r);
        lower_green = std::min(lower_green, (int)paleta[i].g);
        lower_blue = std::min(lower_blue, (int)paleta[i].b);

        upper_red = std::max(upper_red, (int)paleta[i].r);
        upper_green = std::max(upper_green, (int)paleta[i].g);
        upper_blue = std::max(upper_blue, (int)paleta[i].b);
    }

    int red = upper_red - lower_red;
    int green = upper_green - lower_green;
    int blue = upper_blue - lower_blue;
    int max = std::max(std::max(red, green), blue);

    sort(paleta, paleta + ileKolorow, [max, red, green, blue](const SDL_Color& rgb1, const SDL_Color& rgb2) {
        if (max == red)  // if red is our color that has the widest range
            return rgb1.r < rgb2.r; // just compare their red channel
        else if (max == green) //...
            return rgb1.g < rgb2.g;
        else /*if (max == blue)*/
            return rgb1.b < rgb2.b;
        });

    int color_count = palette_size;
    vector<vector<SDL_Color>> lists;

    int list_size = ileKolorow / color_count;

    for (int i = 0; i < color_count; ++i) {
        vector<SDL_Color> listRGB;
        for (int j = list_size * i; j < (list_size * i) + list_size; ++j) {
            listRGB.push_back(paleta[j]);
        }
        lists.push_back(listRGB);
    }

    vector<SDL_Color> palette;
    for (vector<SDL_Color> list : lists) {
        palette.push_back(list[list.size() / 2]);
    }

    return palette;
}

void zapiszBMP(char const* nazwa)
{
    SDL_Surface* bmp = SDL_CreateRGBSurfaceWithFormat(0, szerokosc/2, wysokosc/2, 24, SDL_PIXELFORMAT_RGB24);

        SDL_Color kolor;
        for (int yy=0; yy<bmp->h; yy++) {
			for (int xx=0; xx<bmp->w; xx++) {
				kolor = getPixel(xx+szerokosc/2, yy+wysokosc/2);
				setPixelSurface(bmp, xx, yy, kolor.r, kolor.g, kolor.b);
			}
        }
        if(SDL_SaveBMP(bmp, nazwa) != 0)
        {
        // Error saving bitmap
        printf("SDL_SaveBMP failed: %s\n", SDL_GetError());
        }
        cout << "Zapisano bmp." << endl << endl;
		SDL_FreeSurface(bmp);
        SDL_UpdateWindowSurface(window);
}

int getCode(vector<vector<int>> Vseq, vector<int> v1)
{
	for (int i = 0; i < Vseq.size(); i++)
		if (Vseq[i] == v1)
			return i;
	return 0;
}

vector<int> LZW_compression(vector<int> data)
{
	vector<vector<int>> sequence;
	for (int i = 0; i <= 15; i++){
		vector<int> seq;
		seq.push_back(i);
		sequence.push_back(seq);
	}
	vector<int> p = {}, c = {};
	p.push_back(data[0]);
	vector<int> output_code;

	for (int i = 0; i < data.size(); i++) {

		if (i != data.size() - 1)
			c.push_back(data[i + 1]);

		vector<int> temp(p);
		temp.insert(temp.end(), c.begin(), c.end());

		bool alreadyIn = false;
		for (int k = 0; k < sequence.size(); k++)
			if (temp == sequence[k])
				alreadyIn = true;

		if (alreadyIn) {
			p = temp;
		}
		else {
			int pCode = getCode(sequence, p);
			output_code.push_back(pCode);
			sequence.push_back(temp);
			p = c;
		}
		c.clear();
	}
	int pCode = getCode(sequence, p);
	output_code.push_back(pCode);
	return output_code;
}


vector<int> LZW_decompression(vector<int> data_comp)
{
	vector<int> output_data;
	vector<vector<int>> sequence;

	for (int i = 0; i <= 15; i++)
	{
		vector<int> seq;
		seq.push_back(i);
		sequence.push_back(seq);
	}
	int old = data_comp[0], n;
	vector<int> s = sequence[old];
	vector<int> c = {};
	c.push_back(s[0]);
	output_data.insert(output_data.end(), s.begin(), s.end());

	for (int i = 0; i < data_comp.size() - 1; i++) {
		n = data_comp[i + 1];
		if (n > sequence.size() - 1){
			s = sequence[old];
			s.insert(s.end(), c.begin(), c.end());
		}
		else
			s = sequence[n];

		output_data.insert(output_data.end(), s.begin(), s.end());
		c.clear();
		c.push_back(s[0]);
		vector<int> temp(sequence[old]);
		temp.insert(temp.end(), c.begin(), c.end());
		sequence.push_back(temp);
		old = n;
	}
	return output_data;
}

int dodajKolor(SDL_Color kolor) {
	int aktualnyKolor = ileKolorow;
	paleta[aktualnyKolor] = kolor;
	ileKolorow++;
	return (aktualnyKolor);
}

bool porownajKolory(SDL_Color k1, SDL_Color k2) {
	bool wynik = true;
	if (k1.r != k2.r) wynik = false;
	if (k1.g != k2.g) wynik = false;
	if (k1.b != k2.b) wynik = false;
	return wynik;
}

int sprawdzKolor(SDL_Color kolor) {
	int wynik = -1;
	if (ileKolorow>0) {
		for (int k=0; k<ileKolorow; k++) {
			if (porownajKolory(kolor, paleta[k])) {
				wynik = k;
			}
		}
	}
	if (wynik<0) {
		wynik = dodajKolor(kolor);
	}
	return wynik;
}

void setPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B)
{
  if ((x>=0) && (x<szerokosc) && (y>=0) && (y<wysokosc))
  {
    /* Zamieniamy poszczególne składowe koloru na format koloru piksela */
    Uint32 pixel = SDL_MapRGB(screen->format, R, G, B);

    /* Pobieramy informację ile bajtów zajmuje jeden piksel */
    int bpp = screen->format->BytesPerPixel;

    /* Obliczamy adres piksela */
    Uint8 *p1 = (Uint8 *)screen->pixels + (y*2) * screen->pitch + (x*2) * bpp;
    Uint8 *p2 = (Uint8 *)screen->pixels + (y*2+1) * screen->pitch + (x*2) * bpp;
    Uint8 *p3 = (Uint8 *)screen->pixels + (y*2) * screen->pitch + (x*2+1) * bpp;
    Uint8 *p4 = (Uint8 *)screen->pixels + (y*2+1) * screen->pitch + (x*2+1) * bpp;

    /* Ustawiamy wartość piksela, w zależnoœci od formatu powierzchni*/
    switch(bpp)
    {
        case 1: //8-bit
            *p1 = pixel;
            *p2 = pixel;
            *p3 = pixel;
            *p4 = pixel;
            break;

        case 2: //16-bit
            *(Uint16 *)p1 = pixel;
            *(Uint16 *)p2 = pixel;
            *(Uint16 *)p3 = pixel;
            *(Uint16 *)p4 = pixel;
            break;

        case 3: //24-bit
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p1[0] = (pixel >> 16) & 0xff;
                p1[1] = (pixel >> 8) & 0xff;
                p1[2] = pixel & 0xff;
                p2[0] = (pixel >> 16) & 0xff;
                p2[1] = (pixel >> 8) & 0xff;
                p2[2] = pixel & 0xff;
                p3[0] = (pixel >> 16) & 0xff;
                p3[1] = (pixel >> 8) & 0xff;
                p3[2] = pixel & 0xff;
                p4[0] = (pixel >> 16) & 0xff;
                p4[1] = (pixel >> 8) & 0xff;
                p4[2] = pixel & 0xff;
            } else {
                p1[0] = pixel & 0xff;
                p1[1] = (pixel >> 8) & 0xff;
                p1[2] = (pixel >> 16) & 0xff;
                p2[0] = pixel & 0xff;
                p2[1] = (pixel >> 8) & 0xff;
                p2[2] = (pixel >> 16) & 0xff;
                p3[0] = pixel & 0xff;
                p3[1] = (pixel >> 8) & 0xff;
                p3[2] = (pixel >> 16) & 0xff;
                p4[0] = pixel & 0xff;
                p4[1] = (pixel >> 8) & 0xff;
                p4[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4: //32-bit
            *(Uint32 *)p1 = pixel;
            *(Uint32 *)p2 = pixel;
            *(Uint32 *)p3 = pixel;
            *(Uint32 *)p4 = pixel;
            break;

        }
    }
}

void setPixelSurface(SDL_Surface* sfc,int x, int y, Uint8 R, Uint8 G, Uint8 B)
{
  if ((x>=0) && (x<szerokosc*2) && (y>=0) && (y<wysokosc*2))
  {
    /* Zamieniamy poszczególne składowe koloru na format koloru piksela */
    Uint32 pixel = SDL_MapRGB(sfc->format, R, G, B);

    /* Pobieramy informację ile bajtów zajmuje jeden piksel */
    int bpp = sfc->format->BytesPerPixel;

    /* Obliczamy adres piksela */
    Uint8 *p = (Uint8 *)sfc->pixels + y * sfc->pitch + x * bpp;

    /* Ustawiamy wartość piksela, w zależności od formatu powierzchni*/
    switch(bpp)
    {
        case 1: //8-bit
            *p = pixel;
            break;

        case 2: //16-bit
            *(Uint16 *)p = pixel;
            break;

        case 3: //24-bit
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4: //32-bit
            *(Uint32 *)p = pixel;
            break;
        }
    }
}

SDL_Color getPixel(int x, int y) {
    SDL_Color color ;
    Uint32 col = 0 ;
    if ((x>=0) && (x<szerokosc) && (y>=0) && (y<wysokosc)) {
        //określamy pozycję
        char* pPosition=(char*)screen->pixels ;

        //przesunięcie względem y
        pPosition+=(screen->pitch*y*2) ;

        //przesunięcie względem x
        pPosition+=(screen->format->BytesPerPixel*x*2);

        //kopiujemy dane piksela
        memcpy(&col, pPosition, screen->format->BytesPerPixel);

        //konwertujemy kolor
        SDL_GetRGB(col, screen->format, &color.r, &color.g, &color.b);
    }
    return ( color ) ;
}

SDL_Color getPixelSurface(int x, int y, SDL_Surface *surface) {
    SDL_Color color ;
    Uint32 col = 0 ;
    if ((x>=0) && (x<szerokosc) && (y>=0) && (y<wysokosc)) {
        //określamy pozycję
        char* pPosition=(char*)surface->pixels ;

        //przesunięcie względem y
        pPosition+=(surface->pitch*y) ;

        //przesunięcie względem x
        pPosition+=(surface->format->BytesPerPixel*x);

        //kopiujemy dane piksela
        memcpy(&col, pPosition, surface->format->BytesPerPixel);

        //konwertujemy kolor
        SDL_GetRGB(col, surface->format, &color.r, &color.g, &color.b);
    }
    return ( color ) ;
}

void ladujBMP(char const* nazwa, int x, int y)
{
    SDL_Surface* bmp = SDL_LoadBMP(nazwa);
    if (!bmp)
    {
        printf("Unable to load bitmap: %s\n", SDL_GetError());
    }
    else
    {
        SDL_Color kolor;
        for (int yy=0; yy<bmp->h; yy++) {
			for (int xx=0; xx<bmp->w; xx++) {
				kolor = getPixelSurface(xx, yy, bmp);
				setPixel(xx, yy, kolor.r, kolor.g, kolor.b);
			}
        }
		SDL_FreeSurface(bmp);
        SDL_UpdateWindowSurface(window);
    }

}


void czyscEkran(Uint8 R, Uint8 G, Uint8 B)
{
    SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, R, G, B));
    SDL_UpdateWindowSurface(window);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
    }

    window = SDL_CreateWindow(tytul, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, szerokosc*2, wysokosc*2, SDL_WINDOW_SHOWN);

    if (window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    screen = SDL_GetWindowSurface(window);
    if (screen == NULL) {
        fprintf(stderr, "SDL_GetWindowSurface Error: %s\n", SDL_GetError());
    return false;
    }
    SDL_UpdateWindowSurface(window);


    bool done = false;
    SDL_Event event;
    // główna pętla programu
    while (SDL_WaitEvent(&event)) {
        // sprawdzamy czy pojawiło się zdarzenie
        switch (event.type) {
            case SDL_QUIT:
                done = true;
                break;

            // sprawdzamy czy został wciśnięty klawisz
            case SDL_KEYDOWN: {
                // wychodzimy, gdy wciśnięto ESC
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    done = true;
                if (event.key.keysym.sym == SDLK_1)
                    Funkcja1();
                if (event.key.keysym.sym == SDLK_2)
                    Funkcja2();
                if (event.key.keysym.sym == SDLK_3)
                    Funkcja3();
                if (event.key.keysym.sym == SDLK_4)
                    Funkcja4();
                if (event.key.keysym.sym == SDLK_5)
                    Funkcja5();
                if (event.key.keysym.sym == SDLK_6)
                    Funkcja6();
                if (event.key.keysym.sym == SDLK_7)
                    Funkcja7();
                if (event.key.keysym.sym == SDLK_8)
                    Funkcja8();
                if (event.key.keysym.sym == SDLK_9)
                    Funkcja9();
                if (event.key.keysym.sym == SDLK_a)
                    ladujBMP("obrazek1.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_s)
                    ladujBMP("obrazek2.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_d)
                    ladujBMP("obrazek3.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_f)
                    ladujBMP("obrazek4.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_g)
                    ladujBMP("obrazek5.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_h)
                    ladujBMP("obrazek6.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_j)
                    ladujBMP("obrazek7.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_k)
                    ladujBMP("obrazek8.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_b)
                    czyscEkran(0, 0, 10);
                else
                    break;
               }
        }
        if (done) break;
    }

    if (screen) {
        SDL_FreeSurface(screen);
    }

    if (window) {
        SDL_DestroyWindow(window);
    }


    SDL_Quit();
    return 0;
}

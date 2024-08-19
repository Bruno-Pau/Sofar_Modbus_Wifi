//////////////////////////////////////////////////////////////////////////////
//	Projet :             	ESCLAVE MODBUS            		B.VANNIER MAI 97
//	Fichiers du projet:
//                      trames_esclave.CPP   Construction des trames réponses
//////////////////////////////////////////////////////////////////////////////
// Ce fichier :	trames_esclave.H
//////////////////////////////////////////////////////////////////////////////
#ifndef trames_esclave_sofarH
#define trames_esclave_sofarH

#define			  MAXMOTS	0x280 //255 // taille de la table des mots MODBUS -1
#define			  MAXBITS	1023 // taille de la table des bits MODBUS -1
typedef unsigned char uchar;

// Références externes

class TESCLAVE
{
private:
	unsigned short int __fastcall CRC_16(uchar * tib, int n_oct);
    bool test_crc(uchar * tib, int n_oct); // Pour analyse trame reçue 19/06/01
    unsigned short int __fastcall mot(uchar fort, uchar faible){
        return(((fort<<8)&0xFF00) | (faible & 0xFF));}
    int __fastcall lect_bits(uchar *trame_emi, uchar *trame_rec, int lgtrame);  // 1 ou 2
    int __fastcall lect_mots(uchar *trame_emi, uchar *trame_rec, int lgtrame);  // 3 ou 4
    int __fastcall ecri_bits(uchar *trame_emi, uchar *trame_rec, int lgtrame);  // 15
    int __fastcall ecri_mots(uchar *trame_emi, uchar *trame_rec, int lgtrame);  // 16
    int __fastcall ecri_1mot(uchar *trame_emi, uchar *trame_rec, int lgtrame);  // 6
//    int       NumEsclave;
protected:
public:
    TESCLAVE();     // constructeur
    int __fastcall repond(uchar *trame_emi, uchar *trame_rec, int lgtrame);

    short int Itable[MAXMOTS + 1];	//	Tableau des mots MODBUS
    uchar   Btable[MAXBITS + 1];	//	Tableau des bits MODBUS
    void razTables();
};
#endif

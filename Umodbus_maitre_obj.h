//---------------------------------------------------------------------------

#ifndef Umodbus_maitre_objH
#define Umodbus_maitre_objH

#define uchar unsigned char
#define ushort unsigned short

#include <windows.h>
//---------------------------------------------------------------------------
// Classe MODBUS Maitre a appeler d'un autre module
//---------------------------------------------------------------------------
class TMODBUS_M {
  public:
    TMODBUS_M();                // Constructeur
    ~TMODBUS_M();               // Destructeur
    int init_com(char *szPort, int vit, int par, int timeout);    // ouverture port serie
    int close_com();                                             // fermeture port serie
    int lecture_mots(int nes, int ad, int nbmots, short *tabW);  // requete lecture mots
    int lecture_mots_bool(int nes, int ad, int nbmots, uchar *tabBool);
    int ecri_mots(int nes, int ad, int nbmots, short *tabW);     // requete ecriture mots
    int ecri_mots_bool(int nes, int ad, int nbmots, uchar *tabBool);
    char *Cerreur(int noer, char *StrCErr);                        // texte d'erreur

  private:
    HANDLE Hport;   // Handle de la liaison série
    char buff_out[512];
    char buff_in[512];

  public:
    unsigned short int CRC_16(uchar * tib, int n_oct);
    int trame_readbit(int adr, int nbit, int nes, uchar * Tabemi);
    int extrait_readbit(bool * valeur, uchar * Tabrec);
    int trame_readT(int adr, int nbmot, int nes, uchar * Tabemi);
    int trame_readT4(int adr, int nbmot, int nes, uchar * Tabemi);
    int extrait_readT(short * valeur, uchar * Tabrec);
    int trame_writebit(int adr, int nbit, int nes, bool * valeurb, uchar * Tabemi);
    int trame_writeT(int adr, int nbmot, int nes, short * valeur, uchar * Tabemi);

};

#endif

//---------------------------------------------------------------------------

#ifndef uModbus_SofarH
#define uModbus_SofarH

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
    unsigned short int CRC_16(uchar * tib, int n_oct);
    int trame_readbit(int adr, int nbit, int nes, uchar * Tabemi);
    int extrait_readbit(bool * valeur, uchar * Tabrec);
    int trame_readT(int adr, int nbmot, int nes, uchar * Tabemi);
    int trame_readT4(int adr, int nbmot, int nes, uchar * Tabemi);
    int extrait_readT(short * valeur, uchar * Tabrec);
    int trame_writebit(int adr, int nbit, int nes, bool * valeurb, uchar * Tabemi);
    int trame_writeT(int adr, int nbmot, int nes, short * valeur, uchar * Tabemi);
    int trame_writeT13(int adr, int nbmot, int nes, short * valeur, uchar * Tabemi);
    int trame_writeT30(int adr, int nbmot, int nes, uchar * Tabemi);
    int setWorkingMode(int nes, int mode, uchar * Tabemi);
    int enableEPS(int nes, bool bOn, uchar * Tabemi);
    int trame_writeXX(int codeFunc, int adr, int nbmot, int nes, short * valeur, uchar * Tabemi);
    int trame_write42(int adr, int nes, short valeur, uchar * Tabemi);
    int trame_writeHeartbeat(int nes, uchar * Tabemi);
    char *Cerreur(int noer, char *StrCErr);                        // texte d'erreur

  private:
    char buff_out[512];
    char buff_in[512];

  public:

};

#endif

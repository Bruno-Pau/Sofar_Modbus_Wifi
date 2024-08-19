//---------------------------------------------------------------------------
//  uModbus_Sofar.cpp : Unite MODBUS maitre        B.Vannier juill 2022
//------------------------------------------------------------------------
// Classe permettant de
//  - construire des trames maitre modbus
//  - d'interpreter la reponse de l'esclave
//
// les fonctions MODBUS suivantes sont implémentées
//      Lecture mots  : code 3 ou 4
//      Ecriture mots : code 16
//------------------------------------------------------------------------
// A terminer:
//  Lecture bits  : code 2
//  Ecriture bits : code 15
//  analyse trame retour fonctions  16: écriture bits et mots
//------------------------------------------------------------------------
// prototypes des fonctions a appeler:
//   char *Cerreur(int noer, char *StrCErr);    // Renvoit une chaine C
//
// les autres fonction (CRC_16()...) n'ont pas a etre appelees
//---------------------------------------------------------------------------
// Modifs:
//---------------------------------------------------------------------------

#include <windows.h>
#include "uModbus_Sofar.h"
#pragma argsused
#pragma hdrstop
//---------------------------------------------------------------------------
// Constructeur / Destructeur
//---------------------------------------------------------------------------
TMODBUS_M::TMODBUS_M()
{

}
//-------------------------------
TMODBUS_M::~TMODBUS_M()
{

}

//---------------------------------------------------------------------------
// Méthodes privées
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  CRC_16()
//---------------------------------------------------------------------------
unsigned short int TMODBUS_M::CRC_16(uchar * tib, int n_oct)
{
    int n,retenue=0,bit;
    unsigned int crc=0xffff;

    for(n=0;n<n_oct;n++)
    {
        crc=(crc ^ tib[n]);         /* XOR entre crc et l'octet */
        for(bit=0;bit<8;bit++)
        {
            retenue=(crc & 0x1);   /* retenue du decalage suivant */
            crc=(crc >> 1);       /* DECALAGE A DROITE DE 1 BIT  */
            crc=crc & 0x7fff;
            if (retenue==1)
            {
                crc=(crc ^ 0xa001);   /* XOR entre crc et polynome  */
            }
        }
    }
    return((short)crc);
}
//----------------------------------------------------------------------------
// Fonctions publiques
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// trame_readBit : Fonction 2 modbus, construction trame lecture de bits
//----------------------------------------------------------------------------
int TMODBUS_M::trame_readbit(int adr, int nbit, int nes, uchar * Tabemi)
{

  unsigned int crc;

//  if (nbit > 240) return (-5); // trop de bits demandés!

  Tabemi[0] = (uchar) (nes & 0xFF);       // No d'esclave
  Tabemi[1] = 2;                // No de fonction
  Tabemi[2] = (uchar) ((adr>>8) & 0xFF);  // adresse
  Tabemi[3] = (uchar) (adr & 0xFF);
  Tabemi[4] = (uchar) ((nbit>>8) & 0xFF);// nb de bits
  Tabemi[5] = (uchar) (nbit & 0xFF);
  crc = CRC_16 (Tabemi, 6);
  Tabemi[6] = (uchar) (crc & 0xFF);
  Tabemi[7] = (uchar) ((crc >>8) & 0xFF);

  return(8);        // nb d'octets de la trame
}
//----------------------------------------------------------------------------
// extrait_readbit : Fonction 2 modbus, analyse de la trame retour
//----------------------------------------------------------------------------
int TMODBUS_M::extrait_readbit(bool * valeur, uchar * Tabrec)
{
  unsigned short int crc;
  int  nbit, index_crc, i, j, requete;
  unsigned char octet;

  requete = Tabrec[1];
  if ((requete == 1 ) || (requete == 2)) {
    nbit = (Tabrec[2]) << 3;    // nbits = nb d'octets x 8
    for (i=0; i<Tabrec[2]; i++){   // octet par octet
      octet = (Tabrec[i+3]) & 0xFF;
      for (j=0; j<8; j++) {             // puis bit par bit
        if (((octet >> j) & 1) != 0) valeur[(i << 3) + j] = true;
                                else valeur[(i << 3) + j] = false;
      }
    }
//-------- Test du CRC
    index_crc = Tabrec[2] + 3;
    crc = CRC_16(Tabrec,index_crc);
    if ((Tabrec[index_crc] == (uchar)(crc & 0xFF))
        && (Tabrec[index_crc+1] == (uchar)((crc>>8)& 0xFF)))
      return(nbit);
    else  return(-4);
  }
  else return(-3);
}
//---------------------------------------------------------------------------
// trame_readT : Fonction 3 modbus, construction trame lecture de plusieurs mots
//----------------------------------------------------------------------------
int TMODBUS_M::trame_readT(int adr, int nbmot, int nes, uchar * Tabemi)
{
  unsigned int crc;

  Tabemi[0] = (uchar) (nes & 0xFF);       // No d'esclave
  Tabemi[1] = 3;                // No de fonction
  Tabemi[2] = (uchar) ((adr>>8) & 0xFF);  // adresse
  Tabemi[3] = (uchar) (adr & 0xFF);
  Tabemi[4] = (uchar) ((nbmot>>8) & 0xFF);// nb de mots
  Tabemi[5] = (uchar) (nbmot & 0xFF);
  crc = CRC_16 (Tabemi, 6);
  ushort *pusCrc = (ushort*)&Tabemi[6]; // recup adresse de Tabemi[6]
  *pusCrc = crc;                        // affectation du ushort
  return(8);               // retourne le nb d'octets de la trame
}
//---------------------------------------------------------------------------
// trame_readT : Fonction 4 modbus, construction trame lecture de plusieurs mots
//----------------------------------------------------------------------------
int TMODBUS_M::trame_readT4(int adr, int nbmot, int nes, uchar * Tabemi)
{
  unsigned int crc;

  Tabemi[0] = (uchar) (nes & 0xFF);       // No d'esclave
  Tabemi[1] = 4;                // No de fonction
  Tabemi[2] = (uchar) ((adr>>8) & 0xFF);  // adresse
  Tabemi[3] = (uchar) (adr & 0xFF);
  Tabemi[4] = (uchar) ((nbmot>>8) & 0xFF);// nb de mots
  Tabemi[5] = (uchar) (nbmot & 0xFF);
  crc = CRC_16 (Tabemi, 6);
  ushort *pusCrc = (ushort*)&Tabemi[6]; // recup adresse de Tabemi[6]
  *pusCrc = crc;                        // affectation du ushort
  return(8);               // retourne le nb d'octets de la trame
}
//---------------------------------------------------------------------------
// extrait_readT : Fonction 3 ou 4 modbus, analyse de la trame retour
//----------------------------------------------------------------------------
int TMODBUS_M::extrait_readT(short * valeur, uchar * Tabrec)
{
  unsigned short int crc;
  int  nbmot, index_crc, i, requete;

  requete = Tabrec[1];
  if ((Tabrec[0] == 5 ) && (Tabrec[2] == 0 ) ) {
    return (-5);
  }

  if (requete >= 0x80 ) {
    return (-requete);
  }

  if ((requete == 4 ) || (requete == 3)) {
    nbmot = Tabrec[2] >> 1;    // nombre de mots lus = nb octets / 2
    for (i= 0; i<nbmot; i++) {
      valeur[i] = (Tabrec[(i<<1)+4]) | ((short)(Tabrec[(i<<1)+3]) << 8);
    }
//-------- Test du CRC
    index_crc = Tabrec[2] + 3;
    crc = CRC_16(Tabrec,index_crc);
    if ((Tabrec[index_crc] == (uchar)(crc & 0xFF))
        && (Tabrec[index_crc+1] == (uchar)((crc>>8)& 0xFF)))
      return(nbmot);
    else  return(-4);
  }
  else return(requete);
}
//----------------------------------------------------------------------------
// trame_writebit : Fonction 15 modbus, construction trame écriture de plusieurs bits
//----------------------------------------------------------------------------
int TMODBUS_M::trame_writebit(int adr, int nbit, int nes, bool * valeurb, uchar * Tabemi)
{
  unsigned short int crc;
  int i, j, nboctet;
  uchar octet;

//  if ((adr + nbit) > 256) then return(-5); // le TSS 37 n'accepte que 256 bits max !
  Tabemi[0] = (uchar) (nes & 0xFF);       // No d'esclave
  Tabemi[1] = 15;                // No de fonction 15 écriture bits
  Tabemi[2] = (uchar) ((adr>>8) & 0xFF);  // adresse
  Tabemi[3] = (uchar) (adr & 0xFF);
  Tabemi[4] = (uchar) ((nbit>>8) & 0xFF);// nb de mots
  Tabemi[5] = (uchar) (nbit & 0xFF);
  nboctet   = (nbit + 7) >> 3;         // nb d'octets utiles
  Tabemi[6] = (uchar) nboctet ;        // nb d'octets


  for (i=0; i<nboctet; i++) {   // octet par octet
    octet = 0;
    for (j=0; j<8; j++){        // puis bit par bit
      if ((valeurb [(i << 3) +j])!=0) octet = octet | (1 << j);
    }
    Tabemi[i + 7] = (uchar)octet;
  }

  nboctet = 7 + nboctet;        // nb d'octets sans CRC
  crc = CRC_16 (Tabemi, nboctet);
  Tabemi[nboctet] = (uchar)(crc & 0xFF);
  Tabemi[++nboctet] = (uchar)((crc >> 8) & 0xFF);
  return(++nboctet);    // retourne le nb d'octets de la trame avec CRC
}
//----------------------------------------------------------------------------
// trame_writeT : Fonction 16 modbus, construction trame écriture de plusieurs mots
//----------------------------------------------------------------------------
int TMODBUS_M::trame_writeT(int adr, int nbmot, int nes, short * valeur, uchar * Tabemi)
{
  unsigned short int crc;
  int i, nboctet;

  Tabemi[0] = (uchar) (nes & 0xFF);       // No d'esclave
  Tabemi[1] = 16;                // No de fonction
  Tabemi[2] = (uchar) ((adr>>8) & 0xFF);  // adresse
  Tabemi[3] = (uchar) (adr & 0xFF);
  Tabemi[4] = (uchar) ((nbmot>>8) & 0xFF);// nb de mots
  Tabemi[5] = (uchar) (nbmot & 0xFF);
  Tabemi[6] = (uchar) (nbmot << 1);       // nb d'octets

  for (i=0; i<nbmot; i++){
    Tabemi[7 + (i<<1)] = (uchar) ((valeur[i]>>8) & 0xFF);
    Tabemi[8 + (i<<1)] = (uchar)(valeur[i] & 0xFF);
  }
  nboctet = 7 + (nbmot<<1);
  crc = CRC_16 (Tabemi, nboctet);
  Tabemi[nboctet] = (uchar)(crc & 0xFF);
  Tabemi[++nboctet] = (uchar)((crc >> 8) & 0xFF);
  return(++nboctet);    // retourne le nb d'octets de la trame
}
//----------------------------------------------------------------------------
// trame_writeXX : construction trame écriture de plusieurs mots
//----------------------------------------------------------------------------
int TMODBUS_M::trame_writeXX(int codeFunc, int adr, int nbmot, int nes, short * valeur, uchar * Tabemi)
{
  unsigned short int crc;
  int i, nboctet;

  Tabemi[0] = (uchar) (nes & 0xFF);       // No d'esclave
  Tabemi[1] = codeFunc;                // No de fonction
  Tabemi[2] = (uchar) ((adr>>8) & 0xFF);  // adresse
  Tabemi[3] = (uchar) (adr & 0xFF);
  Tabemi[4] = (uchar) ((nbmot>>8) & 0xFF);// nb de mots
  Tabemi[5] = (uchar) (nbmot & 0xFF);

  int debVal = 6; // debut des valeurs a ecrire

  if ( codeFunc == 0x10 ) {
    Tabemi[6] = (uchar) (nbmot << 1);       // nb d'octets si code 0x10
    debVal = 7;
  }
  for (i=0; i<nbmot; i++){
    Tabemi[debVal + (i<<1)] = (uchar) ((valeur[i]>>8) & 0xFF);
    Tabemi[debVal + 1 + (i<<1)] = (uchar)(valeur[i] & 0xFF);
  }
  nboctet = debVal + (nbmot<<1);
  crc = CRC_16 (Tabemi, nboctet);
  Tabemi[nboctet] = (uchar)(crc & 0xFF);
  Tabemi[++nboctet] = (uchar)((crc >> 8) & 0xFF);
  return(++nboctet);    // retourne le nb d'octets de la trame
}
//----------------------------------------------------------------------------
// trame_writeT13 : Fonction 0x13 , construction trame écriture de plusieurs mots
//----------------------------------------------------------------------------
int TMODBUS_M::trame_writeT13(int adr, int nbmot, int nes, short * valeur, uchar * Tabemi)
{
  return ( this->trame_writeXX(0x13, adr, nbmot, nes, valeur, Tabemi) );
}
//----------------------------------------------------------------------------
// trame_writeT30 : Fonction 30 Sofar, Clear to factory settings
//----------------------------------------------------------------------------
int TMODBUS_M::trame_writeT30(int adr, int nbmot, int nes, uchar * Tabemi)
{
  unsigned short int crc;
  int i, nboctet;

  Tabemi[0] = (uchar) (nes & 0xFF);       // No d'esclave
  Tabemi[1] = 0x30;                // No de fonction
  Tabemi[2] = (uchar) ((adr>>8) & 0xFF);  // adresse
  Tabemi[3] = (uchar) (adr & 0xFF);
  Tabemi[4] = (uchar) ((nbmot>>8) & 0xFF);// nb de mots
  Tabemi[5] = (uchar) (nbmot & 0xFF);

  nboctet = 6;
  crc = CRC_16 (Tabemi, nboctet);
  Tabemi[nboctet] = (uchar)(crc & 0xFF);
  Tabemi[++nboctet] = (uchar)((crc >> 8) & 0xFF);
  return(++nboctet);    // retourne le nb d'octets de la trame
}
//----------------------------------------------------------------------------
// enableEPS() : Set the EPS enable bit ON
//----------------------------------------------------------------------------
int TMODBUS_M::enableEPS(int nes, bool bOn, uchar * Tabemi)
{
  unsigned short int crc;
  int i, nboctet;

  // 0x 01 41 00 00 00 01 00 55 41 3C

  Tabemi[0] = (uchar) (nes & 0xFF);       // No d'esclave
  Tabemi[1] = 0x41;                // No de fonction
  Tabemi[2] = 0x00;
  Tabemi[3] = 0x00;
  Tabemi[4] = 0x00;
  Tabemi[5] = 0x01;
  Tabemi[6] = 0x00;
  if ( bOn ) Tabemi[7] = 0x55;
  else Tabemi[7] = 0xAA;

  nboctet = 8 ;
  crc = CRC_16 (Tabemi, nboctet);
  Tabemi[nboctet] = (uchar)(crc & 0xFF);
  Tabemi[++nboctet] = (uchar)((crc >> 8) & 0xFF);
  return(++nboctet);    // retourne le nb d'octets de la trame
}
//----------------------------------------------------------------------------
// setWorkingMode() : Set the Working Mode
//----------------------------------------------------------------------------
int TMODBUS_M::setWorkingMode(int nes, int mode, uchar * Tabemi)
{
  unsigned short int crc;
  int i, nboctet;

  // 0x 01 42 00 00 00 01 00 55 41 3C  : plus utilise
  // 0x 01 10 00 12 00 00 01 02 00 03 xx xx

  Tabemi[0] = (uchar) (nes & 0xFF);       // No d'esclave
  //Tabemi[1] = 0x42;                // No de fonction
  Tabemi[1] = 0x10;                // No de fonction
  Tabemi[2] = 0x12;                // ad
  Tabemi[3] = 0x00;
  Tabemi[4] = 0x00;                // nb mots
  Tabemi[5] = 0x01;
  Tabemi[6] = 0x02;                // nb octets
  //if ( bOn ) Tabemi[7] = 0x55;
  //else Tabemi[7] = 0xAA;
  Tabemi[7] = 0;
  Tabemi[8] = mode & 0xFF;

  nboctet = 9 ;
  crc = CRC_16 (Tabemi, nboctet);
  Tabemi[nboctet] = (uchar)(crc & 0xFF);
  Tabemi[++nboctet] = (uchar)((crc >> 8) & 0xFF);
  return(++nboctet);    // retourne le nb d'octets de la trame
}
//----------------------------------------------------------------------------
// trame_write42 : Fonction 42 modbus,  écriture 1 mots  (SOFAR)
//----------------------------------------------------------------------------
int TMODBUS_M::trame_write42(int adr, int nes, short valeur, uchar * Tabemi)
{
  unsigned short int crc;
  int i, nboctet;

  // 0x 01 42  01 00  55 55  87 56  get the standby command, into standby mode
  // 0x 01 42  01 01  01 F4  29 EE  battery Discharge 0.5 kW
  // 0x 01 42  01 02  01 F4  D9 EE  battery Charge

  Tabemi[0] = (uchar) (nes & 0xFF);       // No d'esclave
  Tabemi[1] = 0x42;                // No de fonction
  Tabemi[2] = (uchar) ((adr>>8) & 0xFF);  // adresse
  Tabemi[3] = (uchar) (adr & 0xFF);
  Tabemi[4] = (uchar)  ((valeur>>8) & 0xFF);    // valeur poids forts
  Tabemi[5] = (uchar)  ((valeur) & 0xFF);       // valeur poids faibles

  nboctet = 6 ;
  crc = CRC_16 (Tabemi, nboctet);
  Tabemi[nboctet] = (uchar)(crc & 0xFF);
  Tabemi[++nboctet] = (uchar)((crc >> 8) & 0xFF);
  return(++nboctet);    // retourne le nb d'octets de la trame
}

//----------------------------------------------------------------------------
// trame_writeHeartbeat : Fonction 49 modbus,  écriture battement du coeur (SOFAR)
//----------------------------------------------------------------------------
int TMODBUS_M::trame_writeHeartbeat(int nes, uchar * Tabemi)
{
  unsigned short int crc;
  int i, nboctet;

  // 0x 01 49 22 01 22 02 1E DD

  Tabemi[0] = (uchar) (nes & 0xFF);       // No d'esclave
  Tabemi[1] = 0x49;                // No de fonction
  Tabemi[2] = 0x22;  // adresse
  Tabemi[3] = 0x01;
  Tabemi[4] = 0x22;    // valeur poids forts
  Tabemi[5] = 0x02;       // valeur poids faibles

  nboctet = 6 ;
  crc = CRC_16 (Tabemi, nboctet);
  Tabemi[nboctet] = (uchar)(crc & 0xFF);
  Tabemi[++nboctet] = (uchar)((crc >> 8) & 0xFF);
  return(++nboctet);    // retourne le nb d'octets de la trame
}

//---------------------------------------------------------------------------
//  Renvoit une chaine C correspondant à un numéro d'erreur
//---------------------------------------------------------------------------
char *TMODBUS_M::Cerreur(int noer, char *StrCErr)
{
  char origine[30]="", texte[100]="!!!";    //"non referencee";
  int er2; // sous classement d'erreur

  if (noer == 0) {
    strcpy(StrCErr, "fonction executée correctement");
    return (StrCErr);
  }

  if ((noer < -19) && (noer>-30)) { strcpy(origine, "Erreur Lecture mots: "); er2 = noer+20;}
  if ((noer < -29) && (noer>-40)) { strcpy(origine, "Erreur Ecriture mots: "); er2 = noer+30;}

  if ((noer < -39) && (noer>-50)) { strcpy(origine, "Erreur Lecture bits: "); er2 = noer+40;}
  if ((noer < -49) && (noer>-60)) { strcpy(origine, "Erreur Ecriture bits: "); er2 = noer+50;}

  switch (er2) {
    case 0  : strcpy(texte, "no esclave < 1 ou > 254"); break;
    case -1 : strcpy(texte, "adresse mot < 0 ou > 32767"); break;
    case -2 : strcpy(texte, "nb de mots > 128"); break;
    //case -3 : strcpy(texte, "erreur WriteFile sur port COM"); break;
    //case -4 : strcpy(texte, "erreur ReadFile sur port COM"); break;
    //case -5 : strcpy(texte, "pas de réponse API"); break;
    case -6 : strcpy(texte, "trame retour incorrecte"); break;
  }

  strcpy (StrCErr, origine);
  strcat (StrCErr, texte);
  
  return (StrCErr);
}


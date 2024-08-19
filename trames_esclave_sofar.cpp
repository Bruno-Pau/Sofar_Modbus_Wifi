//---------------------------------------------------------------------------
//	Projet :         SIMULATION	ESCLAVE MODBUS             	B.VANNIER nov 22
//	Fichiers du projet:
// Ce fichier :	trames_esclave_sofar.CPP
//---------------------------------------------------------------------------
//  Auteur  : B.Vannier
//	 Modifs :
//     19/06/01 : bool test_crc(uchar * tib, int n_oct); // analyse trame reçue
//  Simule un esclave MODBUS:
// - analyse des trames et construction d'une trame réponse
// - Renvoi des trames réponse sur le port série spécifié (vers le maître)
//   les trames d'écriture et lecture agissent sur une table de 256 mots
//   Codes Traités : 1 ou 2 lecture bits    modif du 5 juil 2000
//                   3 ou 4 lecture mots
//                   6 écriture 1 mot
//                  15 écriture bits        modif du 5 juil 2000
//                  16 écriture plusieurs mots
//
//---------------------------------------------------------------------------
// Modifs:
//   5 juil 2000 : codes Modbus 1,2 et 15  dans trames_esclave.cpp
//  19 juin 2001 : rajout controle CRC sur trames reçues
//   8 nov  2022 : rajout razTables()
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "trames_esclave_sofar.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
TESCLAVE::TESCLAVE()     // constructeur
{
  short int i;
  for (i=0; i<=MAXMOTS; i++) Itable[i] = i + 512;
}
//---------------------------------------------------------------------------
unsigned short int __fastcall TESCLAVE::CRC_16(uchar * tib, int n_oct)
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
//---------------------------------------------------------------------------
void TESCLAVE::razTables()
{
  memset(Itable, 0, sizeof(Itable) );
  memset(Btable, 0, sizeof(Btable) );
}
//---------------------------------------------------------------------------
 // Pour analyse trame reçue 19/06/01
bool TESCLAVE::test_crc(uchar * tib, int n_oct)
{
    unsigned short int crc2 = CRC_16( tib, n_oct-2);
    if (crc2 != mot( tib[n_oct-1], tib[n_oct-2])){
        return(false);
    }
    else return (true);

}
//---------------------------------------------------------------------------
//   lect_bits() : construit la trame réponse treme_emi pour la requète lecture bits 01
//---------------------------------------------------------------------------
int __fastcall TESCLAVE::lect_bits(uchar *trame_emi, uchar *trame_rec, int lgtrame)
{
    int i, j, ad, nbbits, nb_octets;
    short int valeur, crc;
    uchar octet;

    if (lgtrame != 8) return(-1);
    ad = mot(trame_rec[2], trame_rec[3]);
    nbbits = mot(trame_rec[4], trame_rec[5]);
    nb_octets = (nbbits + 7) >> 3 ;
    if (nbbits < 1) return (-2);
    if ((ad < 0) || (ad + nbbits > 1023)) return(-3);
    trame_emi[0] = trame_rec[0];  // n° esclave
    trame_emi[1] = trame_rec[1];  // code fonction
    trame_emi[2] = (uchar)nb_octets;   // nb d'octets
    for (i=0; i < nb_octets; i++){   // octet par octet
        octet = 0;
        for (j=1; j<0x100; j <<= 1) {             // puis bit par bit
            if (Btable[ad++]!=0) octet |= j;       // mise a 1 du bit ?
        }
        trame_emi[3 + i] = octet;  
    }
    // raz des derniers bits non demandés
    int nb_bits_non_demandes = (nb_octets << 3) - nbbits;
    for (i=0, j=0xFF7F; i < nb_bits_non_demandes; i++, j>>=1){
        trame_emi[2 + nb_octets] &= (uchar)j;
    }

    int nb_o = nb_octets + 3;  // nb d'octets de la trame
    crc = CRC_16(trame_emi, nb_o);
    trame_emi[nb_o++] = (uchar)(crc & 0xFF);
    trame_emi[nb_o++] = (uchar)((crc  & 0xFF00) >> 8);
    return(nb_o);
}
//---------------------------------------------------------------------------
//   lect_mots() : construit la trame réponse treme_emi pour la requète lecture mot
//---------------------------------------------------------------------------
int __fastcall TESCLAVE::lect_mots(uchar *trame_emi, uchar *trame_rec, int lgtrame)
{
  int i, ad, nbmots, no_octet;
  short int valeur, crc;

  if (lgtrame != 8) return(-1);
  ad = mot(trame_rec[2], trame_rec[3]);
  nbmots = mot(trame_rec[4], trame_rec[5]);
  if (nbmots < 1) return (-2);
  if ((ad < 0) || (ad + nbmots > MAXMOTS)) return(-3);
  trame_emi[0] = trame_rec[0];  // n° esclave
  trame_emi[1] = trame_rec[1];  // code fonction
  trame_emi[2] = (uchar)(nbmots << 1);   // nb d'octets lus
  no_octet = 3;
  for (i=0; i<nbmots; i++) {
    valeur = Itable[ad + i];
    trame_emi[no_octet++] = (uchar)((valeur & 0xFF00) >> 8); // poids forts
    trame_emi[no_octet++] = (uchar)(valeur & 0xFF);          // poids faibles
  }
  crc = CRC_16(trame_emi, no_octet);
  trame_emi[no_octet++] = (uchar)(crc & 0xFF);
  trame_emi[no_octet++] = (uchar)((crc  & 0xFF00) >> 8);
  return(no_octet);
}
//---------------------------------------------------------------------------
//  ecri_bits  : ecriture dans Btable (table des bits MODBUS)   code 15
//               et construction de la trame émission
//---------------------------------------------------------------------------
int __fastcall TESCLAVE::ecri_bits(uchar *trame_emi, uchar *trame_rec, int lgtrame)
{
    int i, j, ad, nbbits, nb_octets;
    short int crc;
    uchar octet;

    if (lgtrame < 8) return(-1);
    ad = mot(trame_rec[2], trame_rec[3]);
    nbbits = mot(trame_rec[4], trame_rec[5]);
    nb_octets = (nbbits +7) >> 3;
    if (nbbits < 1) return (-2);
    if ((ad < 0) || (ad + nbbits > 1023)) return(-3);

  // écriture dans la table des valeurs :
    for (i = 0; i < nb_octets; i++){
        octet = trame_rec[i+7] & (uchar)0xFF;
        for (j=0; j<8; j++) {             // puis bit par bit
            if (((octet >> j) & 1) != 0) Btable[ad++] = 1;
                                    else Btable[ad++] = 0;
            if ( ((i<<3) + j) >= nbbits) break;
        }
    }
    // construction trame réponse:
    memcpy(trame_emi, trame_rec, 6);
    crc = CRC_16(trame_emi, 6);
    trame_emi[6] = (uchar)(crc & 0xFF);
    trame_emi[7] = (uchar)((crc  & 0xFF00) >> 8);
    return(8);    // nb d'octets
}
//---------------------------------------------------------------------------
//  ecri_mots  : ecriture dans Itable (table des mots MODBUS)   code 16
//               et construction de la trame émission
//---------------------------------------------------------------------------
int __fastcall TESCLAVE::ecri_mots(uchar *trame_emi, uchar *trame_rec, int lgtrame)
{
  int i, j, ad, nbmots;
  short int crc;

  if (lgtrame < 8) return(-1);
  ad = mot(trame_rec[2], trame_rec[3]);
  nbmots = mot(trame_rec[4], trame_rec[5]);
  if (nbmots < 1) return (-2);
  if ((ad < 0) || (ad + nbmots > MAXMOTS)) return(-3);

  // écriture dans la table des valeurs :
  for (i=ad, j=7; i < ad+nbmots; i++, j+=2){
	Itable[i] = ((trame_rec[j]&0xFF) << 8) | (trame_rec[j+1] & 0xFF) ;
  }

  memcpy(trame_emi, trame_rec, 6);
  crc = CRC_16(trame_emi, 6);
  trame_emi[6] = (uchar)(crc & 0xFF);
  trame_emi[7] = (uchar)((crc  & 0xFF00) >> 8);
  return(8);    // nb d'octets
}
//---------------------------------------------------------------------------
//  ecri_1mot  : ecriture dans tab_modbus   code 6
//               et construction de la trame émission
//---------------------------------------------------------------------------
int __fastcall TESCLAVE::ecri_1mot(uchar *trame_emi, uchar *trame_rec, int lgtrame)
{
  int i, j, ad, nbmots;
  short int crc;

  if (lgtrame != 8) return(-1);
  ad = mot(trame_rec[2], trame_rec[3]);
  if ((ad < 0) || (ad > MAXMOTS)) return(-3);

  // écriture dans la table des valeurs :
  Itable[ad] = ((trame_rec[4]&0xFF) << 8) | (trame_rec[5] & 0xFF) ;

  memcpy(trame_emi, trame_rec, 6);
  crc = CRC_16(trame_emi, 6);
  trame_emi[6] = (uchar)(crc & 0xFF);
  trame_emi[7] = (uchar)((crc  & 0xFF00) >> 8);
  return(8);    // nb d'octets
}
//---------------------------------------------------------------------------
//   repond() : analyse trame_rec et construit la trame réponse trame_emi
//---------------------------------------------------------------------------
int __fastcall TESCLAVE::repond(uchar *trame_emi, uchar *trame_rec, int lgtrame)
{
  int cr;
  if (lgtrame < 8) return(-11);  // trame trop courte
  if (!test_crc( trame_rec, lgtrame)) return(-5);   // erreur CRC
//  if (trame_rec[0] != NumEsclave) return(-12);  n° d'esclave : non testé
  switch (trame_rec[1]) {
    case 1:
    case 2: cr = lect_bits(trame_emi, trame_rec, lgtrame);
            break;
    case 3:
    case 4: cr = lect_mots(trame_emi, trame_rec, lgtrame);
            break;
    case 6: cr = ecri_1mot(trame_emi, trame_rec, lgtrame);
            break;
    case 15:cr = ecri_bits(trame_emi, trame_rec, lgtrame);
            break;
    case 16:cr = ecri_mots(trame_emi, trame_rec, lgtrame);
            break;
    default: return(-13);    // code fonction non traité
  }
  return (cr);  //retourne le nombre d'octets de la trame ou un code d'erreur
}


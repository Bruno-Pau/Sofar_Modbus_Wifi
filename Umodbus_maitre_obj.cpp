//---------------------------------------------------------------------------
//  Umodbus_maitre_obj.cpp : Unite MODBUS maitre        B.Vannier nov 2013
//------------------------------------------------------------------------
// Classe permettant de
//  - construire des trames maitre modbus
//  - d'interpreter la reponse de l'esclave
//
// Les accès au port sont gérés dans ce code
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
//   int init_com(char *szPort, int vit, int par, int timeout);
//   int lecture_mots(int nes, int ad, int nbmots, short *tabW);
//   int lecture_mots_bool(int nes, int ad, int nbmots, uchar *tabBool);
//   int ecri_mots(int nes, int ad, int nbmots, short *tabW);
//   int ecri_mots_bool(int nes, int ad, int nbmots, uchar *tabBool);
//   int close_com();
//   char *Cerreur(int noer, char *StrCErr);    // Renvoit une chaine C
//
// les autres fonction (CRC_16()...) n'ont pas a etre appelees
//---------------------------------------------------------------------------
// Modifs:
//  20/11/13 : Transformation fonctions -> Classe
// 26/07/22 : quelques amenagements pour interrogation onduleur SOFAR
//---------------------------------------------------------------------------

#include <windows.h>
#include "Umodbus_maitre_obj.h"
#pragma argsused
#pragma hdrstop
//---------------------------------------------------------------------------
// Constructeur / Destructeur
//---------------------------------------------------------------------------
TMODBUS_M::TMODBUS_M()
{
  Hport = NULL;
}
//-------------------------------
TMODBUS_M::~TMODBUS_M()
{
  close_com();
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
  else return(-3);
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
// Fonctions publiques
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Gestion de la COM
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// init_com(char *szPort, int vit, int par, int timeout):
//----------------------------------------------------------------------------
// int vit: 1200, 9600, 19200; par: 0=NONE, 1=ODD, 2=EVEN
// retourne 1: ok, -1: erreur ouverture port, -2 erreur vitesse, -3 erreur parité
//----------------------------------------------------------------------------
int TMODBUS_M::init_com(char *szPort, int vit, int par, int timeout)
{
  DCB Dcb1;  // structure des paramètres de la liaison série
  COMMTIMEOUTS time_out; // structure de time out

  if ((vit!=1200)&&(vit!=9600)&&(vit!=19200)) return(-4);
  if ((par < 0) || (par > 2)) return(-5);
  if (Hport != NULL) return (-6);

//------ ouverture du port série
  HANDLE Hport1 = CreateFile(
    	szPort,	// chaine indiquant le nom du fichier ou du port à ouvrir
    	GENERIC_READ|GENERIC_WRITE,	// accès (read-write) mode
    	0,	// share mode : non partagé
    	0,	// pointer to security attributes (pour Windows NT)
    	OPEN_EXISTING,	// fichier déjà existant
    	FILE_ATTRIBUTE_NORMAL,	// attributs de fichiers
    	NULL 	// handle to file with attributes to copy
   		);

  if (Hport1==INVALID_HANDLE_VALUE) {
    return(-1);
    }

  Hport = Hport1;

  // ------ Paramètres de la liaison série
  GetCommState(Hport,&Dcb1);   // recup paramètres par défaut
  Dcb1.ByteSize=8;             // 8 bits
  switch(vit) {
    case 1200: Dcb1.BaudRate = CBR_1200; break;// 1200 bauds
    case 19200: Dcb1.BaudRate = CBR_19200; break;// 19200 bauds
    case 9600:
    default:   Dcb1.BaudRate = CBR_9600; break;// 9600 bauds
  }

  Dcb1.Parity = par;               // parité
  Dcb1.StopBits=0;             // 1 bit de stop
  SetCommState(Hport,&Dcb1);

  //------- Gestion des time out
  GetCommTimeouts(Hport,&time_out); // récup structure par défaut
  time_out.ReadIntervalTimeout=50;  // temps entre 2 caractères
  time_out.ReadTotalTimeoutMultiplier=1;
  // temps total
  if (timeout < 50) timeout = 50;
  if (timeout > 2000) timeout = 2000;
  time_out.ReadTotalTimeoutConstant=timeout;
  time_out.WriteTotalTimeoutMultiplier=1; // ajoute le 19/02/14
  time_out.WriteTotalTimeoutConstant=1;   // ajoute le 19/02/14
  SetCommTimeouts(Hport,&time_out); // rechargement de la struture
  return (0);
}
//----------------------------------------------------------------------------
// close_com() : ferme le port série
//----------------------------------------------------------------------------
int TMODBUS_M::close_com()
{
  if (Hport == NULL) return(-10);
  if (CloseHandle( Hport )){
    Hport = NULL;
    return(0);
  }
  else return(-11);
}
//---------------------------------------------------------------------------
// Requète lecture mot:
//   - construction de la trame trame_lec()
//   - envoi trame vers API     WriteFile()
//   - leture trame API         ReadFile()
//   - analyse trame API        extrait_readT()
//---------------------------------------------------------------------------
int TMODBUS_M::lecture_mots(int nes, int ad, int nbmots, short *tabW)
{
  DWORD nblu, nbWritten;
  int cr;

  if ((nes <1) || (nes > 254)) return(-20);
  if ((ad < 0) || (ad > 0x7FFF)) return(-21);
  if ((nbmots < 0) || (nbmots > 128)) return(-22);

  // construction trame requète lecture :
// trame_readT(int adr, int nbmot, int nes, char * Tabemi);
  trame_readT (ad, nbmots, nes, buff_out);


  if (!WriteFile(       // envoi vers Port série de la requète lecture mots
        Hport,	// handle du port
        buff_out,	// chaine à transmettre
        8,	// number of bytes to write
        &nbWritten,	// pointer to number of bytes written
        NULL 	// pointer to structure needed for overlapped I/O
    ) ) return(-23);;


  if (!ReadFile(     // lecture du port série
        Hport,           // Handle du port
        buff_in,	     // address of buffer that receives data
        512,	         // nombre max de bytes à lire
        &nblu,	         // adresse du nombre de bytes lus
        NULL) 	         // LPOVERLAPPED:address of structure for data
    ) return(-24);

  if (nblu <= 0) return(-25); // pas de réponse API
            //extrait_readT(short * valeur, char * Tabrec);
  if(extrait_readT(tabW, buff_in) < 0)return (-26);

  return (0);
}
//---------------------------------------------------------------------------
// Requète lecture mot mais avec résultat sous forme de tableau de booléens
// chaque boléen est un octet prenant la valeur 1 ou 0 (pour Visual Basic)
//---------------------------------------------------------------------------
int TMODBUS_M::lecture_mots_bool(int nes, int ad, int nbmots, uchar *tabBool)
{
  short tabW[128];
  int cr, i;
  unsigned short j;

  cr = lecture_mots(nes, ad, nbmots, tabW);
  if (cr < 0) return (cr);
  for (i=0; i<nbmots; i++)
  {
    for (j=0; j<16; j++){
      tabBool[(i<<4)+j] = (tabW[i] >> j) & 1;
    }
  }
  return (cr);
}
//---------------------------------------------------------------------------
// Requète écriture mots:
//   - construction de la trame trame_writeT()
//   - envoi trame vers API     WriteFile()
//   - leture trame API         ReadFile()
//---------------------------------------------------------------------------
int TMODBUS_M::ecri_mots(int nes, int ad, int nbmots, short *tabW)
{
  DWORD nblu, nbWritten;
  int cr, lg;

  if ((nes <0) || (nes > 254)) return(-30);
  if ((ad < 0) || (ad > 0x7FFF)) return(-31);
  if ((nbmots < 0) || (nbmots > 128)) return(-32);

  // construction trame requète lecture :
  // (int adr, int nbmot, int nes, short * valeur, char * Tabemi)
  lg = trame_writeT(ad, nbmots, nes, tabW, buff_out);

  if (!WriteFile(       // envoi vers Port série
        Hport,	// handle du port
        buff_out,	// chaine à transmettre ATTENTION: forcément déclaré en global
        lg,	// number of bytes to write
        &nbWritten,	// pointer to number of bytes written
        NULL 	// pointer to structure needed for overlapped I/O
    ) ) return(-33);

  if (!ReadFile(     // lecture du port série
        Hport,           // Handle du port
        buff_in,	     // address of buffer that receives data
        512,	         // nombre max de bytes à lire
        &nblu,	         // adresse du nombre de bytes lus
        NULL) 	         // LPOVERLAPPED:address of structure for data
      ) return(-34);
  if (nblu <= 0) return(-35); // pas de réponse API

  return(0);
}
//---------------------------------------------------------------------------
// Requète écriture mots, mais avec présentation des données sous forme de tableau de booléens
// chaque boléen est un octet prenant la valeur 1 ou 0 (pour Visual Basic)
//---------------------------------------------------------------------------
int TMODBUS_M::ecri_mots_bool(int nes, int ad, int nbmots, uchar *tabBool)
{
  short tabW[128];
  int i;
  unsigned short j;

  if ((nbmots < 0) || (nbmots > 128)) return(-32);
  // préparation du tableau de mots
  for (i=0; i<nbmots; i++) {
    tabW[i] = 0;
    for (j=0; j<16; j++){
      tabW[i] = tabW[i] | ( ( (tabBool[(i<<4)+j]) & 1) << j);
    }
  }
  int cr = ecri_mots(nes, ad, nbmots, tabW);
  return (cr);
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

  if ((noer < 0) && (noer>-10)) {
    strcpy(origine, "Erreur ouverture port: ");
    switch (noer) {
      case -1: strcpy(texte, "deja ouvert ou inexistant"); break;
      case -3: strcpy(texte, "No port > 9 ou < 0"); break;
      case -4: strcpy(texte, "vitesse non prise en charge"); break;
      case -5: strcpy(texte, "Parite non prise en charge"); break;
      case -6: strcpy(texte, "Port deja ouvert"); break;
    }
  }
  if ((noer < -9) && (noer>-20)) {
   strcpy(origine, "Erreur fermeture port: ");
    switch (noer) {
      case -10: strcpy(texte, "port non ouvert"); break;
      case -11: strcpy(texte, "echec fermeture"); break;
    }
  }
  if ((noer < -19) && (noer>-30)) { strcpy(origine, "Erreur Lecture mots: "); er2 = noer+20;}
  if ((noer < -29) && (noer>-40)) { strcpy(origine, "Erreur Ecriture mots: "); er2 = noer+30;}

  if ((noer < -39) && (noer>-50)) { strcpy(origine, "Erreur Lecture bits: "); er2 = noer+40;}
  if ((noer < -49) && (noer>-60)) { strcpy(origine, "Erreur Ecriture bits: "); er2 = noer+50;}

  switch (er2) {
    case 0  : strcpy(texte, "no esclave < 1 ou > 254"); break;
    case -1 : strcpy(texte, "adresse mot < 0 ou > 32767"); break;
    case -2 : strcpy(texte, "nb de mots > 128"); break;
    case -3 : strcpy(texte, "erreur WriteFile sur port COM"); break;
    case -4 : strcpy(texte, "erreur ReadFile sur port COM"); break;
    case -5 : strcpy(texte, "pas de réponse API"); break;
    case -6 : strcpy(texte, "trame retour incorrecte"); break;
  }

  strcpy (StrCErr, origine);
  strcat (StrCErr, texte);
  
  return (StrCErr);
}


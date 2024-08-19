//---------------------------------------------------------------------------

#ifndef uClientSofar2H
#define uClientSofar2H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ScktComp.hpp>
#include <ExtCtrls.hpp>
#include <stdio.h>

#include "uModbus_Sofar.h"
#include <Grids.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
#define uint unsigned int
#define ushort unsigned short
#define uchar unsigned char
#ifndef tstbitH
#define tstbitH
//---------------------------------------------------------------------------
// tstbit.h  macros                            B.Vannier      fev 2012
//---------------------------------------------------------------------------
// definition de macros pour mise a 1 et reset de bits
#define SETBIT(x,n)  ((x)|=1<<(n))
#define RAZBIT(x,n)  ((x)&= ~(1<<(n)))
// definition de macros pour tester 1 bit dans une variable
#define TESTBIT(x,n)   ((x)&(1<<(n)))>>(n) // x: valeur a tester, n : no du bit
#endif
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// Composants gérés par l'EDI
  TClientSocket *ClientSocket1;
  TMemo *Memo1;
  TLabel *Label1;
  TButton *Connecter;
  TButton *Deconnecter;
  TEdit *Edit4;
  TLabel *Label5;
  TCheckBox *CheckBox1;
  TEdit *Edit6;
  TLabel *Label7;
  TLabel *Label8;
  TEdit *Edit1;
  TLabel *Label9;
  TButton *bNoSerie;
  TLabel *Label14;
  TTimer *Timer1;
  TPageControl *PageControl1;
  TTabSheet *TabSheet1;
  TTabSheet *TabSheet2;
  TTabSheet *TabSheet3;
  TLabel *Label10;
  TLabel *Label13;
  TLabel *Label15;
  TLabel *Label11;
  TLabel *Label12;
  TEdit *Edit7;
  TEdit *Edit8;
  TButton *Button1;
  TEdit *Edit9;
  TEdit *Edit10;
  TEdit *Edit11;
  TEdit *Edit12;
  TEdit *Edit13;
  TStringGrid *StringGrid1;
  TEdit *Edit14;
  TEdit *Edit15;
  TEdit *Edit16;
  TEdit *Edit17;
  TEdit *Edit18;
  TEdit *Edit19;
  TCheckBox *CheckBox2;
  TEdit *Edit20;
  TShape *Shape1;
  TButton *Button2;
  TButton *Button3;
  TStringGrid *StringGrid2;
  TStringGrid *StringGrid3;
  TLabel *Label2;
  TEdit *EditIP;
  TButton *Help;
  TPageControl *PageControl2;
  TTabSheet *TabSheet200;
  TTabSheet *TabSheet201;
  TTabSheet *TabSheet202;
  TTabSheet *TabSheet203;
  TTabSheet *TabSheet204;
  TTabSheet *TabSheet205;
  void __fastcall ClientSocket1Connect(TObject *Sender,
          TCustomWinSocket *Socket);
  void __fastcall ClientSocket1Disconnect(TObject *Sender,
          TCustomWinSocket *Socket);
  void __fastcall ClientSocket1Error(TObject *Sender,
          TCustomWinSocket *Socket, TErrorEvent ErrorEvent,
          int &ErrorCode);
  void __fastcall ClientSocket1Read(TObject *Sender,
          TCustomWinSocket *Socket);

  void __fastcall ConnecterClick(TObject *Sender);
  void __fastcall DeconnecterClick(TObject *Sender);

  void __fastcall Button1Click(TObject *Sender);
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall bNoSerieClick(TObject *Sender);
  void __fastcall Timer1Timer(TObject *Sender);
  void __fastcall Button2Click(TObject *Sender);
  void __fastcall Button3Click(TObject *Sender);
  void __fastcall HelpClick(TObject *Sender);

private:	// Déclarations de l'utilisateur
  uchar checkSum(uchar *buf, int nb);
  int affichTrame(uchar *buf, int nb, bool bSend);
  int affichTrameModbus(uchar *buf, int nb, bool bSend);
  int SerialNumber();
  int prepareGlobalFrame(uchar *buf, int serialNo, char *szText, uchar *modbus, int modbusLen);
  void prepareStringGrid1(int ad, int nbLi);
  void prepareStringGrid2(int ad, int nbLi);
  void prepareStringGrid3(int ad, int nbLi);
  bool IsServer;
  String Server;
  TMODBUS_M modbus;
  int iReqEnCours;  // indique la derniere requete envoyee (adresse mots)
  int iTimeCount;
  TCheckBox *pCheck[96];

public:		// Déclarations de l'utilisateur
  __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif

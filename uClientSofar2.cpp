//---------------------------------------------------------------------------
//  uClientSofar2.cpp       B. Vannier    21/10/22
//
//  interrogation onduleur photovoltaique SOFAR HYD 3-6K - ES
//
//---------------------------------------------------------------------------
//
// Serial number : SM1ES160MAD079
// SN adaptateur wifi : 2316640687  ( 0x8A1521AF )
//---------------------------------------------------------------------------
// Modifs:
// 29/11/22 : version anglaise
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "uClientSofar2.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
  : TForm(Owner)
{
  iReqEnCours = 0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormCreate(TObject *Sender)
{
  StringGrid1->ColCount = 4;
  StringGrid1->ColWidths[0] = 55;
  StringGrid1->ColWidths[1] = 300;
  StringGrid1->ColWidths[2] = 80;
  StringGrid1->ColWidths[3] = 80;

  StringGrid1->Cells[0][0] = "adr (Hexa)";
  StringGrid1->Cells[1][0] = "Item";
  StringGrid1->Cells[2][0] = "decimal";
  StringGrid1->Cells[3][0] = "hexa";

  prepareStringGrid1( 0x200, 86 );

  StringGrid2->ColCount = 4;
  StringGrid2->ColWidths[0] = 55;
  StringGrid2->ColWidths[1] = 460;
  StringGrid2->ColWidths[2] = 80;
  StringGrid2->ColWidths[3] = 80;

  StringGrid2->Cells[0][0] = "adr (Hexa)";
  StringGrid2->Cells[1][0] = "Item";
  StringGrid2->Cells[2][0] = "decimal";
  StringGrid2->Cells[3][0] = "hexa";

  prepareStringGrid2( 0x1000, 124 );

  StringGrid3->ColCount = 4;
  StringGrid3->ColWidths[0] = 55;
  StringGrid3->ColWidths[1] = 400;
  StringGrid3->ColWidths[2] = 80;
  StringGrid3->ColWidths[3] = 80;

  StringGrid3->Cells[0][0] = "adr (Hexa)";
  StringGrid3->Cells[1][0] = "Item";
  StringGrid3->Cells[2][0] = "decimal";
  StringGrid3->Cells[3][0] = "hexa";

  prepareStringGrid3( 0x10B0, 16 );

  Memo1->Lines->Clear();
  Memo1->Lines->Add(DateToStr(Date()) + " " + TimeToStr(Time()) + " \t ClientSofar2 \tBV \tnov 2022");

  PageControl1->ActivePage = TabSheet1;


  // Création des CheckBox pour le positionnement des sorties Superviseur -> Simu
  for (int i=0, k=0, Id=0; i<6; i++, k+=16) {
    TTabSheet *pTabSheet = NULL;
    switch (i) {
      case 0: pTabSheet = TabSheet200; break;
      case 1: pTabSheet = TabSheet201; break;
      case 2: pTabSheet = TabSheet202; break;
      case 3: pTabSheet = TabSheet203; break;
      case 4: pTabSheet = TabSheet204; break;
      case 5: pTabSheet = TabSheet205; break;
    }
    for (int j=0; j<16; j++) {
      pCheck[k+j] = new TCheckBox((TComponent*)Form1);
      pCheck[k+j]->Parent = pTabSheet;
      pCheck[k+j]->Height = 16;
      pCheck[k+j]->Width = 190;
      pCheck[k+j]->Tag = k + i;
      pCheck[k+j]->Top = 2 + ( 16*(j%8) );  // 248 + ( 16*(i%8) );
      pCheck[k+j]->Left = 8 + (200* ( j / 8 ) ) ;  // 8 + (200* ( i / 8 ) ) ;
      pCheck[k+j]->Checked = false;

      if ( ( i > 0 ) ) {  // pour affectation ID  a l'Item
        Id++;
        AnsiString AsId ;
        AsId.printf("ID%02d", Id);
        pCheck[k+j]->Caption = AsId;
      }
    }
  }
  // 0x200 :
  pCheck[0]->Caption = "WaitState";
  pCheck[1]->Caption = "CheckState";
  pCheck[2]->Caption = "NormalState";
  pCheck[3]->Caption = "EPSState";
  pCheck[4]->Caption = "FaultState";
  pCheck[5]->Caption = "PermanentState";
  // 0x204 :
  pCheck[64+0]->Caption = "ID49 Grid voltage consistence error";
  pCheck[64+1]->Caption = "Grid frequency consistence error ID50";
  pCheck[64+2]->Caption = "DCI current consistence error ID51";
  pCheck[64+3]->Caption = "battery communication fault ID52";
  pCheck[64+4]->Caption = "SPI communication error ID53";
  pCheck[64+5]->Caption = "SCI communication error ID54";
  pCheck[64+6]->Caption = "Relay detecting error ID55";
  pCheck[64+7]->Caption = "Isolation resistance low ID56";
  pCheck[64+8]->Caption = "battery temperature protection ID57";
  pCheck[64+9]->Caption = "Heat shrink over temperature protection ID58";
  pCheck[64+10]->Caption = "Ambient temperature protection ID59";
  pCheck[64+11]->Caption = "PE connection error ID60";
  //pCheck[64+12]->Caption = "ID61";
  //pCheck[64+13]->Caption = "ID62";
  //pCheck[64+14]->Caption = "ID63";
  //pCheck[64+15]->Caption = "ID64";

  Edit20->Text = "60";
}
//---------------------------------------------------------------------------
// Fonctions liées au boutons de Form1
//---------------------------------------------------------------------------
void __fastcall TForm1::ConnecterClick(TObject *Sender)
{
  ClientSocket1->Address = EditIP->Text;
  ClientSocket1->Port = Edit4->Text.ToInt();
  ClientSocket1->Active = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::DeconnecterClick(TObject *Sender)
{
  ClientSocket1->Active = false;
  Memo1->Lines->Add("Disconnection...");
}
//---------------------------------------------------------------------------
uchar TForm1::checkSum(uchar *buf, int nb)
{
  uchar sum = 0;
  for (int i=0; i<nb; i++) {
    sum += buf[i];
  }
  //buf[nb] = sum;
  return ( sum );
}
//---------------------------------------------------------------------------
int TForm1::affichTrame(uchar *buf, int nb, bool bSend)
{
  AnsiString AsSend = TimeToStr(Time()) + " | " ;
  if ( bSend ) AsSend += " Send(" + IntToStr(nb) + "): ";
  else AsSend += " Recv(" + IntToStr(nb) + "): ";
  for (int i=0; i<nb; i++) { // prendre octet par octet
      AsSend += IntToHex(buf[i] & 0xFF, 2) + " ";   // et decoder en Hexa
  }
  Memo1->Lines->Add(AsSend);
}
//---------------------------------------------------------------------------
int TForm1::affichTrameModbus(uchar *buf, int nb, bool bSend)
{
  AnsiString AsSend;
  int codeFunc = buf[1];
  if ( (codeFunc == 3 ) || (codeFunc == 4 ) ) AsSend = "Modbus Read: ";
  else AsSend = " Modbus Write (" + IntToHex(buf[1], 2) + "): ";
  for (int i=0; i<nb; i++) { // prendre octet par octet
      AsSend += IntToHex(buf[i] & 0xFF, 2) + " ";   // et decoder en Hexa
  }
  Memo1->Lines->Add(AsSend);
}
//---------------------------------------------------------------------------
int TForm1::SerialNumber()
{
  int nb = 0;
  int cr = sscanf( Edit6->Text.c_str(), "%X", &nb);
  if (cr > 0) return (nb);
  return(-1);
}
//---------------------------------------------------------------------------
int TForm1::prepareGlobalFrame(uchar *buf, int serialNo, char *szText, uchar *modbus, int modbusLen)
{
  memset( buf, 0, 36 );

  buf[0] = 0xA5;      // Logger Start code
  buf[1] = 0x17;      // Logger frame DataLength  (23)
  buf[2] = 0x00;
  buf[3] = 0x10;      // Logger ControlCode
  buf[4] = 0x45;
  buf[5] = 0x00;      // to Serial interface
  buf[6] = 0x00;

  memcpy ( &buf[7], &serialNo, 4);       // Serial number

  buf[11] = 0x02;
  strncpy(&buf[12], szText, 14);      // SendDataField

  memcpy ( &buf[26], modbus, modbusLen);       // Modbus

  int lg = 26 + modbusLen;

  buf[1] = lg - 11;

  buf[lg] = checkSum(&buf[1], lg-1);  // Checksum

  buf[++lg] = 0x15;   // Logger End code

  return(++lg);
}
//---------------------------------------------------------------------------
// Evennements liés au composant ClientSocket1
//---------------------------------------------------------------------------

void __fastcall TForm1::ClientSocket1Connect(TObject *Sender,
      TCustomWinSocket *Socket)
{
  Memo1->Lines->Add(TimeToStr(Time()) + " | Connect to: " + Socket->RemoteAddress + " : " + Socket->RemotePort  );
  Shape1->Brush->Color = clLime;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ClientSocket1Disconnect(TObject *Sender,
      TCustomWinSocket *Socket)
{
  Memo1->Lines->Add(TimeToStr(Time()) + " | Disconnect from: " + Socket->RemoteAddress);
  Shape1->Brush->Color = clGray;
}

//---------------------------------------------------------------------------
void TForm1::prepareStringGrid1(int ad, int nbLi)
{
  if (nbLi < 16) nbLi = 16;

  if ( ad == 0x200 ) {
    StringGrid1->RowCount = nbLi + 1;

    for (int i=0; i<nbLi; i++) {
      StringGrid1->Cells[0][i+1] = IntToHex( 0x200 + i, 4);
    }
    StringGrid1->Cells[1][1] = "Running state";
    StringGrid1->Cells[1][2] = "Fault message list 1";
    StringGrid1->Cells[1][3] = "Fault message list 2";
    StringGrid1->Cells[1][4] = "Fault message list 3";
    StringGrid1->Cells[1][5] = "Fault message list 4";
    StringGrid1->Cells[1][6] = "Fault message list 5";
    StringGrid1->Cells[1][7] = "Grid A Voltage (1/10 V)";
    StringGrid1->Cells[1][8] = "Grid A Current (1/100 A)";
    StringGrid1->Cells[1][9] = "Grid B Voltage (1/10 V)";
    StringGrid1->Cells[1][10] = "Grid B Current (1/100 A)";
    StringGrid1->Cells[1][11] = "Grid C Voltage (1/10 V)";
    StringGrid1->Cells[1][12] = "Grid C Current (1/100 A)";
    StringGrid1->Cells[1][13] = "Grid Frequency (1/100 Hz)";
    StringGrid1->Cells[1][14] = "Charge/Discharge power (1/100 kW)";
    StringGrid1->Cells[1][15] = "Battery Voltage (1/10 V)";
    StringGrid1->Cells[1][16] = "Battery Charge/Discharge current(1/100 A)";
    StringGrid1->Cells[1][17] = "The residual capacity of battery (%)";
    StringGrid1->Cells[1][18] = "Battery temperature";
    StringGrid1->Cells[1][19] = "Battery feed in/out power";
    StringGrid1->Cells[1][20] = "The power of the load";
    StringGrid1->Cells[1][21] = "Input/Output power";
    StringGrid1->Cells[1][22] = "The power of generation";
    StringGrid1->Cells[1][23] = "EPS output voltage (1/10 V)";
    StringGrid1->Cells[1][24] = "EPS output power";
    StringGrid1->Cells[1][25] = "Generation of one day (1/100 kWh)";
    StringGrid1->Cells[1][26] = "The power sell to grid of today";
    StringGrid1->Cells[1][27] = "The power buy from grid of today (1/100 kWh)";
    StringGrid1->Cells[1][28] = "Today consumption of the load (1/100 kWh)";
    StringGrid1->Cells[1][29] = "Total generation High-Byte (kWh)";
    StringGrid1->Cells[1][30] = "Total generation Low-Byte (kWh)";
    StringGrid1->Cells[1][31] = "Total export power,High (kWh)";
    StringGrid1->Cells[1][32] = "Total export power,Low (kWh)";

    StringGrid1->Cells[1][0x21] = "Total import power Hi (kWh)";
    StringGrid1->Cells[1][0x22] = "Total import power Low (kWh)";
    StringGrid1->Cells[1][0x23] = "Load consuming power Hi (kWh)";
    StringGrid1->Cells[1][0x24] = "Load consuming power Low (kWh)";
    StringGrid1->Cells[1][0x25] = "Battery today Charge power (1/100 kWh)";
    StringGrid1->Cells[1][0x26] = "Battery today discharge power (1/100 kWh)";
    StringGrid1->Cells[1][0x27] = "Battery total charge power Hi (1 kWh)";
    StringGrid1->Cells[1][0x28] = "Battery total charge power Low (1 kWh)";
    StringGrid1->Cells[1][0x29] = "Battery total discharge power Hi (1 kWh)";
    StringGrid1->Cells[1][0x2A] = "Battery total discharge power Low (1 kWh)";
    StringGrid1->Cells[1][0x2B] = "Count down time (1 s)";
    StringGrid1->Cells[1][0x2C] = "Inverter alarm";
    StringGrid1->Cells[1][0x2D] = "Battery cycle time (1 sec)";



    StringGrid1->Cells[1][0x2E] = "Inverter bus Voltage (0.1V)";
    StringGrid1->Cells[1][0x2F] = "LLC bus voltage (0.1V";

    StringGrid1->Cells[1][0x30] = "Buckcurrent";
    StringGrid1->Cells[1][0x31] = "Grid R voltage (0.1V)";
    StringGrid1->Cells[1][0x32] = "Grid R current (0.01A)";

    StringGrid1->Cells[1][0x38] = "Battery health(SOH) %";
    StringGrid1->Cells[1][0x39] = "Hybrid internal temperature (°C)";
    StringGrid1->Cells[1][0x3A] = "Heat shrink temperature";
    StringGrid1->Cells[1][0x3B] = "Country code";
    StringGrid1->Cells[1][0x3C] = "DC current (mA)";
    StringGrid1->Cells[1][0x3D] = "DC voltage (0,1V)";
    StringGrid1->Cells[1][0x3E] = "battery fault list1";
    StringGrid1->Cells[1][0x3F] = "battery fault list2";

    StringGrid1->Cells[1][0x40] = "battery fault list3";
    StringGrid1->Cells[1][0x41] = "battery fault list4";
    StringGrid1->Cells[1][0x42] = "battery fault list5";
    StringGrid1->Cells[1][0x43] = "Communication board internal info";
    StringGrid1->Cells[1][0x44] = "Today Generation time (1mn)";
    StringGrid1->Cells[1][0x45] = "Total generation time Hi (1h)";
    StringGrid1->Cells[1][0x46] = "Total generation time Low (1h)";
    StringGrid1->Cells[1][0x47] = "Isolation resistance of PV1+ to ground";
    StringGrid1->Cells[1][0x48] = "Isolation resistance of PV2+ to ground";
    StringGrid1->Cells[1][0x49] = "Isolation resistance of PV- to ground";

    StringGrid1->Cells[1][0x51] = "PV1 voltage (1/10 V)";
    StringGrid1->Cells[1][0x52] = "PV1 curent (1/100 A)";
    StringGrid1->Cells[1][0x53] = "PV1 power (1/100 kW)";
    StringGrid1->Cells[1][0x54] = "PV2 voltage (1/10 V)";
    StringGrid1->Cells[1][0x55] = "PV2 curent (1/100 A)";
    StringGrid1->Cells[1][0x56] = "PV2 power (1/100 kW)";
  }

}
//---------------------------------------------------------------------------
void TForm1::prepareStringGrid2(int ad, int nbLi)
{
  if (nbLi < 16) nbLi = 16;

  if ( ad == 0x1000 ) {
    StringGrid2->RowCount = nbLi + 1;

    for (int i=0; i<nbLi; i++) {
      StringGrid2->Cells[0][i+1] = IntToHex( 0x1000 + i, 4);
    }
    StringGrid2->Cells[1][1] = "Grid connection waiting time";
    StringGrid2->Cells[1][2] = "Grid power increasing Rate (% power / min)";
    StringGrid2->Cells[1][3] = "Grid connection waiting time after grid fault recovery";
    StringGrid2->Cells[1][4] = "Grid power increasing after grid fault recovery";
    StringGrid2->Cells[1][5] = "Over voltage protection value before grid connection";
    StringGrid2->Cells[1][6] = "Under voltage protection value before grid connection";
    StringGrid2->Cells[1][7] = "Over frequency protection value before grid connection";
    StringGrid2->Cells[1][8] = "Under frequency protection value before grid connection";
    StringGrid2->Cells[1][9] = "Reversed";
    StringGrid2->Cells[1][0xA] = "Reversed";
    StringGrid2->Cells[1][0xB] = "Reversed";
    StringGrid2->Cells[1][0xC] = "Reversed";
    StringGrid2->Cells[1][0xD] = "Reversed";
    StringGrid2->Cells[1][0xE] = "Reversed";
    StringGrid2->Cells[1][0xF] = "Reversed";
    StringGrid2->Cells[1][0x10] = "Reversed";
    StringGrid2->Cells[1][0x11] = "Enabled register for grid voltage protection";
    StringGrid2->Cells[1][0x12] = "Class 1 over voltage protection value";
    StringGrid2->Cells[1][0x13] = "Trigger time for class 1 over voltage protection";
    StringGrid2->Cells[1][0x14] = "Class 2 over voltage protection value";
    StringGrid2->Cells[1][0x15] = "Trigger time for class 1 over voltage protection";
    StringGrid2->Cells[1][0x16] = "Class 1 under voltage protection value";
    StringGrid2->Cells[1][0x17] = "Trigger time for class 1 under voltage protection";
    StringGrid2->Cells[1][0x18] = "Class 2 under voltage protection value";
    StringGrid2->Cells[1][0x19] = "Trigger time for class 2 under voltage protection";
    StringGrid2->Cells[1][0x1A] = "10-Min Over voltage protection value";
    StringGrid2->Cells[1][0x1B] = "";
    StringGrid2->Cells[1][0x1C] = "";
    StringGrid2->Cells[1][0x1D] = "";
    StringGrid2->Cells[1][0x1E] = "";
    StringGrid2->Cells[1][0x1F] = "";

    StringGrid2->Cells[1][0x20] = "";
    StringGrid2->Cells[1][0x21] = "Enabled register for grid frequency Protection";
    StringGrid2->Cells[1][0x22] = "Class 1 over frequency protection value";
    StringGrid2->Cells[1][0x23] = "Trigger Time For class 1over frequency protection";
    StringGrid2->Cells[1][0x24] = "Class 2 over frequency protection value";
    StringGrid2->Cells[1][0x25] = "Trigger Time For class 2 over frequency protection";
    StringGrid2->Cells[1][0x26] = "Class 1 under frequency protection value";
    StringGrid2->Cells[1][0x27] = "Trigger Time For class1 under frequency protection";
    StringGrid2->Cells[1][0x28] = "Class 2 under frequency protection value";
    StringGrid2->Cells[1][0x29] = "Trigger Time For class 2 under frequency protection";

    StringGrid2->Cells[1][0x31] = "Enable register for DCI protection";
    StringGrid2->Cells[1][0x32] = "Class 1 DCI protection value";
    StringGrid2->Cells[1][0x33] = "Trigger Time For class 1 DCI protection";
    StringGrid2->Cells[1][0x34] = "Class 2 DCI protection value";
    StringGrid2->Cells[1][0x35] = "Trigger Time for class 2 DCI protection";
    StringGrid2->Cells[1][0x36] = "DCI injection test value";

    StringGrid2->Cells[1][0x41] = "Active power &remote on/off control";
    StringGrid2->Cells[1][0x42] = "Percentage of active power output (0.1%)";
    StringGrid2->Cells[1][0x43] = "Remote on/off";

    StringGrid2->Cells[1][0x51] = "Enable & Working Mode register";
    StringGrid2->Cells[1][0x52] = "Staring rate for over/under frequency power derate";
    StringGrid2->Cells[1][0x53] = "Over/under Frequency power derate rate";
    StringGrid2->Cells[1][0x54] = "After over/under frequency power derate restore ,maxim um rate for power recovery";
    StringGrid2->Cells[1][0x55] = "After over/under frequency power derate restore ,minimu m rate for power recovery";
    StringGrid2->Cells[1][0x56] = "Waiting time 0-55 Remarks 0.01HzAfter over/under frequency power derate restor";
    StringGrid2->Cells[1][0x57] = "over/under frequency derate recovery rate";

    StringGrid2->Cells[1][0x62] = "Power Factor";
    StringGrid2->Cells[1][0x63] = "Regular reactive power percentage(Hi byte indicate the %)";
    StringGrid2->Cells[1][0x64] = "P-cos phy Curve mode first point power factor";
    StringGrid2->Cells[1][0x65] = "P-cos phy Curve mode first point power percentage";
    StringGrid2->Cells[1][0x66] = "P-cos phy Curve mode second point power factor";
    StringGrid2->Cells[1][0x67] = "P-cos f Curve mode second point power percentage";
    StringGrid2->Cells[1][0x68] = "P-cos f Curve mode third point power factor";
    StringGrid2->Cells[1][0x69] = "P-cos f Curve mode thirdpoint power percentage";
    StringGrid2->Cells[1][0x6A] = "P-cos f Curve mode fourth point power factor";
    StringGrid2->Cells[1][0x6B] = "P-cos f Curve mode fourth point power percentage";
    StringGrid2->Cells[1][0x6C] = "P-cos f curve mode lockin voltage value";
    StringGrid2->Cells[1][0x6D] = "P-cos f curve mode lockout voltage value";
    StringGrid2->Cells[1][0x6E] = "Starting voltage of Q-U curve mode 1";
    StringGrid2->Cells[1][0x6F] = "Ending voltage of Q-U curve mode 1 (high voltage)";
    StringGrid2->Cells[1][0x70] = "Starting voltage of Q-U curve mode 1 (low voltage)";

    StringGrid2->Cells[1][0x71] = "Ending voltage of Q-U curve mode 1 (low voltage)";
    StringGrid2->Cells[1][0x72] = "lockin power from Q-U curve mode1";
    StringGrid2->Cells[1][0x73] = "lockout power from Q-U curve mode1";
    StringGrid2->Cells[1][0x74] = "Max reactive power of Q-U curve mode1";
    StringGrid2->Cells[1][0x75] = "Response time for reactive power from Q- U curve mode1";

  }
}
//---------------------------------------------------------------------------
void TForm1::prepareStringGrid3(int ad, int nbLi)
{
  if (nbLi < 16) nbLi = 16;

  if ( ad == 0x10B0 ) {
    StringGrid3->RowCount = nbLi + 1;

    for (int i=0; i<nbLi; i++) {
      StringGrid3->Cells[0][i+1] = IntToHex( 0x10B0 + i, 4);
    }
    StringGrid3->Cells[1][1] = "Battery Type";
    StringGrid3->Cells[1][2] = "Battery Capacity";
    StringGrid3->Cells[1][3] = "Power management Mode";
    StringGrid3->Cells[1][4] = "Max Charge Voltage";
    StringGrid3->Cells[1][5] = "Max charge Current";
    StringGrid3->Cells[1][6] = "Over voltage protection value";
    StringGrid3->Cells[1][7] = "Min discharge voltage";
    StringGrid3->Cells[1][8] = "Max discharge current";
    StringGrid3->Cells[1][9] = "Under voltage protection";
    StringGrid3->Cells[1][0xA] = "DOD(depth Discharge)";
    StringGrid3->Cells[1][0xB] = "DOD of on grid mode(Shallow DOD)";
    StringGrid3->Cells[1][0xC] = "Empty battery voltage (0.01V)";
    StringGrid3->Cells[1][0xD] = "Full battery voltage (0.01V)";
  }
}
//---------------------------------------------------------------------------
// ClientSocket1Read() : reception trames TCP
//---------------------------------------------------------------------------
void __fastcall TForm1::ClientSocket1Read(TObject *Sender,
      TCustomWinSocket *Socket)
{
/*
Exemple de trames recues:

Bonne trame recue: ( Modbus : 01 03 10 00 04 00 05 00 00 00 00 00 00 00 00 09 45 00 02 7F D7 )
48 Reçus: A5 23 00 10 15 00 52 AF 21 15 8A 02 01 E9 89 0E 1F 79 09 00 00 C8 D4 4B 44 01 03 10 00 04 00 05 00 00 00 00 00 00 00 00 09 45 00 02 7F D7 1B

[0] : A5 // Logger Start code
[1] : 23 // Logger frame DataLength
[2] : 00
[3] : 10 // Logger ControlCode
[4] : 15
[5] : 00
[6] : 52
[7] : AF 21 15 8A // Serial number (8A1521AF)
[11] : 02 01 E9 89 0E 1F 79 09 00 00 C8 D4 4B 44  // ??
[25] : 01 03 10 00 04 00 05 00 00 00 00 00 00 00 00 09 45 00 02 7F D7 // Reponse Modbus
[47] : 15 Checksum

Trame avec Erreur:
29 Reçus: A5 10 00 10 15 00 91 AF 21 15 8A 02 01 F3 78 0E 1F B4 22 00 00 C0 D4 4B 44 06 00 CF 15 
 
[0] : A5 // Logger Start code
[1] : 10 // Logger frame DataLength
[2] : 00
[3] : 10 // Logger ControlCode
[4] : 15
[5] : 00
[6] : 91
[7] : AF 21 15 8A // Serial number (8A1521AF)
[11] : 02 01 F3 78 0E 1F B4 22 00 00 C0 D4 4B 44
[25] : 06 00 CF // code erreur ?
[28] : 15 Checksum

*/

  uchar bufin[512];
  short mots[128];
  int noSerie = 0;

  int lg = Socket->ReceiveLength();
  Socket->ReceiveBuf(bufin, 512);

  if ( CheckBox1->Checked ) {     // Affichage en Hexadécimal
    AnsiString AsRecept = TimeToStr(Time()) + " | Recv("+ IntToStr(lg) + ") : " ;
    for (int i=0; i<lg; i++) { // prendre octet par octet
      AsRecept += IntToHex(bufin[i] & 0xFF, 2) + " ";   // et decoder en Hexa
    }
    Memo1->Lines->Add(AsRecept);
  }

  int cr = 0;

  if (lg >= 29 ) {
    uchar *trameModbus = &bufin[25];
    int lg2 = lg - 26;
    int nes = trameModbus[0];
    int codeFonc = trameModbus[1];

    if (codeFonc >= 0x80) {
      int coder = codeFonc & 0xF;
      switch (coder) {
        case 1 : Memo1->Lines->Add("Illegal function (code " + IntToHex(codeFonc,2) + ")");break;
        case 2 : Memo1->Lines->Add("Illegal data address (code " + IntToHex(codeFonc,2) + ")");break;
        case 3 : Memo1->Lines->Add("Illegal data value (code " + IntToHex(codeFonc,2) + ") plage d'adresse interdite?");break;
        case 4 : Memo1->Lines->Add("Slave device failure (code " + IntToHex(codeFonc,2) + ")");break;
        case 5 : Memo1->Lines->Add("Acknowledge (code " + IntToHex(codeFonc,2) + ")");break;
        case 6 : Memo1->Lines->Add("Slave device busy (code " + IntToHex(codeFonc,2) + ")");break;
        default : Memo1->Lines->Add("Erreur Modbus (code " + IntToHex(codeFonc,2) + ")");
      }
      Shape1->Brush->Color = clRed;
      return;
    }

    if ( (nes == 6) && (codeFonc == 0) ) {
      *(&noSerie) = *( ( int *) &bufin[7] );
      Memo1->Lines->Add("Error Serial N° (code 06 00)\tSerial N° : " + IntToHex(noSerie, 4) );
      Edit6->Text = IntToHex( noSerie, 4 );
      Memo1->Lines->Add("perhaps is it better to disConnect / re-connect to continue ?");
      Shape1->Brush->Color = clYellow;
      return;
    }


    if ( (nes == 5) && (codeFonc == 0) ) {
      Memo1->Lines->Add("Request Error (code 05 00)");
      Shape1->Brush->Color = clRed;
      return;
    }
    if ( (codeFonc != 3) && (codeFonc != 4) ) {
      return;   // requete ecriture
    }

    cr = modbus.extrait_readT(mots, trameModbus);
    if (cr<0) {
      if (cr == -5) {
        Memo1->Lines->Add("Request Error cr: " + IntToStr(cr));
      } else Memo1->Lines->Add("cr: " + IntToStr(cr));
      Shape1->Brush->Color = clRed;
      return ;
    }
    Shape1->Brush->Color = clLime;

    int nbmots = cr;
    Memo1->Lines->Add( IntToStr(nbmots) + " words read");


    if ( iReqEnCours > 0 ) {
      if ( iReqEnCours == 0x200 ) {
        double dVgrid = mots[6]/10.0, dIgrid = mots[7]/100.0;
        double dFgrid = mots[0xC]/100.0, dPgrid = mots[0xD]/100.0;
        double dVbat = mots[0xE]/10.0, dIbat = mots[0xF]/100.0;
        double dWbat = dVbat * dIbat;
        double dVPV1 = mots[0x50]/10.0, dIPV1 = mots[0x51]/100.0, dPPV1 = mots[0x52]/100.0;
        double dVPV2 = mots[0x53]/10.0, dIPV2 = mots[0x54]/100.0, dPPV2 = mots[0x55]/100.0;

        Edit7->Text = FormatFloat("000.0",dVgrid) + " V";
        Edit8->Text = FormatFloat("0.00",dIgrid) + " A";
        Edit9->Text = FormatFloat("0.000",dPgrid) + " KW";
        Edit12->Text = FormatFloat("00.00",dFgrid) + " Hz";

        Edit10->Text = FormatFloat("000.0",dVbat) + " V";
        //Edit11->Text = FormatFloat("0.00",dIbat) + " A";
        Edit11->Text = FormatFloat("0.000",dWbat / 1000.0) + " kW";
        Edit13->Text = IntToStr( mots[0x10] ) + " %";

        Edit14->Text = FormatFloat("000.0",dVPV1) + " V";
        Edit15->Text = FormatFloat("0.00",dIPV1) + " A";
        Edit16->Text = FormatFloat("0.00",dPPV1) + " kW";

        Edit17->Text = FormatFloat("000.0",dVPV2) + " V";
        Edit18->Text = FormatFloat("0.00",dIPV2) + " A";
        Edit19->Text = FormatFloat("0.00",dPPV2) + " kW";

        // Affichage decimal & Hexa dans StringGrid
        for (int i=0; i<nbmots; i++) {
          StringGrid1->Cells[2][i+1] = IntToStr( mots[i]);
          StringGrid1->Cells[3][i+1] = "0x" + IntToHex( mots[i] & 0xFFFF, 4);
        }

        // Etat des bits des mots 0x200
        for (int i=0; i<16; i++) {
          pCheck[i]->Checked = TESTBIT(mots[0x0],i);
        }
        // Etat des bits des mots 0x204
        for (int i=0; i<16; i++) {
          pCheck[48+i]->Checked = TESTBIT(mots[0x4],i);
        }
      }


      if ( iReqEnCours == 0x1000 ) {
        //prepareStringGrid2( iReqEnCours, nbmots );
        for (int i=0; i<nbmots; i++) {
          StringGrid2->Cells[2][i+1] = IntToStr( mots[i]);
          StringGrid2->Cells[3][i+1] = "0x" + IntToHex( mots[i] & 0xFFFF, 4);
        }
      }

      if ( iReqEnCours == 0x10B0 ) {
        //prepareStringGrid3( iReqEnCours, nbmots );
        for (int i=0; i<nbmots; i++) {
          StringGrid3->Cells[2][i+1] = IntToStr( mots[i]);
          StringGrid3->Cells[3][i+1] = "0x" + IntToHex( mots[i] & 0xFFFF, 4);
        }
      }

      iReqEnCours = 0;
      return;
    }


    Memo1->Lines->Add("No request pending ! What is the address ?");

    AnsiString AsRecept = "values (16):\t";
    for (int i=0; i<cr; i++) { // prendre octet par octet
      if ( ( i>15) && ( i%16 == 0 ) ) {
        Memo1->Lines->Add(AsRecept);
        AsRecept = "\t0x" + IntToHex(i, 4) + " :\t";
      }
      AsRecept += IntToHex(mots[i] & 0xFFFF, 4) + " \t";   // et decoder en Hexa
    }
    Memo1->Lines->Add(AsRecept);
    AsRecept = "valeurs (10):\t";

    for (int i=0; i<cr; i++) { // prendre octet par octet
      if ( ( i>15) && ( i%16 == 0 ) ) {
        Memo1->Lines->Add(AsRecept);
        AsRecept = "\t\t";
      }
      AsRecept += IntToStr(mots[i] & 0xFFFF) + "\t";   // et decoder en Hexa
    }
    Memo1->Lines->Add(AsRecept);

  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ClientSocket1Error(TObject *Sender,
      TCustomWinSocket *Socket, TErrorEvent ErrorEvent, int &ErrorCode)
{
  Memo1->Lines->Add(TimeToStr(Time()) + " | Error connecting to:" + Server);
  Shape1->Brush->Color = clRed;
  ErrorCode = 0;
}

//---------------------------------------------------------------------------

void __fastcall TForm1::Button1Click(TObject *Sender)
{
  if ( !ClientSocket1->Active ) {
    Memo1->Lines->Add("Not connected !");
    return;
  }

  uchar bufOut[256], pModbus[256];
  int lgM;
  int nes = 1;

  Memo1->Lines->Add("");

  int ad = 0x200;  // adresse Modbus
  iReqEnCours = ad;

  int nb = 86; // nb de mots


  lgM = modbus.trame_readT(ad, nb, nes, pModbus);

  if (lgM < 0) return;
                                   // (10) 2316640687
  affichTrameModbus(pModbus, lgM, true);

  int lg = prepareGlobalFrame(bufOut, SerialNumber(), Edit1->Text.c_str(), pModbus, lgM);


  // Affichage trame en Hexadécimal
  affichTrame(bufOut, lg, true);

  ClientSocket1->Socket->SendBuf(bufOut , lg);

}

//---------------------------------------------------------------------------
void __fastcall TForm1::bNoSerieClick(TObject *Sender)
{
  uchar bufOut[256], pModbus[256];
  int lgM;
  int nes = 1;

  Memo1->Lines->Add("");

  int ad = 0x200;  // adresse Modbus
  iReqEnCours = ad;

  int nb = 8; // nb de mots


  lgM = modbus.trame_readT(ad, nb, nes, pModbus);

  if (lgM < 0) return;
                                   // (10) 2316640687
  affichTrameModbus(pModbus, lgM, true);

  int lg = prepareGlobalFrame(bufOut, 0, Edit1->Text.c_str(), pModbus, lgM);


  // Affichage trame en Hexadécimal
  affichTrame(bufOut, lg, true);

  ClientSocket1->Socket->SendBuf(bufOut , lg);

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
  if ( !CheckBox2->Checked ) return;
  if ( !ClientSocket1->Socket->Connected ) return;

  if ( --iTimeCount <= 0) {
    iTimeCount = 60;
    Button1Click(NULL);
  }

  Edit20->Text = IntToStr(iTimeCount);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button2Click(TObject *Sender)
{
  if ( !ClientSocket1->Active ) {
    Memo1->Lines->Add("Not connected !");
    return;
  }

  uchar bufOut[256], pModbus[256];
  int lgM;
  int nes = 1;

  Memo1->Lines->Add("");

  int ad = 0x1000;  // adresse Modbus
  iReqEnCours = ad;

  int nb = 124; // nb de mots


  lgM = modbus.trame_readT4(ad, nb, nes, pModbus);

  if (lgM < 0) return;
                                   // (10) 2316640687
  affichTrameModbus(pModbus, lgM, true);

  int lg = prepareGlobalFrame(bufOut, SerialNumber(), Edit1->Text.c_str(), pModbus, lgM);


  // Affichage trame en Hexadécimal
  affichTrame(bufOut, lg, true);

  ClientSocket1->Socket->SendBuf(bufOut , lg);


}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button3Click(TObject *Sender)
{
  if ( !ClientSocket1->Active ) {
    Memo1->Lines->Add("Not connected !");
    return;
  }

  uchar bufOut[256], pModbus[256];
  int lgM;
  int nes = 1;

  Memo1->Lines->Add("");

  int ad = 0x10B0;  // adresse Modbus
  iReqEnCours = ad;

  int nb = 16; // nb de mots


  lgM = modbus.trame_readT4(ad, nb, nes, pModbus);

  if (lgM < 0) return;
                                   // (10) 2316640687
  affichTrameModbus(pModbus, lgM, true);

  int lg = prepareGlobalFrame(bufOut, SerialNumber(), Edit1->Text.c_str(), pModbus, lgM);


  // Affichage trame en Hexadécimal
  affichTrame(bufOut, lg, true);

  ClientSocket1->Socket->SendBuf(bufOut , lg);


}
//---------------------------------------------------------------------------

void __fastcall TForm1::HelpClick(TObject *Sender)
{
  Application->MessageBoxA( "free programm\nbruno.vannier.pau@gmail.com", "B.Vannier");  
}
//---------------------------------------------------------------------------


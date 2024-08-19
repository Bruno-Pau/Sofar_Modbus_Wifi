object Form1: TForm1
  Left = 280
  Top = 119
  Width = 1081
  Height = 728
  Caption = 
    'ClientSofar2: TCP Connection to SOFAR inverter - B.Vannier HYD 3' +
    '-6K-ES   2024'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 16
    Top = 8
    Width = 52
    Height = 13
    Caption = 'SOFAR IP:'
  end
  object Label5: TLabel
    Left = 216
    Top = 8
    Width = 18
    Height = 13
    Caption = 'port'
  end
  object Label7: TLabel
    Left = 720
    Top = 8
    Width = 61
    Height = 16
    Caption = 'Serial N'#176'  :'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object Label8: TLabel
    Left = 720
    Top = 48
    Width = 102
    Height = 13
    Caption = '( 8 Hexa characteres)'
  end
  object Label9: TLabel
    Left = 856
    Top = 8
    Width = 124
    Height = 13
    Caption = 'Text message in the frame'
  end
  object Label14: TLabel
    Left = 856
    Top = 48
    Width = 66
    Height = 13
    Caption = '(unnecessary)'
  end
  object Shape1: TShape
    Left = 296
    Top = 32
    Width = 137
    Height = 17
    Brush.Color = clGray
  end
  object Memo1: TMemo
    Left = 8
    Top = 536
    Width = 1001
    Height = 137
    Lines.Strings = (
      'Client')
    ScrollBars = ssVertical
    TabOrder = 0
  end
  object Connecter: TButton
    Left = 296
    Top = 8
    Width = 65
    Height = 17
    Caption = 'Connect'
    TabOrder = 1
    OnClick = ConnecterClick
  end
  object Deconnecter: TButton
    Left = 368
    Top = 8
    Width = 65
    Height = 17
    Caption = 'Disconnect'
    TabOrder = 2
    OnClick = DeconnecterClick
  end
  object Edit4: TEdit
    Left = 240
    Top = 8
    Width = 49
    Height = 21
    TabOrder = 3
    Text = '8899'
  end
  object CheckBox1: TCheckBox
    Left = 864
    Top = 472
    Width = 97
    Height = 17
    Caption = 'Affichage trames'
    Checked = True
    State = cbChecked
    TabOrder = 4
  end
  object Edit6: TEdit
    Left = 720
    Top = 24
    Width = 105
    Height = 21
    TabOrder = 5
    Text = '8A1521AF'
  end
  object Edit1: TEdit
    Left = 856
    Top = 24
    Width = 121
    Height = 21
    TabOrder = 6
    Text = 'Hello SOFAR'
  end
  object bNoSerie: TButton
    Left = 488
    Top = 24
    Width = 217
    Height = 25
    Caption = 'Ask for Serial number SOFAR WiFi interface'
    TabOrder = 7
    OnClick = bNoSerieClick
  end
  object PageControl1: TPageControl
    Left = 16
    Top = 64
    Width = 1041
    Height = 457
    ActivePage = TabSheet3
    TabIndex = 2
    TabOrder = 8
    object TabSheet1: TTabSheet
      Caption = 'Word 200 ->'
      object Label10: TLabel
        Left = 8
        Top = 88
        Width = 53
        Height = 13
        Caption = '230V Grid :'
      end
      object Label13: TLabel
        Left = 8
        Top = 112
        Width = 132
        Height = 13
        Caption = 'Power Charge / Discharge :'
      end
      object Label15: TLabel
        Left = 8
        Top = 136
        Width = 129
        Height = 13
        Caption = 'Battery Charge / Discharge'
      end
      object Label11: TLabel
        Left = 8
        Top = 168
        Width = 56
        Height = 13
        Caption = 'PV1 Pannel'
      end
      object Label12: TLabel
        Left = 8
        Top = 200
        Width = 56
        Height = 13
        Caption = 'PV2 Pannel'
      end
      object Label2: TLabel
        Left = 8
        Top = 240
        Width = 173
        Height = 13
        Caption = 'Status and Fault Message List 0x20x'
      end
      object Edit7: TEdit
        Left = 184
        Top = 80
        Width = 70
        Height = 21
        TabOrder = 0
        Text = 'Edit7'
      end
      object Edit8: TEdit
        Left = 264
        Top = 80
        Width = 70
        Height = 21
        TabOrder = 1
        Text = 'Edit8'
      end
      object Button1: TButton
        Left = 16
        Top = 8
        Width = 153
        Height = 25
        Caption = 'Read Words 200 ->'
        TabOrder = 2
        OnClick = Button1Click
      end
      object Edit9: TEdit
        Left = 184
        Top = 104
        Width = 70
        Height = 21
        TabOrder = 3
        Text = 'Edit9'
      end
      object Edit10: TEdit
        Left = 184
        Top = 136
        Width = 70
        Height = 21
        TabOrder = 4
        Text = 'Edit10'
      end
      object Edit11: TEdit
        Left = 264
        Top = 136
        Width = 70
        Height = 21
        TabOrder = 5
        Text = 'Edit11'
      end
      object Edit12: TEdit
        Left = 344
        Top = 80
        Width = 70
        Height = 21
        TabOrder = 6
        Text = 'Edit12'
      end
      object Edit13: TEdit
        Left = 344
        Top = 136
        Width = 70
        Height = 21
        TabOrder = 7
        Text = 'Edit13'
      end
      object StringGrid1: TStringGrid
        Left = 440
        Top = 8
        Width = 585
        Height = 353
        TabOrder = 8
      end
      object Edit14: TEdit
        Left = 184
        Top = 168
        Width = 70
        Height = 21
        TabOrder = 9
        Text = 'Edit14'
      end
      object Edit15: TEdit
        Left = 264
        Top = 168
        Width = 70
        Height = 21
        TabOrder = 10
        Text = 'Edit15'
      end
      object Edit16: TEdit
        Left = 344
        Top = 168
        Width = 70
        Height = 21
        TabOrder = 11
        Text = 'Edit16'
      end
      object Edit17: TEdit
        Left = 184
        Top = 200
        Width = 70
        Height = 21
        TabOrder = 12
        Text = 'Edit17'
      end
      object Edit18: TEdit
        Left = 264
        Top = 200
        Width = 70
        Height = 21
        TabOrder = 13
        Text = 'Edit18'
      end
      object Edit19: TEdit
        Left = 344
        Top = 200
        Width = 70
        Height = 21
        TabOrder = 14
        Text = 'Edit19'
      end
      object CheckBox2: TCheckBox
        Left = 184
        Top = 8
        Width = 113
        Height = 17
        Caption = 'Auto Read(1mn)'
        TabOrder = 15
      end
      object Edit20: TEdit
        Left = 184
        Top = 56
        Width = 49
        Height = 21
        TabOrder = 16
        Text = 'Edit20'
      end
      object PageControl2: TPageControl
        Left = 8
        Top = 256
        Width = 417
        Height = 169
        ActivePage = TabSheet200
        TabIndex = 0
        TabOrder = 17
        object TabSheet200: TTabSheet
          Caption = '0x200'
        end
        object TabSheet201: TTabSheet
          Caption = '0x201'
          ImageIndex = 1
        end
        object TabSheet202: TTabSheet
          Caption = '0x202'
          ImageIndex = 2
        end
        object TabSheet203: TTabSheet
          Caption = '0x203'
          ImageIndex = 3
        end
        object TabSheet204: TTabSheet
          Caption = '0x204'
          ImageIndex = 4
        end
        object TabSheet205: TTabSheet
          Caption = '0x205'
          ImageIndex = 5
        end
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Words 1000 ->'
      ImageIndex = 1
      object Button2: TButton
        Left = 8
        Top = 16
        Width = 153
        Height = 25
        Caption = 'Read Words  1000 ->'
        TabOrder = 0
        OnClick = Button2Click
      end
      object StringGrid2: TStringGrid
        Left = 272
        Top = 24
        Width = 737
        Height = 329
        TabOrder = 1
      end
    end
    object TabSheet3: TTabSheet
      Caption = 'Words 10B0 ->'
      ImageIndex = 2
      object Button3: TButton
        Left = 8
        Top = 8
        Width = 153
        Height = 25
        Caption = 'Read Words  10B0 ->'
        TabOrder = 0
        OnClick = Button3Click
      end
      object StringGrid3: TStringGrid
        Left = 288
        Top = 16
        Width = 705
        Height = 329
        TabOrder = 1
      end
    end
  end
  object EditIP: TEdit
    Left = 80
    Top = 8
    Width = 121
    Height = 21
    TabOrder = 9
    Text = '192.168.1.123'
  end
  object Help: TButton
    Left = 938
    Top = 48
    Width = 41
    Height = 20
    Caption = 'Help'
    TabOrder = 10
    OnClick = HelpClick
  end
  object ClientSocket1: TClientSocket
    Active = False
    ClientType = ctNonBlocking
    Port = 1000
    OnConnect = ClientSocket1Connect
    OnDisconnect = ClientSocket1Disconnect
    OnRead = ClientSocket1Read
    OnError = ClientSocket1Error
    Left = 448
    Top = 8
  end
  object Timer1: TTimer
    OnTimer = Timer1Timer
    Left = 496
  end
end

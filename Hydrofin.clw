; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=GameChestDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "Hydrofin.h"
LastPage=0

ClassCount=2

ResourceCount=3
Class1=HydrofinDlg
Resource1=IDD_GAMECHEST
Resource2=IDD_CONFIG
Class2=GameChestDlg
Resource3=IDS_MENU1

[DLG:IDD_CONFIG]
Type=1
Class=HydrofinDlg
ControlCount=25
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_CONFIGTAB,SysTabControl32,1342181376
Control4=IDC_STATIC1,button,1073872903
Control5=IDC_LANGRUS,button,1073741833
Control6=IDC_LANGENG,button,1073741833
Control7=IDC_STATIC2,static,1342308352
Control8=IDC_STATIC3,static,1342308352
Control9=IDC_CHOOSEGAMEDIR,button,1342246656
Control10=IDC_CHOOSELIBDIR,button,1342246656
Control11=IDC_HYDRADIR,edit,1350631554
Control12=IDC_DEFAULTDIR,edit,1350631554
Control13=IDC_STATIC4,button,1342177287
Control14=IDC_COMBOUPDOWN,combobox,1344340035
Control15=IDC_STATIC5,static,1342308866
Control16=IDC_STATIC6,static,1342308866
Control17=IDC_MORETEXT,edit,1350631552
Control18=IDC_COMBOPAGEUPDOWN,combobox,1344340035
Control19=IDC_STATIC7,static,1342308866
Control20=IDC_NUMBERSCROLLLINES,edit,1350639744
Control21=IDC_BUMBERSCROLLLINESSPIN,msctls_updown32,1342177334
Control22=IDC_STATIC8,static,1342308866
Control23=IDC_DELAYTIME,edit,1350639744
Control24=IDC_DELAYTIMESPIN,msctls_updown32,1342177334
Control25=IDC_STATIC9,static,1342308866

[CLS:HydrofinDlg]
Type=0
HeaderFile=HydrofinDlg.h
ImplementationFile=HydrofinDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=HydrofinDlg
VirtualFilter=dWC

[MNU:IDS_MENU1]
Type=1
Class=?
CommandCount=0

[DLG:IDD_GAMECHEST]
Type=1
Class=GameChestDlg
ControlCount=10
Control1=IDOK,button,1476460545
Control2=IDCANCEL,button,1342242816
Control3=IDC_GAMELIST,SysListView32,1350633481
Control4=IDC_ADDGAMEFILE,button,1342242816
Control5=IDC_ADDGAMEDIR,button,1476460544
Control6=IDC_REMOVEGAME,button,1476460544
Control7=IDC_MOVEGAMEUP,button,1073807360
Control8=IDC_MOVEGAMEDOWN,button,1073807360
Control9=IDC_GAMECOMMENT,static,1342308352
Control10=IDC_STATIC,button,1342177287

[CLS:GameChestDlg]
Type=0
HeaderFile=GameChestDlg.h
ImplementationFile=GameChestDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_GAMELIST
VirtualFilter=dWC


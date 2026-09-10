#ifndef THANDOR_GENERATED_V523_TYPES_H
#define THANDOR_GENERATED_V523_TYPES_H

#include <stdint.h>
typedef uint32_t pointer32;
typedef uint32_t undefined3;
typedef void code;

#define FIELD_GRID_WORLD_Y_Q20_MULTIPLIER 0x1c6e9c
#define FIELD_GRID_WORLD_X_Q21_MULTIPLIER_NEG 0xffdf3734
#define GLIDE_QUERY_SELECTOR_0x13 0x13
#define ARENA_HEAP_FAILURE_SENTINEL_0x13 0x13
#define GRAPHICS_ADAPTER_ENUMERATION_FAILURE_SENTINEL_0x17 0x17
#define GRAPHICS_CAPTURE_FAILURE_SENTINEL_0x1B 0x1b
#define GRAPHICS_TEXTURE_METADATA_FAILURE_SENTINEL_0x2F 0x2f
#define TEXT_RESOURCE_MISSING_SENTINEL_0x33 0x33
#define ROM_ASSET_REGISTRATION_FAILURE_SENTINEL_0x3B 0x3b
#define MODEL_DEFINITION_REFERENCE_FAILURE_SENTINEL_0x4B 0x4b
#define TECHNOLOGY_ASSET_FAILURE_SENTINEL_0x4F 0x4f
#define WIN32_IDC_ARROW_RESOURCE_ID 0x7f00

typedef unsigned char   undefined;

typedef unsigned char    bool;
typedef unsigned char    byte;
typedef unsigned int    dword;
typedef pointer32 ImageBaseOffset32;

typedef long long    longlong;
typedef unsigned long long    qword;
typedef char    sbyte;
typedef int    sdword;
typedef unsigned int    uint;
typedef unsigned long    ulong;
typedef unsigned long long    ulonglong;
typedef unsigned char    undefined1;
typedef unsigned short    undefined2;
typedef unsigned int    undefined4;
typedef unsigned long long    undefined8;
typedef unsigned short    ushort;
typedef short    wchar_t;
typedef unsigned short    word;
typedef union IMAGE_RESOURCE_DIRECTORY_ENTRY_DirectoryUnion IMAGE_RESOURCE_DIRECTORY_ENTRY_DirectoryUnion, *PIMAGE_RESOURCE_DIRECTORY_ENTRY_DirectoryUnion;

typedef struct IMAGE_RESOURCE_DIRECTORY_ENTRY_DirectoryStruct IMAGE_RESOURCE_DIRECTORY_ENTRY_DirectoryStruct, *PIMAGE_RESOURCE_DIRECTORY_ENTRY_DirectoryStruct;

struct IMAGE_RESOURCE_DIRECTORY_ENTRY_DirectoryStruct {
    dword OffsetToDirectory:31;
    dword DataIsDirectory:1;
};

union IMAGE_RESOURCE_DIRECTORY_ENTRY_DirectoryUnion {
    dword OffsetToData;
    struct IMAGE_RESOURCE_DIRECTORY_ENTRY_DirectoryStruct IMAGE_RESOURCE_DIRECTORY_ENTRY_DirectoryStruct;
};

typedef unsigned short    wchar16;
typedef ulong DWORD;

typedef DWORD LCTYPE;

typedef struct _OVERLAPPED _OVERLAPPED, *P_OVERLAPPED;

typedef ulong ULONG_PTR;

typedef union _union_518 _union_518, *P_union_518;

typedef void *HANDLE;

typedef struct _struct_519 _struct_519, *P_struct_519;

typedef void *PVOID;

struct _struct_519 {
    DWORD Offset;
    DWORD OffsetHigh;
};

union _union_518 {
    struct _struct_519 s;
    PVOID Pointer;
};

struct _OVERLAPPED {
    ULONG_PTR Internal;
    ULONG_PTR InternalHigh;
    union _union_518 u;
    HANDLE hEvent;
};

typedef struct _SECURITY_ATTRIBUTES _SECURITY_ATTRIBUTES, *P_SECURITY_ATTRIBUTES;

typedef void *LPVOID;

typedef int BOOL;

struct _SECURITY_ATTRIBUTES {
    DWORD nLength;
    LPVOID lpSecurityDescriptor;
    BOOL bInheritHandle;
};

typedef struct _SYSTEMTIME _SYSTEMTIME, *P_SYSTEMTIME;

typedef ushort WORD;

struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
};

typedef struct _WIN32_FIND_DATAA _WIN32_FIND_DATAA, *P_WIN32_FIND_DATAA;

typedef struct _FILETIME _FILETIME, *P_FILETIME;

typedef struct _FILETIME FILETIME;

typedef char CHAR;

struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
};

struct _WIN32_FIND_DATAA {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    CHAR cFileName[260];
    CHAR cAlternateFileName[14];
};

typedef struct _OVERLAPPED *LPOVERLAPPED;

typedef DWORD (*PTHREAD_START_ROUTINE)(LPVOID);

typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;

typedef struct _SECURITY_ATTRIBUTES *LPSECURITY_ATTRIBUTES;

typedef struct _WIN32_FIND_DATAA *LPWIN32_FIND_DATAA;

typedef struct _SYSTEMTIME *LPSYSTEMTIME;

typedef struct _COORD _COORD, *P_COORD;

typedef struct _COORD COORD;

typedef short SHORT;

struct _COORD {
    SHORT X;
    SHORT Y;
};

typedef struct _SMALL_RECT _SMALL_RECT, *P_SMALL_RECT;

typedef struct _SMALL_RECT SMALL_RECT;

struct _SMALL_RECT {
    SHORT Left;
    SHORT Top;
    SHORT Right;
    SHORT Bottom;
};

typedef struct _CONSOLE_SCREEN_BUFFER_INFO _CONSOLE_SCREEN_BUFFER_INFO, *P_CONSOLE_SCREEN_BUFFER_INFO;

struct _CONSOLE_SCREEN_BUFFER_INFO {
    COORD dwSize;
    COORD dwCursorPosition;
    WORD wAttributes;
    SMALL_RECT srWindow;
    COORD dwMaximumWindowSize;
};

typedef struct _CONSOLE_READCONSOLE_CONTROL _CONSOLE_READCONSOLE_CONTROL, *P_CONSOLE_READCONSOLE_CONTROL;

typedef struct _CONSOLE_READCONSOLE_CONTROL *PCONSOLE_READCONSOLE_CONTROL;

typedef ulong ULONG;

struct _CONSOLE_READCONSOLE_CONTROL {
    ULONG nLength;
    ULONG nInitialChars;
    ULONG dwCtrlWakeupMask;
    ULONG dwControlKeyState;
};

typedef struct _CONSOLE_SCREEN_BUFFER_INFO *PCONSOLE_SCREEN_BUFFER_INFO;

typedef char *va_list;

typedef struct tagMSG tagMSG, *PtagMSG;

typedef struct tagMSG MSG;

typedef struct HWND__ HWND__, *PHWND__;

typedef struct HWND__ *HWND;

typedef uint UINT;

typedef uint UINT_PTR;

typedef UINT_PTR WPARAM;

typedef long LONG_PTR;

typedef LONG_PTR LPARAM;

typedef struct tagPOINT tagPOINT, *PtagPOINT;

typedef struct tagPOINT POINT;

typedef long LONG;

struct tagPOINT {
    LONG x;
    LONG y;
};

struct tagMSG {
    HWND hwnd;
    UINT message;
    WPARAM wParam;
    LPARAM lParam;
    DWORD time;
    POINT pt;
};

struct HWND__ {
    int unused;
};

typedef struct tagWNDCLASSA tagWNDCLASSA, *PtagWNDCLASSA;

typedef LONG_PTR LRESULT;

typedef LRESULT (*WNDPROC)(HWND, UINT, WPARAM, LPARAM);

typedef struct HINSTANCE__ HINSTANCE__, *PHINSTANCE__;

typedef struct HINSTANCE__ *HINSTANCE;

typedef struct HICON__ HICON__, *PHICON__;

typedef struct HICON__ *HICON;

typedef HICON HCURSOR;

typedef struct HBRUSH__ HBRUSH__, *PHBRUSH__;

typedef struct HBRUSH__ *HBRUSH;

typedef CHAR *LPCSTR;

struct HBRUSH__ {
    int unused;
};

struct tagWNDCLASSA {
    UINT style;
    WNDPROC lpfnWndProc;
    int cbClsExtra;
    int cbWndExtra;
    HINSTANCE hInstance;
    HICON hIcon;
    HCURSOR hCursor;
    HBRUSH hbrBackground;
    LPCSTR lpszMenuName;
    LPCSTR lpszClassName;
};

struct HICON__ {
    int unused;
};

struct HINSTANCE__ {
    int unused;
};

typedef struct tagMSG *LPMSG;

typedef struct tagWNDCLASSA WNDCLASSA;

typedef LONG *PLONG;

typedef CHAR *LPSTR;

typedef DWORD LCID;

typedef struct IMAGE_DOS_HEADER IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

struct IMAGE_DOS_HEADER {
    char e_magic[2]; 
    word e_cblp; 
    word e_cp; 
    word e_crlc; 
    word e_cparhdr; 
    word e_minalloc; 
    word e_maxalloc; 
    word e_ss; 
    word e_sp; 
    word e_csum; 
    word e_ip; 
    word e_cs; 
    word e_lfarlc; 
    word e_ovno; 
    word e_res[4][4]; 
    word e_oemid; 
    word e_oeminfo; 
    word e_res2[10][10]; 
    dword e_lfanew; 
    byte e_program[64]; 
};

typedef union ResourceRegistrationRuntimePayloadReference4 ResourceRegistrationRuntimePayloadReference4, *PResourceRegistrationRuntimePayloadReference4;

typedef struct ArmyRuntimeSlot ArmyRuntimeSlot, *PArmyRuntimeSlot;

typedef struct EffectRuntimeSlot EffectRuntimeSlot, *PEffectRuntimeSlot;

typedef struct ShotRuntimeSlot ShotRuntimeSlot, *PShotRuntimeSlot;

typedef struct ModelRuntimeNode ModelRuntimeNode, *PModelRuntimeNode;

typedef struct GameEntityRuntime GameEntityRuntime, *PGameEntityRuntime;

typedef int FactionRuntimeIndex;

typedef struct ArmyRuntimeMovementControlState ArmyRuntimeMovementControlState, *PArmyRuntimeMovementControlState;

typedef dword ArmyMovementStateFlags;

typedef int Q12;

typedef dword ArmyCommandModeFlags;

typedef dword ArmyCommandGeneration;

typedef dword FieldGridRegionMask;

typedef enum ModelRuntimeClassId {
    MODEL_RUNTIME_CLASS_00=0,
    MODEL_RUNTIME_CLASS_01_GROUND=1,
    MODEL_RUNTIME_CLASS_02_TRACKED=2,
    MODEL_RUNTIME_CLASS_03_ARTICULATED_WALKER=3,
    MODEL_RUNTIME_CLASS_04=4,
    MODEL_RUNTIME_CLASS_05=5,
    MODEL_RUNTIME_CLASS_06=6,
    MODEL_RUNTIME_CLASS_07=7,
    MODEL_RUNTIME_CLASS_08=8,
    MODEL_RUNTIME_CLASS_09=9,
    MODEL_RUNTIME_CLASS_10_CONTINUOUS_RADAR=10,
    MODEL_RUNTIME_CLASS_11=11,
    MODEL_RUNTIME_CLASS_12=12,
    MODEL_RUNTIME_CLASS_13=13,
    MODEL_RUNTIME_CLASS_14=14,
    MODEL_RUNTIME_CLASS_15=15,
    MODEL_RUNTIME_CLASS_16=16,
    MODEL_RUNTIME_CLASS_17_DEPLOYING_GLIDER=17,
    MODEL_RUNTIME_CLASS_18=18,
    MODEL_RUNTIME_CLASS_19_WATER_SURFACE=19,
    MODEL_RUNTIME_CLASS_20=20,
    MODEL_RUNTIME_CLASS_21_AIRCRAFT=21,
    MODEL_RUNTIME_CLASS_22=22,
    MODEL_RUNTIME_CLASS_23=23
} ModelRuntimeClassId;

typedef enum PckArmyAssetIdCatalog {
    ARM_0001_UNIT_MDL0100=1,
    ARM_0002_UNIT_MDL0100=2,
    ARM_0003_UNIT_MDL0100=3,
    ARM_0010_UNIT_MDL0101=10,
    ARM_0011_UNIT_MDL0101=11,
    ARM_0012_UNIT_MDL0101=12,
    ARM_0013_UNIT_MDL0101=13,
    ARM_0014_UNIT_MDL0101=14,
    ARM_0015_UNIT_MDL0101=15,
    ARM_0016_UNIT_MDL0101=16,
    ARM_0017_UNIT_MDL0101=17,
    ARM_0018_UNIT_MDL0101=18,
    ARM_0019_UNIT_MDL0101=19,
    ARM_0030_UNIT_MDL0102=30,
    ARM_0031_UNIT_MDL0102=31,
    ARM_0032_UNIT_MDL0102=32,
    ARM_0033_UNIT_MDL0102=33,
    ARM_0034_UNIT_MDL0102=34,
    ARM_0035_UNIT_MDL0102=35,
    ARM_0036_UNIT_MDL0102=36,
    ARM_0037_UNIT_MDL0102=37,
    ARM_0038_UNIT_MDL0102=38,
    ARM_0039_UNIT_MDL0102=39,
    ARM_0050_UNIT_MDL0103=50,
    ARM_0060_UNIT_MDL0111=60,
    ARM_0061_UNIT_MDL0111=61,
    ARM_0062_UNIT_MDL0111=62,
    ARM_0063_UNIT_MDL0111=63,
    ARM_0064_UNIT_MDL0111=64,
    ARM_0065_UNIT_MDL0111=65,
    ARM_0066_UNIT_MDL0111=66,
    ARM_0067_UNIT_MDL0111=67,
    ARM_0080_UNIT_MDL0112=80,
    ARM_0081_UNIT_MDL0112=81,
    ARM_0082_UNIT_MDL0112=82,
    ARM_0083_UNIT_MDL0112=83,
    ARM_0084_UNIT_MDL0112=84,
    ARM_0085_UNIT_MDL0112=85,
    ARM_0086_UNIT_MDL0112=86,
    ARM_0087_UNIT_MDL0112=87,
    ARM_0088_UNIT_MDL0112=88,
    ARM_0089_UNIT_MDL0112=89,
    ARM_0100_UNIT_MDL0113=100,
    ARM_0101_UNIT_MDL0113=101,
    ARM_0102_UNIT_MDL0113=102,
    ARM_0103_UNIT_MDL0113=103,
    ARM_0104_UNIT_MDL0113=104,
    ARM_0105_UNIT_MDL0113=105,
    ARM_0110_UNIT_MDL0120=110,
    ARM_0111_UNIT_MDL0120=111,
    ARM_0112_UNIT_MDL0120=112,
    ARM_0113_UNIT_MDL0120=113,
    ARM_0114_UNIT_MDL0120=114,
    ARM_0115_UNIT_MDL0120=115,
    ARM_0116_UNIT_MDL0120=116,
    ARM_0117_UNIT_MDL0120=117,
    ARM_0130_UNIT_MDL0121=130,
    ARM_0131_UNIT_MDL0121=131,
    ARM_0132_UNIT_MDL0121=132,
    ARM_0133_UNIT_MDL0121=133,
    ARM_0134_UNIT_MDL0121=134,
    ARM_0135_UNIT_MDL0121=135,
    ARM_0136_UNIT_MDL0121=136,
    ARM_0137_UNIT_MDL0121=137,
    ARM_0138_UNIT_MDL0121=138,
    ARM_0139_UNIT_MDL0121=139,
    ARM_0140_UNIT_MDL0121=140,
    ARM_0141_UNIT_MDL0121=141,
    ARM_0150_UNIT_MDL0122=150,
    ARM_0151_UNIT_MDL0122=151,
    ARM_0152_UNIT_MDL0122=152,
    ARM_0153_UNIT_MDL0122=153,
    ARM_0154_UNIT_MDL0122=154,
    ARM_0155_UNIT_MDL0122=155,
    ARM_0156_UNIT_MDL0122=156,
    ARM_0157_UNIT_MDL0122=157,
    ARM_0170_UNIT_MDL0130=170,
    ARM_0171_UNIT_MDL0130=171,
    ARM_0172_UNIT_MDL0130=172,
    ARM_0180_UNIT_MDL0132=180,
    ARM_0181_UNIT_MDL0132=181,
    ARM_0182_UNIT_MDL0132=182,
    ARM_0183_UNIT_MDL0132=183,
    ARM_0184_UNIT_MDL0132=184,
    ARM_0185_UNIT_MDL0132=185,
    ARM_0186_UNIT_MDL0132=186,
    ARM_0187_UNIT_MDL0132=187,
    ARM_0188_UNIT_MDL0132=188,
    ARM_0200_UNIT_MDL0131=200,
    ARM_0201_UNIT_MDL0131=201,
    ARM_0202_UNIT_MDL0131=202,
    ARM_0203_UNIT_MDL0131=203,
    ARM_0204_UNIT_MDL0131=204,
    ARM_0205_UNIT_MDL0131=205,
    ARM_0206_UNIT_MDL0131=206,
    ARM_0207_UNIT_MDL0131=207,
    ARM_0208_UNIT_MDL0131=208,
    ARM_0220_UNIT_MDL0133=220,
    ARM_0221_UNIT_MDL0133=221,
    ARM_0222_UNIT_MDL0133=222,
    ARM_0223_UNIT_MDL0133=223,
    ARM_0224_UNIT_MDL0133=224,
    ARM_0240_UNIT_MDL0143=240,
    ARM_0242_UNIT_MDL0142=242,
    ARM_0244_UNIT_MDL0141=244,
    ARM_0250_UNIT_MDL0150=250,
    ARM_0251_UNIT_MDL0150=251,
    ARM_0252_UNIT_MDL0150=252,
    ARM_0260_UNIT_MDL0152=260,
    ARM_0261_UNIT_MDL0152=261,
    ARM_0262_UNIT_MDL0152=262,
    ARM_0263_UNIT_MDL0152=263,
    ARM_0264_UNIT_MDL0152=264,
    ARM_0265_UNIT_MDL0152=265,
    ARM_0266_UNIT_MDL0152=266,
    ARM_0267_UNIT_MDL0152=267,
    ARM_0268_UNIT_MDL0152=268,
    ARM_0269_UNIT_MDL0152=269,
    ARM_0270_UNIT_MDL0152=270,
    ARM_0271_UNIT_MDL0152=271,
    ARM_0272_UNIT_MDL0152=272,
    ARM_0273_UNIT_MDL0152=273,
    ARM_0274_UNIT_MDL0152=274,
    ARM_0275_UNIT_MDL0152=275,
    ARM_0280_UNIT_MDL0151=280,
    ARM_0281_UNIT_MDL0151=281,
    ARM_0282_UNIT_MDL0151=282,
    ARM_0283_UNIT_MDL0151=283,
    ARM_0284_UNIT_MDL0151=284,
    ARM_0285_UNIT_MDL0151=285,
    ARM_0295_UNIT_MDL0153=295,
    ARM_0300_BUILDING_MDL0301=300,
    ARM_0301_BUILDING_MDL0318=301,
    ARM_0302_BUILDING_MDL0300=302,
    ARM_0303_BUILDING_MDL0316=303,
    ARM_0304_BUILDING_MDL0324=304,
    ARM_0305_BUILDING_MDL0317=305,
    ARM_0306_BUILDING_MDL0310=306,
    ARM_0310_BUILDING_MDL0305=310,
    ARM_0318_KULISSE_MDL0324=318,
    ARM_0320_BUILDING_MDL0311=320,
    ARM_0321_BUILDING_MDL0326=321,
    ARM_0322_BUILDING_MDL0327=322,
    ARM_0323_BUILDING_MDL0328=323,
    ARM_0330_BUILDING_MDL0303=330,
    ARM_0331_BUILDING_MDL0308=331,
    ARM_0332_BUILDING_MDL0302=332,
    ARM_0333_BUILDING_MDL0307=333,
    ARM_0340_BUILDING_MDL0314=340,
    ARM_0345_BUILDING_MDL0315=345,
    ARM_0350_BUILDING_MDL0322=350,
    ARM_0351_BUILDING_MDL0322=351,
    ARM_0352_BUILDING_MDL0322=352,
    ARM_0357_KULISSE_MDL0357=357,
    ARM_0358_KULISSE_MDL0358=358,
    ARM_0359_KULISSE_MDL0359=359,
    ARM_0360_BUILDING_MDL0314=360,
    ARM_0361_BUILDING_MDL0314=361,
    ARM_0362_BUILDING_MDL0314=362,
    ARM_0363_BUILDING_MDL0314=363,
    ARM_0364_BUILDING_MDL0314=364,
    ARM_0365_BUILDING_MDL0314=365,
    ARM_0366_BUILDING_MDL0314=366,
    ARM_0370_BUILDING_MDL0312=370,
    ARM_0371_BUILDING_MDL0312=371,
    ARM_0372_BUILDING_MDL0312=372,
    ARM_0373_BUILDING_MDL0312=373,
    ARM_0374_BUILDING_MDL0312=374,
    ARM_0375_BUILDING_MDL0312=375,
    ARM_0376_BUILDING_MDL0312=376,
    ARM_0377_BUILDING_MDL0312=377,
    ARM_0380_BUILDING_MDL0329=380,
    ARM_0381_BUILDING_MDL0319=381,
    ARM_0382_BUILDING_MDL0320=382,
    ARM_0390_BUILDING_MDL0321=390,
    ARM_0391_BUILDING_MDL0325=391,
    ARM_0392_BUILDING_MDL0309=392,
    ARM_0395_BUILDING_MDL0323=395,
    ARM_0400_RUINEN_MDL0400=400,
    ARM_0401_RUINEN_MDL0401=401,
    ARM_0402_RUINEN_MDL0402=402,
    ARM_0403_RUINEN_MDL0403=403,
    ARM_0404_RUINEN_MDL0404=404,
    ARM_0405_RUINEN_MDL0405=405,
    ARM_0406_RUINEN_MDL0406=406,
    ARM_0407_RUINEN_MDL0407=407,
    ARM_0408_RUINEN_MDL0408=408,
    ARM_0409_RUINEN_MDL0409=409,
    ARM_0410_RUINEN_MDL0410=410,
    ARM_0411_RUINEN_MDL0411=411,
    ARM_0412_RUINEN_MDL0412=412,
    ARM_0413_RUINEN_MDL0413=413,
    ARM_0414_RUINEN_MDL0414=414,
    ARM_0415_RUINEN_MDL0415=415,
    ARM_0416_RUINEN_MDL0416=416,
    ARM_0417_RUINEN_MDL0417=417,
    ARM_0418_RUINEN_MDL0418=418,
    ARM_0419_RUINEN_MDL0419=419,
    ARM_0420_RUINEN_MDL0420=420,
    ARM_0421_RUINEN_MDL0421=421,
    ARM_0422_RUINEN_MDL0422=422,
    ARM_0423_RUINEN_MDL0423=423,
    ARM_0424_RUINEN_MDL0424=424,
    ARM_0425_RUINEN_MDL0425=425,
    ARM_0426_RUINEN_MDL0426=426,
    ARM_0427_RUINEN_MDL0427=427,
    ARM_0428_RUINEN_MDL0428=428,
    ARM_0429_RUINEN_MDL0429=429,
    ARM_0430_RUINEN_MDL0430=430,
    ARM_0431_RUINEN_MDL0431=431,
    ARM_0432_RUINEN_MDL0432=432,
    ARM_0433_RUINEN_MDL0433=433,
    ARM_0434_RUINEN_MDL0434=434,
    ARM_0435_RUINEN_MDL0435=435,
    ARM_0436_RUINEN_MDL0436=436,
    ARM_0437_RUINEN_MDL0437=437,
    ARM_0438_RUINEN_MDL0438=438,
    ARM_0439_RUINEN_MDL0439=439,
    ARM_0440_RUINEN_MDL0440=440,
    ARM_0500_LBAUM_MDL0500=500,
    ARM_0501_LBAUM_MDL0501=501,
    ARM_0502_LBAUM_MDL0502=502,
    ARM_0503_LBAUM_MDL0503=503,
    ARM_0504_LBAUM_MDL0504=504,
    ARM_0505_LBAUM_MDL0505=505,
    ARM_0506_LBAUM_MDL0506=506,
    ARM_0507_LBAUM_MDL0507=507,
    ARM_0540_NBAUM_MDL0540=540,
    ARM_0541_NBAUM_MDL0541=541,
    ARM_0542_NBAUM_MDL0542=542,
    ARM_0543_NBAUM_MDL0543=543,
    ARM_0544_NBAUM_MDL0544=544,
    ARM_0545_NBAUM_MDL0545=545,
    ARM_0546_NBAUM_MDL0546=546,
    ARM_0547_NBAUM_MDL0547=547,
    ARM_0548_NBAUM_MDL0548=548,
    ARM_0549_NBAUM_MDL0549=549,
    ARM_0550_NBAUM_MDL0550=550,
    ARM_0551_NBAUM_MDL0551=551,
    ARM_0552_NBAUM_MDL0552=552,
    ARM_0553_NBAUM_MDL0553=553,
    ARM_0554_NBAUM_MDL0554=554,
    ARM_0555_NBAUM_MDL0555=555,
    ARM_0556_NBAUM_MDL0556=556,
    ARM_0557_NBAUM_MDL0557=557,
    ARM_0558_NBAUM_MDL0558=558,
    ARM_0559_NBAUM_MDL0559=559,
    ARM_0560_NBAUM_MDL0560=560,
    ARM_0561_NBAUM_MDL0561=561,
    ARM_0562_NBAUM_MDL0562=562,
    ARM_0563_NBAUM_MDL0563=563,
    ARM_0580_PALMEN_MDL0580=580,
    ARM_0581_PALMEN_MDL0581=581,
    ARM_0582_PALMEN_MDL0582=582,
    ARM_0583_PALMEN_MDL0583=583,
    ARM_0584_PALMEN_MDL0584=584,
    ARM_0585_PALMEN_MDL0585=585,
    ARM_0586_PALMEN_MDL0586=586,
    ARM_0587_PALMEN_MDL0587=587,
    ARM_0588_PALMEN_MDL0588=588,
    ARM_0620_STEIN_MDL0620=620,
    ARM_0621_STEIN_MDL0621=621,
    ARM_0622_STEIN_MDL0622=622,
    ARM_0623_STEIN_MDL0623=623,
    ARM_0624_STEIN_MDL0624=624,
    ARM_0625_STEIN_MDL0625=625,
    ARM_0626_STEIN_MDL0626=626,
    ARM_0627_STEIN_MDL0627=627,
    ARM_0628_STEIN_MDL0628=628,
    ARM_0629_STEIN_MDL0629=629,
    ARM_0630_STEIN_MDL0630=630,
    ARM_0631_STEIN_MDL0631=631,
    ARM_0632_STEIN_MDL0632=632,
    ARM_0633_STEIN_MDL0633=633,
    ARM_0634_STEIN_MDL0634=634,
    ARM_0635_STEIN_MDL0635=635,
    ARM_0636_STEIN_MDL0636=636,
    ARM_0637_STEIN_MDL0637=637,
    ARM_0638_STEIN_MDL0638=638,
    ARM_0639_STEIN_MDL0639=639,
    ARM_0640_STEIN_MDL0640=640,
    ARM_0641_STEIN_MDL0641=641,
    ARM_0642_STEIN_MDL0642=642,
    ARM_0647_STEIN_MDL0647=647,
    ARM_0648_STEIN_MDL0648=648,
    ARM_0649_STEIN_MDL0649=649,
    ARM_0650_STEIN_MDL0650=650,
    ARM_0651_STEIN_MDL0651=651,
    ARM_0652_STEIN_MDL0652=652,
    ARM_0653_STEIN_MDL0653=653,
    ARM_0654_STEIN_MDL0654=654,
    ARM_0655_STEIN_MDL0655=655,
    ARM_0656_STEIN_MDL0656=656,
    ARM_0657_STEIN_MDL0657=657,
    ARM_0658_STEIN_MDL0658=658,
    ARM_0660_BUSCH_MDL0660=660,
    ARM_0661_BUSCH_MDL0661=661,
    ARM_0662_BUSCH_MDL0662=662,
    ARM_0663_BUSCH_MDL0663=663,
    ARM_0664_BUSCH_MDL0664=664,
    ARM_0665_BUSCH_MDL0665=665,
    ARM_0710_FARNE_MDL0710=710,
    ARM_0711_FARNE_MDL0711=711,
    ARM_0712_FARNE_MDL0712=712,
    ARM_0713_FARNE_MDL0713=713,
    ARM_0750_KAKTUS_MDL0750=750,
    ARM_0751_KAKTUS_MDL0751=751,
    ARM_0752_KAKTUS_MDL0752=752,
    ARM_0800_ROHSTOFF_MDL0800=800,
    ARM_0801_ROHSTOFF_MDL0801=801,
    ARM_0802_ROHSTOFF_MDL0802=802,
    ARM_0803_ROHSTOFF_MDL0804=803,
    ARM_0805_ROHSTOFF_MDL0805=805,
    ARM_0806_ROHSTOFF_MDL0806=806,
    ARM_0807_ROHSTOFF_MDL0807=807,
    ARM_0808_ROHSTOFF_MDL0808=808,
    ARM_0810_ROHSTOFF_MDL0809=810,
    ARM_0811_ROHSTOFF_MDL0810=811,
    ARM_0814_ROHSTOFF_MDL0811=814,
    ARM_0815_ROHSTOFF_MDL0812=815,
    ARM_0850_ROHSTOFF_MDL0850=850,
    ARM_0851_ROHSTOFF_MDL0851=851,
    ARM_0852_ROHSTOFF_MDL0852=852,
    ARM_0853_ROHSTOFF_MDL0853=853,
    ARM_0854_ROHSTOFF_MDL0854=854,
    ARM_0855_ROHSTOFF_MDL0855=855
} PckArmyAssetIdCatalog;

typedef struct ArmyRuntimeArticulatedContactState14 ArmyRuntimeArticulatedContactState14, *PArmyRuntimeArticulatedContactState14;

typedef struct ArmyRuntimeLinkedChildOverloadedState10 ArmyRuntimeLinkedChildOverloadedState10, *PArmyRuntimeLinkedChildOverloadedState10;

typedef struct ArmyRuntimeLinkedChildSpawnParameters0C ArmyRuntimeLinkedChildSpawnParameters0C, *PArmyRuntimeLinkedChildSpawnParameters0C;

typedef struct ArmyRuntimeLinkedChildPendingCounts4 ArmyRuntimeLinkedChildPendingCounts4, *PArmyRuntimeLinkedChildPendingCounts4;

typedef dword ArmyRuntimeFlags;

typedef dword ArmyRuntimeTimer;

typedef int ArmySelectionMetric;

typedef union EffectDefinitionReferenceOrSavedId4 EffectDefinitionReferenceOrSavedId4, *PEffectDefinitionReferenceOrSavedId4;

typedef union EffectModelNodeReferenceOrSavedOffset4 EffectModelNodeReferenceOrSavedOffset4, *PEffectModelNodeReferenceOrSavedOffset4;

typedef dword EffectAnimationFrameCount;

typedef dword DefinitionReferencePresentFlag;

typedef struct EffectRuntimeLifecycleState10 EffectRuntimeLifecycleState10, *PEffectRuntimeLifecycleState10;

typedef enum EffectRuntimeCompletionAction {
    EFFECT_RUNTIME_COMPLETION_NONE=0,
    EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY=1,
    EFFECT_RUNTIME_COMPLETION_INVOKE_LINKED_HANDLER=2,
    EFFECT_RUNTIME_COMPLETION_SPAWN_ARMY_FROM_MODEL=3
} EffectRuntimeCompletionAction;

typedef dword EffectShadingCountdownTicks;

typedef dword EffectPeriodicIntervalTicks;

typedef dword PackedArgb32;

typedef dword EffectAgeTicks;

typedef union ShotDefinitionReferenceOrSavedId4 ShotDefinitionReferenceOrSavedId4, *PShotDefinitionReferenceOrSavedId4;

typedef dword ShotAnimationFrameAccumulatorQ4;

typedef union ShotModelNodeReferenceOrSavedOffset4 ShotModelNodeReferenceOrSavedOffset4, *PShotModelNodeReferenceOrSavedOffset4;

typedef union ShotModelRuntimeStateOrSavedOffset4 ShotModelRuntimeStateOrSavedOffset4, *PShotModelRuntimeStateOrSavedOffset4;

typedef int AngleTurn16Stored32;

typedef dword ShotImpactEffectEmissionFlags;

typedef dword ShotProjectileAgeTicks;

typedef dword ShotLifetimeRemainingTicks;

typedef struct ShotRuntimeOwnerAndTrajectoryLiveState18 ShotRuntimeOwnerAndTrajectoryLiveState18, *PShotRuntimeOwnerAndTrajectoryLiveState18;

typedef struct WorldRuntimeNodeCommon WorldRuntimeNodeCommon, *PWorldRuntimeNodeCommon;

typedef struct WorldRuntimeNodeModelPayload WorldRuntimeNodeModelPayload, *PWorldRuntimeNodeModelPayload;

typedef union ModelRuntimePayloadReference4 ModelRuntimePayloadReference4, *PModelRuntimePayloadReference4;

typedef dword ModelRuntimeFlags;

typedef int ModelTextureSubresourceIndex;

typedef struct GraphicsShadingRuntimeRecord GraphicsShadingRuntimeRecord, *PGraphicsShadingRuntimeRecord;

typedef int ModelTextureOffsetTexel;

typedef struct GraphicsFixedMatrix3x4 GraphicsFixedMatrix3x4, *PGraphicsFixedMatrix3x4;

typedef dword ModelDepthBinMask;

typedef struct GameEntityRuntimeCommon GameEntityRuntimeCommon, *PGameEntityRuntimeCommon;

typedef union GameEntityRuntimeClassPayload GameEntityRuntimeClassPayload, *PGameEntityRuntimeClassPayload;

typedef int ArmyTurnVelocityAngle16;

typedef enum ArmyTerrainContactDispatchMode {
    ARMY_TERRAIN_CONTACT_ACQUIRE_OR_INITIALIZE_CONTACT_SLOT=0,
    ARMY_TERRAIN_CONTACT_ADVANCE_ACTIVE_CONTACT_AND_RELEASE=1
} ArmyTerrainContactDispatchMode;

typedef union ArmyRuntimeContactRadiusOrLinkedSlotMask4 ArmyRuntimeContactRadiusOrLinkedSlotMask4, *PArmyRuntimeContactRadiusOrLinkedSlotMask4;

typedef union ArmyRuntimeCoordinateCommandOrHistoryValue4 ArmyRuntimeCoordinateCommandOrHistoryValue4, *PArmyRuntimeCoordinateCommandOrHistoryValue4;

typedef struct EffectDefinition EffectDefinition, *PEffectDefinition;

typedef enum PckEffectDefinitionIdCatalog {
    EFF_0001_EGRXA0=1,
    EFF_0002_EGRXB0=2,
    EFF_0003_EGRXC0=3,
    EFF_0004_EGRXC0=4,
    EFF_0005_EGRXG0=5,
    EFF_0006_EGRXD0=6,
    EFF_0007_EGRXE0=7,
    EFF_0008_EGRXF0=8,
    EFF_0009_EPEXA0=9,
    EFF_0010_EGRXH0=10,
    EFF_0020_EPARA0=20,
    EFF_0021_EPARB0=21,
    EFF_0022_EPARA0=22,
    EFF_0023_EPARB0=23,
    EFF_0024_EPARB0=24,
    EFF_0025_EPARC0=25,
    EFF_0030_EHUNA0=30,
    EFF_0031_EHUNB0=31,
    EFF_0032_EHUNC0=32,
    EFF_0033_EHUNB0=33,
    EFF_0034_EHUNA0=34,
    EFF_0035_EHUNC0=35,
    EFF_0036_EHUND0=36,
    EFF_0037_EHUND0=37,
    EFF_0050_EDESA0=50,
    EFF_0051_EDESB0=51,
    EFF_0052_EDESC0=52,
    EFF_0053_EDESD0=53,
    EFF_0054_EDESE0=54,
    EFF_0055_EDESF0=55,
    EFF_0070_EMMGA0=70,
    EFF_0071_EMLAS0=71,
    EFF_0072_EMLAS0=72,
    EFF_0073_EMKAN0=73,
    EFF_0074_EMFLK0=74,
    EFF_0075_EMION0=75,
    EFF_0076_EMLAS0=76,
    EFF_0077_EMFLK0=77,
    EFF_0078_EMKAN0=78,
    EFF_0079_EMKAN0=79,
    EFF_0080_EMRAK0=80,
    EFF_0081_EMTRM0=81,
    EFF_0082_EMECR0=82,
    EFF_0083_EMRAK0=83,
    EFF_0084_EMKAB0=84,
    EFF_0085_EMTRM0=85,
    EFF_0090_EHUNB1=90,
    EFF_0100_EREXH0=100,
    EFF_0101_EREXD0=101,
    EFF_0102_EREXB0=102,
    EFF_0103_EREXH0=103,
    EFF_0104_EREXC0=104,
    EFF_0105_ERARA0=105,
    EFF_0106_ERARA0=106,
    EFF_0107_EREXE0=107,
    EFF_0108_EREXF0=108,
    EFF_0109_EREXG0=109,
    EFF_0110_EREXI0=110,
    EFF_0111_EREXJ0=111,
    EFF_0112_ERNEA0=112,
    EFF_0113_ERNEB0=113,
    EFF_0114_ELTRI0=114,
    EFF_0115_EREXK0=115,
    EFF_0116_ELTRI0=116,
    EFF_0117_EREXI0=117,
    EFF_0118_EREXI0=118,
    EFF_0120_EBAUA0=120,
    EFF_0121_EBAUB0=121,
    EFF_0122_EBAUC0=122,
    EFF_0123_EBAUD0=123,
    EFF_0124_EBAUE0=124,
    EFF_0125_EBAUF0=125,
    EFF_0126_EBAUD0=126,
    EFF_0130_EKKWK0=130,
    EFF_0131_ERARC0=131,
    EFF_0132_EFGLA0=132,
    EFF_0133_EFGLC0=133,
    EFF_0134_EFGLB0=134,
    EFF_0135_EFKFA0=135,
    EFF_0136_EFKFB0=136,
    EFF_0137_EFKFC0=137,
    EFF_0138_EFBTA0=138,
    EFF_0139_EFBTB0=139,
    EFF_0140_EFBTC0=140,
    EFF_0141_EFGLD0=141,
    EFF_0142_ERARB0=142,
    EFF_0143_EGATH0=143,
    EFF_0144_EFWLK0=144,
    EFF_0145_EFWLK0=145,
    EFF_0146_EFWLK0=146,
    EFF_0147_EPEXA0=147,
    EFF_0148_EWAYP0=148,
    EFF_0149_ETARG0=149,
    EFF_0150_ELRAK0=150,
    EFF_0151_ELION0=151,
    EFF_0152_ELPRO0=152,
    EFF_0153_ELFOR0=153,
    EFF_0200_FBNBA0=200,
    EFF_0201_FBNBB0=201,
    EFF_0202_FBNBC0=202,
    EFF_0203_FBNBD0=203,
    EFF_0204_FBNBE0=204,
    EFF_0205_FBNBF0=205,
    EFF_0206_FBNBG0=206,
    EFF_0207_FBNBH0=207,
    EFF_0208_FBNBI0=208,
    EFF_0209_FBNBJ0=209,
    EFF_0210_FBNBK0=210,
    EFF_0211_FBNBL0=211,
    EFF_0212_FBNSA0=212,
    EFF_0213_FBNSB0=213,
    EFF_0214_FBNSC0=214,
    EFF_0215_FBNSD0=215,
    EFF_0216_FBNSE0=216,
    EFF_0217_FBNSF0=217,
    EFF_0218_FBNSG0=218,
    EFF_0219_FBNSH0=219,
    EFF_0220_FBNSI0=220,
    EFF_0221_FBNSJ0=221,
    EFF_0222_FBNSK0=222,
    EFF_0223_FBNSL0=223,
    EFF_0224_FBTBA0=224,
    EFF_0225_EBLBA0=225,
    EFF_0226_FBTBA0=226,
    EFF_0227_EBLBB0=227,
    EFF_0228_FBTBB0=228,
    EFF_0229_EBLBC0=229,
    EFF_0230_FBTBB0=230,
    EFF_0231_FBPLB0=231,
    EFF_0232_FBPLC0=232,
    EFF_0233_FBPLD0=233,
    EFF_0234_FBPLE0=234,
    EFF_0235_FBPLF0=235,
    EFF_0236_FBPLG0=236,
    EFF_0237_FBPLH0=237,
    EFF_0238_FBPLI0=238,
    EFF_0239_FBPLA0=239,
    EFF_0240_ERCKA0=240,
    EFF_0241_ERCKB0=241,
    EFF_0242_ERCKC0=242
} PckEffectDefinitionIdCatalog;

typedef dword EffectAnimationFrameAccumulatorQ4;

typedef struct EffectRuntimeOwnerAndDefinitionState8 EffectRuntimeOwnerAndDefinitionState8, *PEffectRuntimeOwnerAndDefinitionState8;

typedef struct ShotDefinition ShotDefinition, *PShotDefinition;

typedef enum PckShotDefinitionIdCatalog {
    SHT_0000_SLRAY0=0,
    SHT_0001_SSFBA0=1,
    SHT_0002_SPMGW0=2,
    SHT_0003_SPFLK0=3,
    SHT_0004_SPKAN0=4,
    SHT_0005_SSFBB0=5,
    SHT_0006_SIIOA0=6,
    SHT_0007_SSFBC0=7,
    SHT_0008_SSFBC0=8,
    SHT_0009_SPMGW0=9,
    SHT_0010_SLRAB0=10,
    SHT_0011_SRMIA0=11,
    SHT_0012_SRMIB0=12,
    SHT_0013_SRMIC0=13,
    SHT_0014_SPMGW0=14,
    SHT_0015_SPHAU0=15,
    SHT_0016_SPHAU0=16,
    SHT_0017_SPKAB0=17,
    SHT_0018_SLRAB0=18,
    SHT_0019_SLTRM0=19,
    SHT_0020_SRMID0=20,
    SHT_0021_SRECR0=21,
    SHT_0022_SPMGW0=22,
    SHT_0023_SPMGW0=23,
    SHT_0024_SPMGW0=24,
    SHT_0025_SPMGW0=25,
    SHT_0026_SLRAY0=26,
    SHT_0027_SPFLK0=27,
    SHT_0028_SPMGW0=28,
    SHT_0029_SPMGW0=29,
    SHT_0030_SPMGW0=30,
    SHT_0031_SRMIB0=31,
    SHT_0032_SRMIB0=32,
    SHT_0033_SIIOB0=33,
    SHT_1000_SLRAY0=1000,
    SHT_1001_SSFBA0=1001,
    SHT_1002_SPMGW0=1002,
    SHT_1003_SPFLK0=1003,
    SHT_1004_SPKAN0=1004,
    SHT_1005_SSFBB0=1005,
    SHT_1006_SIIOA0=1006,
    SHT_1007_SSFBC0=1007,
    SHT_1008_SSFBC0=1008,
    SHT_1009_SPMGW0=1009,
    SHT_1010_SLRAB0=1010,
    SHT_1011_SRMIA0=1011,
    SHT_1012_SRMIB0=1012,
    SHT_1013_SRMIC0=1013,
    SHT_1014_SPMGW0=1014,
    SHT_1015_SPHAU0=1015,
    SHT_1016_SPHAU0=1016,
    SHT_1017_SPKAB0=1017,
    SHT_1018_SLRAB0=1018,
    SHT_1019_SLTRM0=1019,
    SHT_1020_SRMID0=1020,
    SHT_1021_SRECR0=1021,
    SHT_1022_SPMGW0=1022,
    SHT_1023_SPMGW0=1023,
    SHT_1024_SPMGW0=1024,
    SHT_1025_SPMGW0=1025,
    SHT_1026_SLRAY0=1026,
    SHT_1027_SPFLK0=1027,
    SHT_1028_SPMGW0=1028,
    SHT_1029_SPMGW0=1029,
    SHT_1030_SPMGW0=1030,
    SHT_1031_SRMIB0=1031,
    SHT_1032_SRMIB0=1032,
    SHT_1033_SIIOB0=1033,
    SHT_2000_SLRAY0=2000,
    SHT_2001_SSFBA0=2001,
    SHT_2002_SPMGW0=2002,
    SHT_2003_SPFLK0=2003,
    SHT_2004_SPKAN0=2004,
    SHT_2005_SSFBB0=2005,
    SHT_2006_SIIOA0=2006,
    SHT_2007_SSFBC0=2007,
    SHT_2008_SSFBC0=2008,
    SHT_2009_SPMGW0=2009,
    SHT_2010_SLRAB0=2010,
    SHT_2011_SRMIA0=2011,
    SHT_2012_SRMIB0=2012,
    SHT_2013_SRMIC0=2013,
    SHT_2014_SPMGW0=2014,
    SHT_2015_SPHAU0=2015,
    SHT_2016_SPHAU0=2016,
    SHT_2017_SPKAB0=2017,
    SHT_2018_SLRAB0=2018,
    SHT_2019_SLTRM0=2019,
    SHT_2020_SRMID0=2020,
    SHT_2021_SRECR0=2021,
    SHT_2022_SPMGW0=2022,
    SHT_2023_SPMGW0=2023,
    SHT_2024_SPMGW0=2024,
    SHT_2025_SPMGW0=2025,
    SHT_2026_SLRAY0=2026,
    SHT_2027_SPFLK0=2027,
    SHT_2028_SPMGW0=2028,
    SHT_2029_SPMGW0=2029,
    SHT_2030_SPMGW0=2030,
    SHT_2031_SRMIB0=2031,
    SHT_2032_SRMIB0=2032,
    SHT_2033_SIIOB0=2033,
    SHT_3000_SLRAY0=3000,
    SHT_3001_SSFBA0=3001,
    SHT_3002_SPMGW0=3002,
    SHT_3003_SPFLK0=3003,
    SHT_3004_SPKAN0=3004,
    SHT_3005_SSFBB0=3005,
    SHT_3006_SIIOA0=3006,
    SHT_3007_SSFBC0=3007,
    SHT_3008_SSFBC0=3008,
    SHT_3009_SPMGW0=3009,
    SHT_3010_SLRAB0=3010,
    SHT_3011_SRMIA0=3011,
    SHT_3012_SRMIB0=3012,
    SHT_3013_SRMIC0=3013,
    SHT_3014_SPMGW0=3014,
    SHT_3015_SPHAU0=3015,
    SHT_3016_SPHAU0=3016,
    SHT_3017_SPKAB0=3017,
    SHT_3018_SLRAB0=3018,
    SHT_3019_SLTRM0=3019,
    SHT_3020_SRMID0=3020,
    SHT_3021_SRECR0=3021,
    SHT_3022_SPMGW0=3022,
    SHT_3023_SPMGW0=3023,
    SHT_3024_SPMGW0=3024,
    SHT_3025_SPMGW0=3025,
    SHT_3026_SLRAY0=3026,
    SHT_3027_SPFLK0=3027,
    SHT_3028_SPMGW0=3028,
    SHT_3029_SPMGW0=3029,
    SHT_3030_SPMGW0=3030,
    SHT_3031_SRMIB0=3031,
    SHT_3032_SRMIB0=3032,
    SHT_3033_SIIOB0=3033
} PckShotDefinitionIdCatalog;

typedef dword ShotAnimationFrameIndex;

typedef dword ShotSecondaryEffectCountdownTicks;

typedef struct WorldRuntimeNode WorldRuntimeNode, *PWorldRuntimeNode;

typedef struct WorldRuntimeContext WorldRuntimeContext, *PWorldRuntimeContext;

typedef dword AngleTurn32;

typedef struct GraphicsPaletteAsset GraphicsPaletteAsset, *PGraphicsPaletteAsset;

typedef struct GraphicsTextureSet GraphicsTextureSet, *PGraphicsTextureSet;

typedef struct ModelResourceHitTestAndRenderView210 ModelResourceHitTestAndRenderView210, *PModelResourceHitTestAndRenderView210;

typedef dword ModelMeshGroupMask;

typedef struct ModelRuntimeSlot ModelRuntimeSlot, *PModelRuntimeSlot;

typedef int GraphicsWorldCoordinateQ12;

typedef dword PackedRgb24;

typedef int GraphicsTransitionTickCount;

typedef int GraphicsRadiusQ12;

typedef struct GraphicsFixedVec3 GraphicsFixedVec3, *PGraphicsFixedVec3;

typedef struct GameEntityOwnershipState10 GameEntityOwnershipState10, *PGameEntityOwnershipState10;

typedef dword GameEntityCommandFlags;

typedef struct GameEntityCommandTargetState GameEntityCommandTargetState, *PGameEntityCommandTargetState;

typedef struct GameEntityDamageState2C GameEntityDamageState2C, *PGameEntityDamageState2C;

typedef dword RuntimeToken;

typedef dword GameEntityRuntimeFlags;

typedef struct GameEntityPathingAndImpactState10 GameEntityPathingAndImpactState10, *PGameEntityPathingAndImpactState10;

typedef int GameEntityCommandState;

typedef struct GameEntityTechnologyPayload GameEntityTechnologyPayload, *PGameEntityTechnologyPayload;

typedef struct GameEntityImpactOwnerLinksPayloadFC GameEntityImpactOwnerLinksPayloadFC, *PGameEntityImpactOwnerLinksPayloadFC;

typedef dword UQ12;

typedef struct EffectDefinitionTransitionPrefix8 EffectDefinitionTransitionPrefix8, *PEffectDefinitionTransitionPrefix8;

typedef Q12 EffectMovementSpeedQ12;

typedef dword EffectFrameAdvanceThresholdQ4;

typedef int TerrainGridMaskIndex;

typedef enum EffectCreationFlagBits {
    EFFECT_CREATION_RANDOMIZE_ORIENTATION=1,
    EFFECT_CREATION_USE_ARMY_PALETTE_AND_TEXTURE_SET=2
} EffectCreationFlagBits;

typedef dword EffectAlphaFadeTicks;

typedef dword SpatialSoundGainQ15;

typedef dword SpatialSoundMaximumDistanceQ12;

typedef dword OwnedNestedResourceFlag;

typedef union EffectRuntimeOwnerReference4 EffectRuntimeOwnerReference4, *PEffectRuntimeOwnerReference4;

typedef enum ShotTrajectoryMode {
    SHOT_TRAJECTORY_DIRECT_LINE=0,
    SHOT_TRAJECTORY_BALLISTIC=1,
    SHOT_TRAJECTORY_FIXED_RANGE=2,
    SHOT_TRAJECTORY_LEAD_ADJUSTED=3
} ShotTrajectoryMode;

typedef dword ShotProjectileLifetimeTicks;

typedef AngleTurn16Stored32 ShotModelSpinStepTurn16;

typedef dword ShotTerrainImpactEffectOwnerSlot;

typedef dword ShotFrameAdvanceThresholdQ4;

typedef dword ShotAnimationFrameCount;

typedef int TerrainMaterialIndex;

typedef dword ShotSecondaryEffectIntervalTicks;

typedef dword ShotTrajectoryRampDurationTicks;

typedef dword ShotFixedRangeTransitionAgeTicks;

typedef dword EffectLeadDisableFlag;

typedef union WorldRuntimeNodePayload WorldRuntimeNodePayload, *PWorldRuntimeNodePayload;

typedef dword WorldRuntimeNodeFlags;

typedef struct WorldRuntimeInteractionState WorldRuntimeInteractionState, *PWorldRuntimeInteractionState;

typedef dword WorldRuntimeFlags;

typedef struct FieldGridAsset FieldGridAsset, *PFieldGridAsset;

typedef struct WorldObjectRecord WorldObjectRecord, *PWorldObjectRecord;

typedef struct WorldMotionState WorldMotionState, *PWorldMotionState;

typedef dword WorldSurfaceSelectionFlags;

typedef dword WorldObjectRecordCount;

typedef struct WorldFieldRegionState WorldFieldRegionState, *PWorldFieldRegionState;

typedef dword WorldWorkspaceElementCount;

typedef dword WorldRuntimeControlFlags;

typedef struct WorldRuntimeSelectionState WorldRuntimeSelectionState, *PWorldRuntimeSelectionState;

typedef struct WorldLightingState WorldLightingState, *PWorldLightingState;

typedef struct WorldMotionSnapshot WorldMotionSnapshot, *PWorldMotionSnapshot;

typedef enum AssetMagic {
    ASSET_MAGIC_TEC=6514036,
    ASSET_MAGIC_FNC=6516326,
    ASSET_MAGIC_FLD=6581350,
    ASSET_MAGIC_EFF=6710885,
    ASSET_MAGIC_PAL=7102832,
    ASSET_MAGIC_MDL=7103597,
    ASSET_MAGIC_SAM=7168371,
    ASSET_MAGIC_FLM=7171174,
    ASSET_MAGIC_ROM=7171954,
    ASSET_MAGIC_ARM=7172705,
    ASSET_MAGIC_SPR=7499891,
    ASSET_MAGIC_STR=7500915,
    ASSET_MAGIC_SHT=7628915,
    ASSET_MAGIC_LEV=7759212,
    ASSET_MAGIC_GFX=7890535
} AssetMagic;

typedef dword AssetAllocationSizeBytes;

typedef dword AssetPaletteBankCount;

typedef struct GraphicsPaletteAssetEntry GraphicsPaletteAssetEntry, *PGraphicsPaletteAssetEntry;

typedef struct GraphicsTextureSourceAsset GraphicsTextureSourceAsset, *PGraphicsTextureSourceAsset;

typedef struct GraphicsTextureSetEntry GraphicsTextureSetEntry, *PGraphicsTextureSetEntry;

typedef dword ModelMeshGroupCount;

typedef dword ModelPackedLookupTableRelativeOffset;

typedef dword ModelPackedLookupTableEntryCount;

typedef dword ModelMeshGroupRelativeOffset;

typedef dword ModelPackedGeometryRecordCount;

typedef enum ModelResourceHitTestFlags {
    MODEL_RESOURCE_DISABLE_PROJECTED_HIT_TEST=4
} ModelResourceHitTestFlags;

typedef union ModelDefinitionReferenceOrSavedId4 ModelDefinitionReferenceOrSavedId4, *PModelDefinitionReferenceOrSavedId4;

typedef union ModelRuntimeNodeReferenceOrSavedOffset4 ModelRuntimeNodeReferenceOrSavedOffset4, *PModelRuntimeNodeReferenceOrSavedOffset4;

typedef union ArmyRuntimeReferenceOrSavedOffset4 ArmyRuntimeReferenceOrSavedOffset4, *PArmyRuntimeReferenceOrSavedOffset4;

typedef union ModelRuntimeSlotReferenceOrSavedOffset4 ModelRuntimeSlotReferenceOrSavedOffset4, *PModelRuntimeSlotReferenceOrSavedOffset4;

typedef struct ModelRuntimeClassLinkState24 ModelRuntimeClassLinkState24, *PModelRuntimeClassLinkState24;

typedef struct ModelRuntimeSlotClassState7C ModelRuntimeSlotClassState7C, *PModelRuntimeSlotClassState7C;

typedef struct ModelRuntimeAttachmentDescriptor ModelRuntimeAttachmentDescriptor, *PModelRuntimeAttachmentDescriptor;

typedef dword GameEntityCommandTargetFlags;

typedef union GameEntityDamageCounterOrTerminalReference4 GameEntityDamageCounterOrTerminalReference4, *PGameEntityDamageCounterOrTerminalReference4;

typedef struct GameEntityPathingReferenceState8 GameEntityPathingReferenceState8, *PGameEntityPathingReferenceState8;

typedef struct GameEntityImpactReactionBytes8 GameEntityImpactReactionBytes8, *PGameEntityImpactReactionBytes8;

typedef dword TechnologyResearchDurationQ5;

typedef dword TechnologyEnergyCostQ4;

typedef dword TechnologyXeniteCostQ4;

typedef enum EffectLifecycleTransitionKind {
    EFFECT_TRANSITION_SPAWN_LINKED_EFFECT_AFTER_COUNTDOWN=0,
    EFFECT_TRANSITION_ADVANCE_PERIODIC_EMISSION_AND_COMPLETION_ACTION=1,
    EFFECT_TRANSITION_INTEGRATE_LINEAR_MOTION_AND_SHADING_POSITION=2,
    EFFECT_TRANSITION_NO_ADDITIONAL_ACTION=3,
    EFFECT_TRANSITION_ADVANCE_TERRAIN_RELATIVE_MOTION_AND_TERMINATE_ON_CONTACT=4
} EffectLifecycleTransitionKind;

typedef dword WorldInteractionFlags;

typedef struct GeneratedAssetCommonPrefix GeneratedAssetCommonPrefix, *PGeneratedAssetCommonPrefix;

typedef dword FieldGridFlags;

typedef dword FieldGridRuntimeFlags;

typedef dword FieldGridDimension;

typedef struct FieldGridCell FieldGridCell, *PFieldGridCell;

typedef struct WorldObjectRecordCommon WorldObjectRecordCommon, *PWorldObjectRecordCommon;

typedef dword WorldFieldDimension;

typedef dword PlayerRuntimeId;

typedef struct GraphicsTextureSourceTableDescriptor GraphicsTextureSourceTableDescriptor, *PGraphicsTextureSourceTableDescriptor;

typedef struct GraphicsTextureResource GraphicsTextureResource, *PGraphicsTextureResource;

typedef dword GraphicsTextureDimensionLog2;

typedef struct GraphicsTextureSourceEntry GraphicsTextureSourceEntry, *PGraphicsTextureSourceEntry;

typedef dword GraphicsSubresourceIndex;

typedef struct ModelDefinitionRecordPrefix ModelDefinitionRecordPrefix, *PModelDefinitionRecordPrefix;

typedef union ModelRuntimeSlotLinkOrState4 ModelRuntimeSlotLinkOrState4, *PModelRuntimeSlotLinkOrState4;

typedef union ModelRuntimeArmyLinkOrState4 ModelRuntimeArmyLinkOrState4, *PModelRuntimeArmyLinkOrState4;

typedef struct ModelAttachmentTransformRecord ModelAttachmentTransformRecord, *PModelAttachmentTransformRecord;

typedef dword ModelChildNodeIndex;

typedef dword AssetFormatVersion;

typedef enum PckConverterVersion {
    PCK_CONVERTER_ROM_00010005=65541,
    PCK_CONVERTER_TEC_00020000=131072,
    PCK_CONVERTER_SPR_00020007=131079,
    PCK_CONVERTER_ARM_00020008=131080,
    PCK_CONVERTER_EFF_00040007=262151,
    PCK_CONVERTER_FLD_SHT_00060006=393222,
    PCK_CONVERTER_LEV_00070001=458753,
    PCK_CONVERTER_MDL_0008000A=524298
} PckConverterVersion;

typedef struct GeneratedAssetBuildMetadata GeneratedAssetBuildMetadata, *PGeneratedAssetBuildMetadata;

typedef dword PackedTerrainNormalAngles;

typedef enum FieldCellPackedFlagsAndMaterial { 
    FIELD_CELL_MATERIAL_ID_MASK=255,
    FIELD_CELL_RANDOM_VARIANT_MASK=1792,
    FIELD_CELL_XENITE_SUPPORT=2048,
    FIELD_CELL_TRITIUM_SUPPORT=4096,
    FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK=6144,
    FIELD_CELL_FIRST_COLUMN_BOUNDARY=8192,
    FIELD_CELL_FIRST_ROW_BOUNDARY=16384,
    FIELD_CELL_INIT_CLEARED_UNRESOLVED_BIT15=32768,
    FIELD_CELL_CONNECTED_REGION_VISITED=65536,
    FIELD_CELL_LAST_COLUMN_BOUNDARY=134217728,
    FIELD_CELL_TERRAIN_VISUAL_CLEARABLE_UNRESOLVED_BIT28=268435456,
    FIELD_CELL_FLUID_RECEIVER_EXCLUDED=536870912,
    FIELD_CELL_FLUID_SOURCE_EXCLUDED=1073741824
} FieldCellPackedFlagsAndMaterial;

typedef dword FieldCellPersistedAux;

typedef qword TerrainOccupancyMask;

typedef dword WorldObjectAllocationFlags;

typedef dword AssetSubresourceCount;

typedef dword AssetRelativeOffset;

typedef struct IDirectDrawSurface IDirectDrawSurface, *PIDirectDrawSurface;

typedef struct IDirectDrawSurface3 IDirectDrawSurface3, *PIDirectDrawSurface3;

typedef struct IDirect3DTexture2 IDirect3DTexture2, *PIDirect3DTexture2;

typedef dword GraphicsTextureHandle;

typedef dword GraphicsResourceUsageSerial;

typedef struct DDPIXELFORMAT DDPIXELFORMAT, *PDDPIXELFORMAT;

typedef enum GraphicsTextureDownsampleShift {
    GRAPHICS_TEXTURE_DOWNSAMPLE_1X=0,
    GRAPHICS_TEXTURE_DOWNSAMPLE_2X=1,
    GRAPHICS_TEXTURE_DOWNSAMPLE_4X=2
} GraphicsTextureDownsampleShift;

typedef struct GrTexInfo GrTexInfo, *PGrTexInfo;

typedef enum GraphicsTextureResidentTmuIndex {
    GRAPHICS_TEXTURE_RESIDENT_TMU0=0,
    GRAPHICS_TEXTURE_NOT_RESIDENT=4294967295
} GraphicsTextureResidentTmuIndex;

typedef dword GraphicsTextureMemoryAddress;

typedef dword AssetDimension;

typedef int GraphicsPaletteIndex;

typedef int GraphicsPixelOrigin;

typedef dword AssetRecordByteCount;

typedef enum PckModelDefinitionIdCatalog {
    MDL_0100_UNTERBAU1=100,
    MDL_0101_UNTERBAU1=101,
    MDL_0102_UNTERBAU1=102,
    MDL_0103_UNTERBAU1=103,
    MDL_0111_UNTERBAU1=111,
    MDL_0112_UNTERBAU1=112,
    MDL_0113_UNTERBAU1=113,
    MDL_0120_UNTERBAU1=120,
    MDL_0121_UNTERBAU1=121,
    MDL_0122_UNTERBAU1=122,
    MDL_0130_UNTERBAU1=130,
    MDL_0131_UNTERBAU1=131,
    MDL_0132_UNTERBAU1=132,
    MDL_0133_UNTERBAU1=133,
    MDL_0141_UNTERBAU1=141,
    MDL_0142_UNTERBAU1=142,
    MDL_0143_UNTERBAU1=143,
    MDL_0150_UNTERBAU1=150,
    MDL_0151_UNTERBAU1=151,
    MDL_0152_UNTERBAU1=152,
    MDL_0153_UNTERBAU1=153,
    MDL_0200_AUFBAU1=200,
    MDL_0201_AUFBAU1=201,
    MDL_0202_AUFBAU1=202,
    MDL_0210_AUFBAU1=210,
    MDL_0211_AUFBAU1=211,
    MDL_0212_AUFBAU1=212,
    MDL_0213_AUFBAU1=213,
    MDL_0214_AUFBAU1=214,
    MDL_0215_AUFBAU1=215,
    MDL_0216_AUFBAU1=216,
    MDL_0217_AUFBAU1=217,
    MDL_0218_AUFBAU1=218,
    MDL_0219_AUFBAU1=219,
    MDL_0220_AUFBAU1=220,
    MDL_0221_AUFBAU1=221,
    MDL_0222_AUFBAU1=222,
    MDL_0223_AUFBAU1=223,
    MDL_0224_AUFBAU1=224,
    MDL_0225_AUFBAU1=225,
    MDL_0226_AUFBAU1=226,
    MDL_0227_AUFBAU1=227,
    MDL_0228_AUFBAU1=228,
    MDL_0229_AUFBAU1=229,
    MDL_0230_AUFBAU1=230,
    MDL_0231_AUFBAU1=231,
    MDL_0232_AUFBAU1=232,
    MDL_0233_AUFBAU1=233,
    MDL_0300_BUILDING1=300,
    MDL_0301_BUILDING1=301,
    MDL_0302_BUILDING1=302,
    MDL_0303_BUILDING1=303,
    MDL_0305_BUILDING1=305,
    MDL_0306_BUILDING1=306,
    MDL_0307_BUILDING1=307,
    MDL_0308_BUILDING1=308,
    MDL_0309_BUILDING1=309,
    MDL_0310_BUILDING1=310,
    MDL_0311_BUILDING1=311,
    MDL_0312_BUILDING1=312,
    MDL_0314_BUILDING1=314,
    MDL_0315_BUILDING1=315,
    MDL_0316_BUILDING1=316,
    MDL_0317_BUILDING1=317,
    MDL_0318_BUILDING1=318,
    MDL_0319_BUILDING1=319,
    MDL_0320_BUILDING1=320,
    MDL_0321_BUILDING1=321,
    MDL_0322_BUILDING1=322,
    MDL_0323_BUILDING1=323,
    MDL_0324_BUILDING1=324,
    MDL_0325_BUILDING1=325,
    MDL_0326_BUILDING1=326,
    MDL_0327_BUILDING1=327,
    MDL_0328_BUILDING1=328,
    MDL_0329_BUILDING1=329,
    MDL_0400_RUINEN=400,
    MDL_0401_RUINEN=401,
    MDL_0402_RUINEN=402,
    MDL_0403_RUINEN=403,
    MDL_0404_RUINEN=404,
    MDL_0405_RUINEN=405,
    MDL_0406_RUINEN=406,
    MDL_0407_RUINEN=407,
    MDL_0408_RUINEN=408,
    MDL_0409_RUINEN=409,
    MDL_0410_RUINEN=410,
    MDL_0411_RUINEN=411,
    MDL_0412_RUINEN=412,
    MDL_0413_RUINEN=413,
    MDL_0414_RUINEN=414,
    MDL_0415_RUINEN=415,
    MDL_0416_RUINEN=416,
    MDL_0417_RUINEN=417,
    MDL_0418_RUINEN=418,
    MDL_0419_RUINEN=419,
    MDL_0420_RUINEN=420,
    MDL_0421_RUINEN=421,
    MDL_0422_RUINEN=422,
    MDL_0423_RUINEN=423,
    MDL_0424_RUINEN=424,
    MDL_0425_RUINEN=425,
    MDL_0426_RUINEN=426,
    MDL_0427_RUINEN=427,
    MDL_0428_RUINEN=428,
    MDL_0429_RUINEN=429,
    MDL_0430_RUINEN=430,
    MDL_0431_RUINEN=431,
    MDL_0432_RUINEN=432,
    MDL_0433_RUINEN=433,
    MDL_0434_RUINEN=434,
    MDL_0435_RUINEN=435,
    MDL_0436_RUINEN=436,
    MDL_0437_RUINEN=437,
    MDL_0438_RUINEN=438,
    MDL_0439_RUINEN=439,
    MDL_0440_RUINEN=440,
    MDL_0500_LBAUM=500,
    MDL_0501_LBAUM=501,
    MDL_0502_LBAUM=502,
    MDL_0503_LBAUM=503,
    MDL_0504_LBAUM=504,
    MDL_0505_LBAUM=505,
    MDL_0506_LBAUM=506,
    MDL_0507_LBAUM=507,
    MDL_0508_LBAUM=508,
    MDL_0540_NBAUM=540,
    MDL_0541_NBAUM=541,
    MDL_0542_NBAUM=542,
    MDL_0543_NBAUM=543,
    MDL_0544_NBAUM=544,
    MDL_0545_NBAUM=545,
    MDL_0546_NBAUM=546,
    MDL_0547_NBAUM=547,
    MDL_0548_NBAUM=548,
    MDL_0549_NBAUM=549,
    MDL_0550_NBAUM=550,
    MDL_0551_NBAUM=551,
    MDL_0552_NBAUM=552,
    MDL_0553_NBAUM=553,
    MDL_0554_NBAUM=554,
    MDL_0555_NBAUM=555,
    MDL_0556_NBAUM=556,
    MDL_0557_NBAUM=557,
    MDL_0558_NBAUM=558,
    MDL_0559_NBAUM=559,
    MDL_0560_NBAUM=560,
    MDL_0561_NBAUM=561,
    MDL_0562_NBAUM=562,
    MDL_0563_NBAUM=563,
    MDL_0580_PALMEN=580,
    MDL_0581_PALMEN=581,
    MDL_0582_PALMEN=582,
    MDL_0583_PALMEN=583,
    MDL_0584_PALMEN=584,
    MDL_0585_PALMEN=585,
    MDL_0586_PALMEN=586,
    MDL_0587_PALMEN=587,
    MDL_0588_PALMEN=588,
    MDL_0620_STEIN=620,
    MDL_0621_STEIN=621,
    MDL_0622_STEIN=622,
    MDL_0623_STEIN=623,
    MDL_0624_STEIN=624,
    MDL_0625_STEIN=625,
    MDL_0626_STEIN=626,
    MDL_0627_STEIN=627,
    MDL_0628_STEIN=628,
    MDL_0629_STEIN=629,
    MDL_0630_STEIN=630,
    MDL_0631_STEIN=631,
    MDL_0632_STEIN=632,
    MDL_0633_STEIN=633,
    MDL_0634_STEIN=634,
    MDL_0635_STEIN=635,
    MDL_0636_STEIN=636,
    MDL_0637_STEIN=637,
    MDL_0638_STEIN=638,
    MDL_0639_STEIN=639,
    MDL_0640_STEIN=640,
    MDL_0641_STEIN=641,
    MDL_0642_STEIN=642,
    MDL_0647_STEIN=647,
    MDL_0648_STEIN=648,
    MDL_0649_STEIN=649,
    MDL_0650_STEIN=650,
    MDL_0651_STEIN=651,
    MDL_0652_STEIN=652,
    MDL_0653_STEIN=653,
    MDL_0654_STEIN=654,
    MDL_0655_STEIN=655,
    MDL_0656_STEIN=656,
    MDL_0657_STEIN=657,
    MDL_0658_STEIN=658,
    MDL_0660_BUSCH=660,
    MDL_0661_BUSCH=661,
    MDL_0662_BUSCH=662,
    MDL_0663_BUSCH=663,
    MDL_0664_BUSCH=664,
    MDL_0665_BUSCH=665,
    MDL_0710_FARNE=710,
    MDL_0711_FARNE=711,
    MDL_0712_FARNE=712,
    MDL_0713_FARNE=713,
    MDL_0750_KAKTUS=750,
    MDL_0751_KAKTUS=751,
    MDL_0752_KAKTUS=752,
    MDL_0800_ROHSTOFF=800,
    MDL_0801_ROHSTOFF=801,
    MDL_0802_ROHSTOFF=802,
    MDL_0803_ROHSTOFF=803,
    MDL_0804_ROHSTOFF=804,
    MDL_0805_ROHSTOFF=805,
    MDL_0806_ROHSTOFF=806,
    MDL_0807_ROHSTOFF=807,
    MDL_0808_ROHSTOFF=808,
    MDL_0809_ROHSTOFF=809,
    MDL_0810_ROHSTOFF=810,
    MDL_0811_ROHSTOFF=811,
    MDL_0812_ROHSTOFF=812,
    MDL_0850_ROHSTOFF=850,
    MDL_0851_ROHSTOFF=851,
    MDL_0852_ROHSTOFF=852,
    MDL_0853_ROHSTOFF=853,
    MDL_0854_ROHSTOFF=854,
    MDL_0855_ROHSTOFF=855,
    MDL_1100_UNTERBAU2=1100,
    MDL_1101_UNTERBAU2=1101,
    MDL_1102_UNTERBAU2=1102,
    MDL_1103_UNTERBAU2=1103,
    MDL_1111_UNTERBAU2=1111,
    MDL_1112_UNTERBAU2=1112,
    MDL_1113_UNTERBAU2=1113,
    MDL_1120_UNTERBAU2=1120,
    MDL_1121_UNTERBAU2=1121,
    MDL_1122_UNTERBAU2=1122,
    MDL_1130_UNTERBAU2=1130,
    MDL_1131_UNTERBAU2=1131,
    MDL_1132_UNTERBAU2=1132,
    MDL_1133_UNTERBAU2=1133,
    MDL_1141_UNTERBAU2=1141,
    MDL_1142_UNTERBAU2=1142,
    MDL_1143_UNTERBAU2=1143,
    MDL_1150_UNTERBAU2=1150,
    MDL_1151_UNTERBAU2=1151,
    MDL_1152_UNTERBAU2=1152,
    MDL_1153_UNTERBAU2=1153,
    MDL_1200_AUFBAU2=1200,
    MDL_1201_AUFBAU2=1201,
    MDL_1202_AUFBAU2=1202,
    MDL_1210_AUFBAU2=1210,
    MDL_1211_AUFBAU2=1211,
    MDL_1212_AUFBAU2=1212,
    MDL_1213_AUFBAU2=1213,
    MDL_1214_AUFBAU2=1214,
    MDL_1215_AUFBAU2=1215,
    MDL_1216_AUFBAU2=1216,
    MDL_1217_AUFBAU2=1217,
    MDL_1218_AUFBAU2=1218,
    MDL_1219_AUFBAU2=1219,
    MDL_1220_AUFBAU2=1220,
    MDL_1221_AUFBAU2=1221,
    MDL_1222_AUFBAU2=1222,
    MDL_1223_AUFBAU2=1223,
    MDL_1224_AUFBAU2=1224,
    MDL_1225_AUFBAU2=1225,
    MDL_1226_AUFBAU2=1226,
    MDL_1227_AUFBAU2=1227,
    MDL_1228_AUFBAU2=1228,
    MDL_1229_AUFBAU2=1229,
    MDL_1230_AUFBAU2=1230,
    MDL_1231_AUFBAU2=1231,
    MDL_1232_AUFBAU2=1232,
    MDL_1233_AUFBAU2=1233,
    MDL_1300_BUILDING2=1300,
    MDL_1301_BUILDING2=1301,
    MDL_1302_BUILDING2=1302,
    MDL_1303_BUILDING2=1303,
    MDL_1305_BUILDING2=1305,
    MDL_1306_BUILDING2=1306,
    MDL_1307_BUILDING2=1307,
    MDL_1308_BUILDING2=1308,
    MDL_1309_BUILDING2=1309,
    MDL_1310_BUILDING2=1310,
    MDL_1311_BUILDING2=1311,
    MDL_1312_BUILDING2=1312,
    MDL_1314_BUILDING2=1314,
    MDL_1315_BUILDING2=1315,
    MDL_1316_BUILDING2=1316,
    MDL_1317_BUILDING2=1317,
    MDL_1318_BUILDING2=1318,
    MDL_1319_BUILDING2=1319,
    MDL_1320_BUILDING2=1320,
    MDL_1321_BUILDING2=1321,
    MDL_1322_BUILDING2=1322,
    MDL_1323_BUILDING2=1323,
    MDL_1324_BUILDING2=1324,
    MDL_1325_BUILDING2=1325,
    MDL_1326_BUILDING2=1326,
    MDL_1327_BUILDING2=1327,
    MDL_1328_BUILDING2=1328,
    MDL_1329_BUILDING2=1329,
    MDL_2100_UNTERBAU3=2100,
    MDL_2101_UNTERBAU3=2101,
    MDL_2102_UNTERBAU3=2102,
    MDL_2103_UNTERBAU3=2103,
    MDL_2111_UNTERBAU3=2111,
    MDL_2112_UNTERBAU3=2112,
    MDL_2113_UNTERBAU3=2113,
    MDL_2120_UNTERBAU3=2120,
    MDL_2121_UNTERBAU3=2121,
    MDL_2122_UNTERBAU3=2122,
    MDL_2130_UNTERBAU3=2130,
    MDL_2131_UNTERBAU3=2131,
    MDL_2132_UNTERBAU3=2132,
    MDL_2133_UNTERBAU3=2133,
    MDL_2141_UNTERBAU3=2141,
    MDL_2142_UNTERBAU3=2142,
    MDL_2143_UNTERBAU3=2143,
    MDL_2150_UNTERBAU3=2150,
    MDL_2151_UNTERBAU3=2151,
    MDL_2152_UNTERBAU3=2152,
    MDL_2153_UNTERBAU3=2153,
    MDL_2200_AUFBAU3=2200,
    MDL_2201_AUFBAU3=2201,
    MDL_2202_AUFBAU3=2202,
    MDL_2210_AUFBAU3=2210,
    MDL_2211_AUFBAU3=2211,
    MDL_2212_AUFBAU3=2212,
    MDL_2213_AUFBAU3=2213,
    MDL_2214_AUFBAU3=2214,
    MDL_2215_AUFBAU3=2215,
    MDL_2216_AUFBAU3=2216,
    MDL_2217_AUFBAU3=2217,
    MDL_2218_AUFBAU3=2218,
    MDL_2219_AUFBAU3=2219,
    MDL_2220_AUFBAU3=2220,
    MDL_2221_AUFBAU3=2221,
    MDL_2222_AUFBAU3=2222,
    MDL_2223_AUFBAU3=2223,
    MDL_2224_AUFBAU3=2224,
    MDL_2225_AUFBAU3=2225,
    MDL_2226_AUFBAU3=2226,
    MDL_2227_AUFBAU3=2227,
    MDL_2228_AUFBAU3=2228,
    MDL_2229_AUFBAU3=2229,
    MDL_2230_AUFBAU3=2230,
    MDL_2231_AUFBAU3=2231,
    MDL_2232_AUFBAU3=2232,
    MDL_2233_AUFBAU3=2233,
    MDL_2300_BUILDING3=2300,
    MDL_2301_BUILDING3=2301,
    MDL_2302_BUILDING3=2302,
    MDL_2303_BUILDING3=2303,
    MDL_2305_BUILDING3=2305,
    MDL_2306_BUILDING3=2306,
    MDL_2307_BUILDING3=2307,
    MDL_2308_BUILDING3=2308,
    MDL_2309_BUILDING3=2309,
    MDL_2310_BUILDING3=2310,
    MDL_2311_BUILDING3=2311,
    MDL_2312_BUILDING3=2312,
    MDL_2314_BUILDING3=2314,
    MDL_2315_BUILDING3=2315,
    MDL_2316_BUILDING3=2316,
    MDL_2317_BUILDING3=2317,
    MDL_2318_BUILDING3=2318,
    MDL_2319_BUILDING3=2319,
    MDL_2320_BUILDING3=2320,
    MDL_2321_BUILDING3=2321,
    MDL_2322_BUILDING3=2322,
    MDL_2323_BUILDING3=2323,
    MDL_2324_BUILDING3=2324,
    MDL_2325_BUILDING3=2325,
    MDL_2326_BUILDING3=2326,
    MDL_2327_BUILDING3=2327,
    MDL_2328_BUILDING3=2328,
    MDL_2329_BUILDING3=2329
} PckModelDefinitionIdCatalog;

typedef dword SprAttachmentPackedKey;

typedef struct AssetBuildTimestampSet AssetBuildTimestampSet, *PAssetBuildTimestampSet;

typedef struct AssetProducerSourceNames AssetProducerSourceNames, *PAssetProducerSourceNames;

typedef struct IDirectDrawSurface_Vtbl IDirectDrawSurface_Vtbl, *PIDirectDrawSurface_Vtbl;

typedef sdword TH_LEGACY_HRESULT;

typedef struct TH_LEGACY_GUID TH_LEGACY_GUID, *PTH_LEGACY_GUID;

typedef void *TH_LEGACY_LPVOID;

typedef dword TH_LEGACY_ULONG;

typedef struct TH_LEGACY_RECT TH_LEGACY_RECT, *PTH_LEGACY_RECT;

typedef dword TH_LEGACY_DWORD;

typedef struct DDSCAPS DDSCAPS, *PDDSCAPS;

typedef struct DDCOLORKEY DDCOLORKEY, *PDDCOLORKEY;

typedef void *TH_LEGACY_HANDLE;

typedef sdword TH_LEGACY_LONG;

typedef struct DDSURFACEDESC_DX6 DDSURFACEDESC_DX6, *PDDSURFACEDESC_DX6;

typedef struct IDirectDraw IDirectDraw, *PIDirectDraw;

typedef struct IDirectDrawSurface3_Vtbl IDirectDrawSurface3_Vtbl, *PIDirectDrawSurface3_Vtbl;

typedef struct IDirect3DTexture2_Vtbl IDirect3DTexture2_Vtbl, *PIDirect3DTexture2_Vtbl;

typedef struct IDirect3DDevice2 IDirect3DDevice2, *PIDirect3DDevice2;

typedef int GrLOD_t;

typedef int GrAspectRatio_t;

typedef int GrTextureFormat_t;

typedef dword AssetPackedDate;

typedef dword AssetPackedTime;

typedef word TH_LEGACY_WORD;

typedef byte TH_LEGACY_BYTE;

typedef struct IDirectDraw_Vtbl IDirectDraw_Vtbl, *PIDirectDraw_Vtbl;

typedef sdword TH_LEGACY_BOOL;

typedef void *TH_LEGACY_HWND;

typedef struct IDirect3DDevice2_Vtbl IDirect3DDevice2_Vtbl, *PIDirect3DDevice2_Vtbl;

typedef struct IDirect3DViewport2 IDirect3DViewport2, *PIDirect3DViewport2;

typedef struct IDirect3D2 IDirect3D2, *PIDirect3D2;

typedef enum D3DRENDERSTATETYPE_DX6 {
    D3DRENDERSTATE_TEXTUREHANDLE=1,
    D3DRENDERSTATE_ANTIALIAS=2,
    D3DRENDERSTATE_TEXTUREADDRESS=3,
    D3DRENDERSTATE_TEXTUREPERSPECTIVE=4,
    D3DRENDERSTATE_WRAPU=5,
    D3DRENDERSTATE_WRAPV=6,
    D3DRENDERSTATE_ZENABLE=7,
    D3DRENDERSTATE_FILLMODE=8,
    D3DRENDERSTATE_SHADEMODE=9,
    D3DRENDERSTATE_LINEPATTERN=10,
    D3DRENDERSTATE_MONOENABLE=11,
    D3DRENDERSTATE_ROP2=12,
    D3DRENDERSTATE_PLANEMASK=13,
    D3DRENDERSTATE_ZWRITEENABLE=14,
    D3DRENDERSTATE_ALPHATESTENABLE=15,
    D3DRENDERSTATE_LASTPIXEL=16,
    D3DRENDERSTATE_TEXTUREMAG=17,
    D3DRENDERSTATE_TEXTUREMIN=18,
    D3DRENDERSTATE_SRCBLEND=19,
    D3DRENDERSTATE_DESTBLEND=20,
    D3DRENDERSTATE_TEXTUREMAPBLEND=21,
    D3DRENDERSTATE_CULLMODE=22,
    D3DRENDERSTATE_ZFUNC=23,
    D3DRENDERSTATE_ALPHAREF=24,
    D3DRENDERSTATE_ALPHAFUNC=25,
    D3DRENDERSTATE_DITHERENABLE=26,
    D3DRENDERSTATE_ALPHABLENDENABLE=27,
    D3DRENDERSTATE_FOGENABLE=28,
    D3DRENDERSTATE_SPECULARENABLE=29,
    D3DRENDERSTATE_ZVISIBLE=30,
    D3DRENDERSTATE_SUBPIXEL=31,
    D3DRENDERSTATE_SUBPIXELX=32,
    D3DRENDERSTATE_STIPPLEDALPHA=33,
    D3DRENDERSTATE_FOGCOLOR=34,
    D3DRENDERSTATE_FOGTABLEMODE=35,
    D3DRENDERSTATE_FOGSTART=36,
    D3DRENDERSTATE_FOGEND=37,
    D3DRENDERSTATE_FOGDENSITY=38,
    D3DRENDERSTATE_STIPPLEENABLE=39,
    D3DRENDERSTATE_EDGEANTIALIAS=40,
    D3DRENDERSTATE_COLORKEYENABLE=41,
    D3DRENDERSTATE_BORDERCOLOR=43,
    D3DRENDERSTATE_TEXTUREADDRESSU=44,
    D3DRENDERSTATE_TEXTUREADDRESSV=45,
    D3DRENDERSTATE_MIPMAPLODBIAS=46,
    D3DRENDERSTATE_ZBIAS=47,
    D3DRENDERSTATE_RANGEFOGENABLE=48,
    D3DRENDERSTATE_ANISOTROPY=49,
    D3DRENDERSTATE_FLUSHBATCH=50,
    D3DRENDERSTATE_TRANSLUCENTSORTINDEPENDENT=51
} D3DRENDERSTATETYPE_DX6;

typedef struct D3DTLVERTEX_DX6 D3DTLVERTEX_DX6, *PD3DTLVERTEX_DX6;

typedef struct IDirect3DViewport2_Vtbl IDirect3DViewport2_Vtbl, *PIDirect3DViewport2_Vtbl;

typedef struct D3DVIEWPORT2 D3DVIEWPORT2, *PD3DVIEWPORT2;

typedef struct IDirect3D2_Vtbl IDirect3D2_Vtbl, *PIDirect3D2_Vtbl;

typedef struct D3DDEVICEDESC_DX6 D3DDEVICEDESC_DX6, *PD3DDEVICEDESC_DX6;

typedef struct GraphicsAdapterRecord GraphicsAdapterRecord, *PGraphicsAdapterRecord;

typedef struct D3DTRANSFORMCAPS_DX6 D3DTRANSFORMCAPS_DX6, *PD3DTRANSFORMCAPS_DX6;

typedef struct D3DLIGHTINGCAPS_DX6 D3DLIGHTINGCAPS_DX6, *PD3DLIGHTINGCAPS_DX6;

typedef struct D3DPRIMCAPS_DX6 D3DPRIMCAPS_DX6, *PD3DPRIMCAPS_DX6;

struct ModelResourceHitTestAndRenderView210 {
    byte reserved00_AF[176]; 
    ModelMeshGroupCount meshGroupCount; 
    byte reservedB4_BF[12]; 
    Q12 localBoundsX0Q12; 
    Q12 localBoundsX1Q12; 
    Q12 localBoundsY0Q12; 
    Q12 localBoundsY1Q12; 
    Q12 localBoundsZ0Q12; 
    Q12 localBoundsZ1Q12; 
    Q12 boundingRadiusQ12; 
    Q12 placementHeightOffsetQ12; 
    Q12 lightingScaleQ12; 
    ModelPackedLookupTableRelativeOffset packedLookupTableRelativeOffset; 
    ModelPackedLookupTableEntryCount packedLookupTableEntryCount; 
    byte reservedEC_1FF[276]; 
    ModelMeshGroupRelativeOffset firstMeshGroupRelativeOffset; 
    ModelPackedGeometryRecordCount packedGeometryRecordCount; 
    byte reserved208_20B[4]; 
    enum ModelResourceHitTestFlags hitTestFlags20C; 
};

union GameEntityDamageCounterOrTerminalReference4 {
    int countdownOrState; 
    struct GameEntityRuntime *terminalEntity; 
    dword raw; 
};

struct GrTexInfo {
    GrLOD_t smallLodLog2; 
    GrLOD_t largeLodLog2; 
    GrAspectRatio_t aspectRatioLog2; 
    GrTextureFormat_t format; 
    void *data; 
};

struct GraphicsFixedVec3 {
    GraphicsWorldCoordinateQ12 x; 
    GraphicsWorldCoordinateQ12 y; 
    GraphicsWorldCoordinateQ12 z; 
};

struct GraphicsFixedMatrix3x4 {
    sdword basisRow0[3]; 
    sdword basisRow1[3]; 
    sdword basisRow2[3]; 
    struct GraphicsFixedVec3 translation; 
};

struct IDirect3D2 {
    struct IDirect3D2_Vtbl *lpVtbl;
};

struct IDirectDrawSurface3_Vtbl {
    TH_LEGACY_HRESULT (*QueryInterface)(struct IDirectDrawSurface3 *, struct TH_LEGACY_GUID *, TH_LEGACY_LPVOID *);
    TH_LEGACY_ULONG (*AddRef)(struct IDirectDrawSurface3 *);
    TH_LEGACY_ULONG (*Release)(struct IDirectDrawSurface3 *);
    TH_LEGACY_HRESULT (*AddAttachedSurface)(struct IDirectDrawSurface3 *, struct IDirectDrawSurface3 *);
    TH_LEGACY_HRESULT (*AddOverlayDirtyRect)(struct IDirectDrawSurface3 *, struct TH_LEGACY_RECT *);
    TH_LEGACY_HRESULT (*Blt)(struct IDirectDrawSurface3 *, struct TH_LEGACY_RECT *, struct IDirectDrawSurface3 *, struct TH_LEGACY_RECT *, TH_LEGACY_DWORD, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*BltBatch)(struct IDirectDrawSurface3 *, TH_LEGACY_LPVOID, TH_LEGACY_DWORD, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*BltFast)(struct IDirectDrawSurface3 *, TH_LEGACY_DWORD, TH_LEGACY_DWORD, struct IDirectDrawSurface3 *, struct TH_LEGACY_RECT *, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*DeleteAttachedSurface)(struct IDirectDrawSurface3 *, TH_LEGACY_DWORD, struct IDirectDrawSurface3 *);
    TH_LEGACY_HRESULT (*EnumAttachedSurfaces)(struct IDirectDrawSurface3 *, TH_LEGACY_LPVOID, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*EnumOverlayZOrders)(struct IDirectDrawSurface3 *, TH_LEGACY_DWORD, TH_LEGACY_LPVOID, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*Flip)(struct IDirectDrawSurface3 *, struct IDirectDrawSurface3 *, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*GetAttachedSurface)(struct IDirectDrawSurface3 *, struct DDSCAPS *, struct IDirectDrawSurface3 **);
    TH_LEGACY_HRESULT (*GetBltStatus)(struct IDirectDrawSurface3 *, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*GetCaps)(struct IDirectDrawSurface3 *, struct DDSCAPS *);
    TH_LEGACY_HRESULT (*GetClipper)(struct IDirectDrawSurface3 *, TH_LEGACY_LPVOID *);
    TH_LEGACY_HRESULT (*GetColorKey)(struct IDirectDrawSurface3 *, TH_LEGACY_DWORD, struct DDCOLORKEY *);
    TH_LEGACY_HRESULT (*GetDC)(struct IDirectDrawSurface3 *, TH_LEGACY_HANDLE *);
    TH_LEGACY_HRESULT (*GetFlipStatus)(struct IDirectDrawSurface3 *, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*GetOverlayPosition)(struct IDirectDrawSurface3 *, TH_LEGACY_LONG *, TH_LEGACY_LONG *);
    TH_LEGACY_HRESULT (*GetPalette)(struct IDirectDrawSurface3 *, TH_LEGACY_LPVOID *);
    TH_LEGACY_HRESULT (*GetPixelFormat)(struct IDirectDrawSurface3 *, struct DDPIXELFORMAT *);
    TH_LEGACY_HRESULT (*GetSurfaceDesc)(struct IDirectDrawSurface3 *, struct DDSURFACEDESC_DX6 *);
    TH_LEGACY_HRESULT (*Initialize)(struct IDirectDrawSurface3 *, struct IDirectDraw *, struct DDSURFACEDESC_DX6 *);
    TH_LEGACY_HRESULT (*IsLost)(struct IDirectDrawSurface3 *);
    TH_LEGACY_HRESULT (*Lock)(struct IDirectDrawSurface3 *, struct TH_LEGACY_RECT *, struct DDSURFACEDESC_DX6 *, TH_LEGACY_DWORD, TH_LEGACY_HANDLE);
    TH_LEGACY_HRESULT (*ReleaseDC)(struct IDirectDrawSurface3 *, TH_LEGACY_HANDLE);
    TH_LEGACY_HRESULT (*Restore)(struct IDirectDrawSurface3 *);
    TH_LEGACY_HRESULT (*SetClipper)(struct IDirectDrawSurface3 *, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*SetColorKey)(struct IDirectDrawSurface3 *, TH_LEGACY_DWORD, struct DDCOLORKEY *);
    TH_LEGACY_HRESULT (*SetOverlayPosition)(struct IDirectDrawSurface3 *, TH_LEGACY_LONG, TH_LEGACY_LONG);
    TH_LEGACY_HRESULT (*SetPalette)(struct IDirectDrawSurface3 *, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*Unlock)(struct IDirectDrawSurface3 *, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*UpdateOverlay)(struct IDirectDrawSurface3 *, struct TH_LEGACY_RECT *, struct IDirectDrawSurface3 *, struct TH_LEGACY_RECT *, TH_LEGACY_DWORD, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*UpdateOverlayDisplay)(struct IDirectDrawSurface3 *, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*UpdateOverlayZOrder)(struct IDirectDrawSurface3 *, TH_LEGACY_DWORD, struct IDirectDrawSurface3 *);
    TH_LEGACY_HRESULT (*GetDDInterface)(struct IDirectDrawSurface3 *, TH_LEGACY_LPVOID *);
    TH_LEGACY_HRESULT (*PageLock)(struct IDirectDrawSurface3 *, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*PageUnlock)(struct IDirectDrawSurface3 *, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*SetSurfaceDesc)(struct IDirectDrawSurface3 *, struct DDSURFACEDESC_DX6 *, TH_LEGACY_DWORD);
};

struct IDirect3DTexture2_Vtbl {
    sdword (*QueryInterface)(struct IDirect3DTexture2 *, struct TH_LEGACY_GUID *, void **);
    dword (*AddRef)(struct IDirect3DTexture2 *);
    dword (*Release)(struct IDirect3DTexture2 *);
    sdword (*GetHandle)(struct IDirect3DTexture2 *, struct IDirect3DDevice2 *, dword *);
    sdword (*PaletteChanged)(struct IDirect3DTexture2 *, dword, dword);
    sdword (*Load)(struct IDirect3DTexture2 *, struct IDirect3DTexture2 *);
};

struct ModelDefinitionRecordPrefix {
    AssetRecordByteCount byteSize; 
    dword flags; 
    enum PckModelDefinitionIdCatalog definitionId; 
};

struct GameEntityTechnologyPayload {
    TechnologyResearchDurationQ5 appliedResearchDurationQ5; 
    dword entityValue24; 
    TechnologyEnergyCostQ4 energyCostQ4; 
[RESOURCE_FUEL_ENERGY_CAPACITY_SEPARATION_CLOSURE] Energy requirement Q4 for active research. Starting research adds this demand to the research ArmyRuntime load; completion removes it. It is not Tritium stock.
    TechnologyXeniteCostQ4 xeniteCostQ4; 
[RESOURCE_FUEL_ENERGY_CAPACITY_SEPARATION_CLOSURE] Xenite requirement Q4. Starting research pays this once from faction Xenite stock before research enters its active Energy-demand phase.
    byte reserved14_FB[236]; 
};

struct TH_LEGACY_GUID {
    TH_LEGACY_DWORD Data1;
    TH_LEGACY_WORD Data2;
    TH_LEGACY_WORD Data3;
    TH_LEGACY_BYTE Data4[8];
};

union EffectRuntimeOwnerReference4 {
    struct ModelRuntimeNode *modelNode; 
    struct ArmyRuntimeSlot *armyRuntime; 
    dword serializedOffset; 
};

struct EffectRuntimeOwnerAndDefinitionState8 {
    union EffectRuntimeOwnerReference4 owner; 
    dword runtimeValue24; 
};

struct EffectRuntimeLifecycleState10 {
    dword runtimeState14; 
    EffectAnimationFrameAccumulatorQ4 animationFrameAccumulatorQ4; 
    struct EffectRuntimeOwnerAndDefinitionState8 ownerAndDefinition; 
};

struct ArmyRuntimeLinkedChildSpawnParameters0C {
    dword parameter0; 
    dword parameter1; 
    dword parameter2; 
};

struct GameEntityOwnershipState10 {
    void *definitionOrClassRecord; 
    struct ModelRuntimeNode *modelNode; 
    void *runtimeLink; 
    FactionRuntimeIndex ownerIndex; 
};

struct D3DLIGHTINGCAPS_DX6 {
    dword dwSize;
    dword dwCaps;
    dword dwLightingModel;
    dword dwNumLights;
};

struct D3DPRIMCAPS_DX6 {
    dword dwSize;
    dword dwMiscCaps;
    dword dwRasterCaps;
    dword dwZCmpCaps;
    dword dwSrcBlendCaps;
    dword dwDestBlendCaps;
    dword dwAlphaCmpCaps;
    dword dwShadeCaps;
    dword dwTextureCaps;
    dword dwTextureFilterCaps;
    dword dwTextureBlendCaps;
    dword dwTextureAddressCaps;
    dword dwStippleWidth;
    dword dwStippleHeight;
};

struct D3DTRANSFORMCAPS_DX6 {
    dword dwSize;
    dword dwCaps;
};

struct D3DDEVICEDESC_DX6 {
    dword dwSize;
    dword dwFlags;
    dword dcmColorModel;
    dword dwDevCaps;
    struct D3DTRANSFORMCAPS_DX6 dtcTransformCaps;
    sdword bClipping;
    struct D3DLIGHTINGCAPS_DX6 dlcLightingCaps;
    struct D3DPRIMCAPS_DX6 dpcLineCaps;
    struct D3DPRIMCAPS_DX6 dpcTriCaps;
    dword dwDeviceRenderBitDepth;
    dword dwDeviceZBufferBitDepth;
    dword dwMaxBufferSize;
    dword dwMaxVertexCount;
    dword dwMinTextureWidth;
    dword dwMinTextureHeight;
    dword dwMaxTextureWidth;
    dword dwMaxTextureHeight;
    dword dwMinStippleWidth;
    dword dwMaxStippleWidth;
    dword dwMinStippleHeight;
    dword dwMaxStippleHeight;
};

struct ShotDefinition {
    enum ShotTrajectoryMode trajectoryMode; 
    dword reservedDword04; 
    enum PckShotDefinitionIdCatalog definitionId; 
    Q12 launchSpeedQ12; 
    struct EffectDefinition *primaryEffectDefinition; 
    struct EffectDefinition *terrainImpactEffectDefinitions31[31]; 
    struct EffectDefinition *targetClassImpactEffectDefinitions8[8]; 
    Q12 targetClassImpactDamageQ12[8]; 
    ShotProjectileLifetimeTicks projectileLifetimeTicks; 
    struct EffectDefinition *launchEffectDefinition; 
    ShotModelSpinStepTurn16 modelSpinStepTurn16; 
    Q12 ballisticDivisorQ12; 
    ShotTerrainImpactEffectOwnerSlot terrainImpactEffectOwnerSlots31[31]; 
    ShotFrameAdvanceThresholdQ4 animationFrameAdvanceThresholdQ4; 
    dword reservedDwords160_1DB[31]; 
    ShotAnimationFrameCount animationFrameCount; 
    TerrainMaterialIndex terrainMaterialIndices31[31]; 
    PackedArgb32 shadingColorArgb; 
    GraphicsTransitionTickCount shadingTransitionDurationTicks; 
    GraphicsTransitionTickCount shadingReleaseTransitionDurationTicks; 
    struct EffectDefinition *secondaryEffectDefinition; 
    ShotSecondaryEffectIntervalTicks secondaryEffectIntervalTicks; 
    ShotTrajectoryRampDurationTicks trajectoryRampDurationTicks; 
    ShotFixedRangeTransitionAgeTicks fixedRangeTransitionAgeThresholdTicks; 
    AngleTurn16Stored32 elevationOffsetAngle16; 
    Q12 mode2SelectionRangeQ12; 
    TerrainGridMaskIndex terrainGridMaskIndex; 
    PackedArgb32 stateTintArgb; 
    SpatialSoundGainQ15 positionedSoundGainQ15; 
    SpatialSoundMaximumDistanceQ12 positionedSoundMaximumDistanceQ12; 
    EffectLeadDisableFlag mode3LeadDisabled; 
    void *ownedNestedResource; 
    OwnedNestedResourceFlag ownedNestedResourcePresent; 
    word resourcePathUtf16[34]; 
};

struct WorldRuntimeSelectionState {
    PlayerRuntimeId activePlayerRuntimeId; 
    byte reserved04_1F[28]; 
    struct GameEntityRuntime *selectedEntity; 
    void (*dispatchCommandCallback)(dword, dword, struct WorldRuntimeContext *);
    dword (*resolveContextActionPrimaryCallback)(dword, dword, dword, dword, struct WorldRuntimeNode *, struct WorldRuntimeContext *);
    dword (*resolveContextActionSecondaryCallback)(dword, dword, dword, dword, struct WorldRuntimeNode *, struct WorldRuntimeContext *);
    void (*beginPointerCaptureCallback)(dword, dword, dword, dword, struct WorldRuntimeNode *, struct WorldRuntimeContext *);
    void (*updateDragSelectionCallback)(dword, dword, dword, dword, struct WorldRuntimeNode *, struct WorldRuntimeContext *);
    void (*commitPointerActionCallback)(dword, dword, dword, dword, struct WorldRuntimeNode *, struct WorldRuntimeContext *);
    void (*dispatchWorldContextActionCallback)(struct WorldRuntimeContext *);
    dword reservedCallbackState40; 
};

union EffectModelNodeReferenceOrSavedOffset4 {
    struct ModelRuntimeNode *modelNode; 
    dword savedIdOrOffset; 
    dword raw; 
};

struct WorldMotionState {
    Q12 positionXQ12; 
    Q12 positionYQ12; 
    Q12 positionZQ12; 
    UQ12 positionMagnitudeQ12; 
    AngleTurn32 headingAngle; 
    AngleTurn32 pitchAngle; 
    dword motionValue78; 
    UQ12 committedDistanceQ12; 
    Q12 targetPositionXQ12; 
    Q12 targetPositionYQ12; 
    Q12 targetPositionZQ12; 
    UQ12 targetDistanceQ12; 
    AngleTurn32 minimumPitchAngle; 
    AngleTurn32 maximumPitchAngle; 
};

struct WorldMotionSnapshot {
    Q12 positionXQ12; 
    Q12 positionYQ12; 
    Q12 positionZQ12; 
    UQ12 magnitudeQ12; 
    AngleTurn32 headingAngle; 
    AngleTurn32 pitchAngle; 
    UQ12 distanceQ12; 
};

struct WorldLightingState {
    PackedArgb32 baseColorArgb; 
    PackedArgb32 rampColorArgb; 
    PackedArgb32 color128Argb; 
    PackedArgb32 color12CArgb; 
    PackedArgb32 color130Argb; 
    PackedArgb32 color134Argb; 
    PackedArgb32 color138Argb; 
    PackedArgb32 color13CArgb; 
};

struct WorldRuntimeInteractionState {
    byte reserved00_47[72]; 
    WorldInteractionFlags interactionFlags48; 
};

struct WorldFieldRegionState {
    void (*clearTransientStateCallback)(struct WorldRuntimeContext *); 
    dword reservedCallbackState04; 
    WorldFieldDimension regionWidth; 
    WorldFieldDimension regionHeight; 
};

struct WorldRuntimeContext {
    struct WorldRuntimeInteractionState interaction; 
    WorldRuntimeFlags runtimeFlags; 
    FactionRuntimeIndex activeFactionRuntimeIndex; 
    struct FieldGridAsset *fieldGrid; 
    struct WorldObjectRecord *objectArray; 
    RuntimeToken pendingToken; 
    struct WorldMotionState motion; 
    dword reserved98; 
    WorldSurfaceSelectionFlags surfaceSelectionFlags; 
    byte reservedA0_AB[12]; 
    WorldObjectRecordCount objectCount; 
    struct WorldFieldRegionState fieldRegion; 
    dword *dwordArray; 
    WorldWorkspaceElementCount dwordArrayCount; 
    dword runtimeStateC8; 
    WorldRuntimeControlFlags runtimeControlFlags; 
    dword *tickSpinLock; 
    void (*simulationAndNetworkTickCallback)(void); 
    struct WorldRuntimeNode *ownerListHead; 
    struct WorldRuntimeSelectionState selection; 
    struct WorldLightingState lighting; 
    struct WorldMotionSnapshot snapshot; 
};

struct IDirect3DViewport2_Vtbl {
    TH_LEGACY_HRESULT (*QueryInterface)(struct IDirect3DViewport2 *, struct TH_LEGACY_GUID *, TH_LEGACY_LPVOID *);
    TH_LEGACY_ULONG (*AddRef)(struct IDirect3DViewport2 *);
    TH_LEGACY_ULONG (*Release)(struct IDirect3DViewport2 *);
    TH_LEGACY_HRESULT (*Initialize)(struct IDirect3DViewport2 *, struct IDirect3D2 *);
    TH_LEGACY_HRESULT (*GetViewport)(struct IDirect3DViewport2 *, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*SetViewport)(struct IDirect3DViewport2 *, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*TransformVertices)(struct IDirect3DViewport2 *, TH_LEGACY_DWORD, TH_LEGACY_LPVOID, TH_LEGACY_DWORD, TH_LEGACY_DWORD *);
    TH_LEGACY_HRESULT (*LightElements)(struct IDirect3DViewport2 *, TH_LEGACY_DWORD, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*SetBackground)(struct IDirect3DViewport2 *, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*GetBackground)(struct IDirect3DViewport2 *, TH_LEGACY_LPVOID *, TH_LEGACY_BOOL *);
    TH_LEGACY_HRESULT (*SetBackgroundDepth)(struct IDirect3DViewport2 *, struct IDirectDrawSurface *);
    TH_LEGACY_HRESULT (*GetBackgroundDepth)(struct IDirect3DViewport2 *, struct IDirectDrawSurface **, TH_LEGACY_BOOL *);
    TH_LEGACY_HRESULT (*Clear)(struct IDirect3DViewport2 *, TH_LEGACY_DWORD, TH_LEGACY_LPVOID, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*AddLight)(struct IDirect3DViewport2 *, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*DeleteLight)(struct IDirect3DViewport2 *, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*NextLight)(struct IDirect3DViewport2 *, TH_LEGACY_LPVOID, TH_LEGACY_LPVOID *, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*GetViewport2)(struct IDirect3DViewport2 *, struct D3DVIEWPORT2 *);
    TH_LEGACY_HRESULT (*SetViewport2)(struct IDirect3DViewport2 *, struct D3DVIEWPORT2 *);
};

union ModelRuntimeArmyLinkOrState4 {
    struct ArmyRuntimeSlot *armyRuntime; 
    dword classState; 
    dword serializedOffset; 
};

union ArmyRuntimeCoordinateCommandOrHistoryValue4 {
    int signedValue; 
    Q12 coordinateOrTargetQ12; 
    AngleTurn32 headingOrTurnValue; 
    dword raw; 
};

struct IDirect3DDevice2 {
    struct IDirect3DDevice2_Vtbl *lpVtbl;
};

struct GraphicsAdapterRecord {
    struct TH_LEGACY_GUID adapterGuid; 
    struct TH_LEGACY_GUID deviceGuid; 
    word driverDescriptionUtf16[21]; 
    word deviceNameUtf16[21]; 
    dword reserved74; 
    struct D3DDEVICEDESC_DX6 *hardwareDesc; 
    struct D3DDEVICEDESC_DX6 *softwareDesc; 
};

union ResourceRegistrationRuntimePayloadReference4 {
    struct ArmyRuntimeSlot *armyRuntime;
    struct EffectRuntimeSlot *effectRuntime;
    struct ShotRuntimeSlot *shotRuntime;
    dword savedOffset;
    dword raw;
};

struct DDCOLORKEY {
    TH_LEGACY_DWORD dwColorSpaceLowValue;
    TH_LEGACY_DWORD dwColorSpaceHighValue;
};

struct ModelAttachmentTransformRecord {
    SprAttachmentPackedKey packedKindAndSelector; 
    Q12 localTranslationXQ12; 
    Q12 localTranslationYQ12; 
    Q12 localTranslationZQ12; 
};

struct WorldObjectRecordCommon {
    byte reserved00_07[8]; 
    struct WorldRuntimeContext *ownerWorld; 
    byte reserved0C_4B[64]; 
    WorldObjectAllocationFlags allocationFlags; 
};

struct WorldObjectRecord {
    struct WorldObjectRecordCommon common; 
    byte classPayload[176]; 
};

struct GameEntityImpactOwnerLinksPayloadFC {
    byte reserved00_3B[60]; 
    struct ArmyRuntimeSlot *primaryImpactArmyRuntime; 
    byte reserved40_5B[28]; 
    struct ArmyRuntimeSlot *secondaryImpactArmyRuntime; 
    byte reserved60_FB[156]; 
};

union GameEntityRuntimeClassPayload {
    byte opaque[252]; 
    struct GameEntityTechnologyPayload technology; 
    struct GameEntityImpactOwnerLinksPayloadFC impactOwnerLinks; 
};

struct GameEntityImpactReactionBytes8 {
    byte state08; 
    byte reactionCode09; 
    byte state0A; 
    byte state0B; 
    byte reserved0C_0F[4]; 
};

struct GameEntityPathingReferenceState8 {
    struct GameEntityRuntime *overlappingEntity; 
    void *secondaryPathingReference; 
};

struct GameEntityPathingAndImpactState10 {
    struct GameEntityPathingReferenceState8 pathingReferences; 
    struct GameEntityImpactReactionBytes8 impactReaction; 
};

struct GameEntityDamageState2C {
    dword reserved00; 
    union GameEntityDamageCounterOrTerminalReference4 counterOrTerminalReference; 
    int remainingIntegrity; 
    byte reserved0C_2B[32]; 
};

struct GameEntityCommandTargetState {
    struct GameEntityRuntime *targetEntity; 
    Q12 targetWorldXQ12; 
    Q12 targetWorldYQ12; 
    Q12 targetWorldZQ12; 
    GameEntityCommandTargetFlags targetFlags; 
    ArmyCommandGeneration commandGeneration; 
};

struct GameEntityRuntimeCommon {
    struct GameEntityOwnershipState10 ownership; 
    byte reserved10_17[8]; 
    GameEntityCommandFlags commandFlags; 
    struct GameEntityCommandTargetState commandTarget; 
    struct GameEntityDamageState2C damageState; 
    Q12 selectionOffsetXQ12; 
    Q12 selectionOffsetYQ12; 
    byte reserved68_9F[56]; 
    RuntimeToken runtimeIdentityOrArmyAssetId; 
    byte reservedA4_B7[20]; 
    Q12 pathCoordinate0Q12; 
    Q12 pathCoordinate1Q12; 
    byte reservedC0_EB[44]; 
    GameEntityRuntimeFlags runtimeFlags; 
    struct GameEntityPathingAndImpactState10 pathingAndImpactState; 
    GameEntityCommandState commandState; 
};

struct GameEntityRuntime {
    struct GameEntityRuntimeCommon common; 
    union GameEntityRuntimeClassPayload classPayload; 
};

union ArmyRuntimeReferenceOrSavedOffset4 {
    struct ArmyRuntimeSlot *armyRuntime; 
    dword savedIdOrOffset; 
    dword raw; 
};

struct FieldGridCell {
    byte runtime00_07[8]; 
    PackedTerrainNormalAngles triangle0NormalAngles; 
    byte runtime0C_3F[52]; 
    Q12 worldX; 
    Q12 worldY; 
    Q12 terrainHeight; 
    Q12 waterSurfaceDelta; 
    enum FieldCellPackedFlagsAndMaterial flagsAndMaterial; 
    FieldCellPersistedAux persistedAux54; 
    byte runtime58_6F[24]; 
    TerrainOccupancyMask occupancyMask; 
    PackedTerrainNormalAngles triangle1NormalAngles; 
    dword runtime7C; 
};

struct AssetProducerSourceNames {
    word producerName[32];
    word sourceName[32];
};

struct AssetBuildTimestampSet {
    AssetPackedDate dateValue0;
    AssetPackedTime timeValue0;
    AssetPackedDate dateValue1;
    AssetPackedTime timeValue1;
    AssetPackedDate dateValue2;
    AssetPackedTime timeValue2;
};

struct GeneratedAssetBuildMetadata {
    struct AssetBuildTimestampSet timestamps;
    byte assetRelativeAddressAnchor28[8]; 
    struct AssetProducerSourceNames names;
};

struct GeneratedAssetCommonPrefix {
    enum AssetMagic magic;
    AssetAllocationSizeBytes allocationSizeBytes;
    AssetFormatVersion formatVersion;
    enum PckConverterVersion converterVersion;
    struct GeneratedAssetBuildMetadata buildMetadata;
};

struct FieldGridAsset {
    struct GeneratedAssetCommonPrefix common; 
    FieldGridFlags fieldFlags; 
    FieldGridRuntimeFlags runtimeStateFlags; 
    FieldGridDimension gridWidth; 
    FieldGridDimension gridHeight; 
    byte reservedC0_FF[64]; 
    word sourcePath[128]; 
    struct FieldGridCell cells[1]; 
};

struct ArmyRuntimeMovementControlState {
    Q12 movementAdvancePerTickQ12; 
    ArmyTurnVelocityAngle16 turnVelocityAngle16; 
};

union ArmyRuntimeContactRadiusOrLinkedSlotMask4 {
    UQ12 contactRadiusQ12; 
    dword linkedChildSlotMask; 
    dword raw; 
};

struct ArmyRuntimeArticulatedContactState14 {
    enum ArmyTerrainContactDispatchMode terrainContactMode; 
    Q12 lateralOffsetQ12; 
    union ArmyRuntimeContactRadiusOrLinkedSlotMask4 contactRadiusOrLinkedSlotMask; 
    Q12 fallbackPosition0Q12; 
    Q12 fallbackPosition1Q12; 
};

struct ArmyRuntimeLinkedChildPendingCounts4 {
    byte slot0; 
    byte slot1; 
    byte slot2; 
    byte reserved03; 
};

struct ArmyRuntimeLinkedChildOverloadedState10 {
    union ArmyRuntimeCoordinateCommandOrHistoryValue4 primaryCoordinateCommandOrHistory; 
    union ArmyRuntimeCoordinateCommandOrHistoryValue4 secondaryCoordinateCommandOrHistory; 
    union ArmyRuntimeCoordinateCommandOrHistoryValue4 leftHeadingCommandOrSpawnValue; 
    union ArmyRuntimeCoordinateCommandOrHistoryValue4 rightHeadingCommandOrSpawnValue; 
};

struct ArmyRuntimeSlot {
    void *definitionOrAsset; 
    struct ModelRuntimeNode *modelNodeRuntime; 
    struct GameEntityRuntime *linkedEntityRuntime; 
    FactionRuntimeIndex factionIndex; 
    struct ArmyRuntimeMovementControlState movementControl; 
    ArmyMovementStateFlags movementStateFlags; 
    struct ArmyRuntimeSlot *commandTargetArmyRuntime; 
    Q12 commandCoordinate0Q12; 
    Q12 commandCoordinate1Q12; 
    Q12 commandCoordinate2Q12; 
    ArmyCommandModeFlags commandModeFlags; 
    ArmyCommandGeneration commandGeneration; 
    Q12 actionVector0Q12; 
    Q12 actionVector1Q12; 
    Q12 actionVector2Q12; 
    dword runtimeState40; 
    dword runtimeState44; 
    dword runtimeState48; 
    dword runtimeState4C; 
    FieldGridRegionMask terrainOccupancyMask0; 
    FieldGridRegionMask terrainOccupancyMask1; 
    Q12 movementPosition0Q12; 
    Q12 movementPosition1Q12; 
    dword classState60; 
    dword ownerValue64; 
    dword ownerValue68; 
    struct ArmyRuntimeSlot *linkedArmyRuntimeOrSavedOffset; 
    Q12 fallbackWorldYQ12; 
    Q12 fallbackWorldXQ12; 
    Q12 movementTarget0Q12; 
    Q12 movementTarget1Q12; 
    dword definitionClassValue80; 
    dword definitionClassValue84; 
    dword definitionClassValue88; 
    dword runtimeState8C; 
    dword runtimeState90; 
    dword runtimeState94; 
    dword runtimeState98; 
    enum ModelRuntimeClassId depthBinClass; 
    enum PckArmyAssetIdCatalog armyAssetId; 
    dword runtimeStateA4; 
    dword runtimeStateA8; 
    struct ArmyRuntimeArticulatedContactState14 articulatedContact; 
    struct ArmyRuntimeLinkedChildOverloadedState10 linkedChildOverloadedState; 
    struct ArmyRuntimeLinkedChildSpawnParameters0C linkedChildSpawnParameters; 
    struct ArmyRuntimeLinkedChildPendingCounts4 linkedChildPendingCounts; 
    byte reservedE0_EB[12]; 
    ArmyRuntimeFlags runtimeFlags; 
    struct ArmyRuntimeSlot *linkedArmyRuntime; 
    ArmyRuntimeTimer runtimeTimer; 
    byte reservedF8_FF[8]; 
    int stateOrTechnologyId; 
    dword runtimeState104; 
    ArmySelectionMetric selectionMetric0; 
    ArmySelectionMetric selectionMetric1; 
    ArmySelectionMetric selectionMetric2; 
    ArmySelectionMetric selectionMetric3; 
    ArmySelectionMetric selectionMetric4; 
    ArmySelectionMetric selectionMetric5; 
};

struct GraphicsPaletteAssetEntry {
    PackedArgb32 argb8888;
    dword reserved04;
};

struct GraphicsPaletteAsset {
    enum AssetMagic magic;
    AssetAllocationSizeBytes allocationSizeBytes;
    byte reserved08_AF[168];
    AssetPaletteBankCount paletteBankCount;
    byte reservedB4_1FF[332];
    struct GraphicsPaletteAssetEntry paletteEntries[1];
};

struct GraphicsTextureResource {
    struct IDirectDrawSurface *deviceSurfaceBase; 
    struct IDirectDrawSurface3 *deviceSurface3; 
    struct IDirect3DTexture2 *deviceTexture2; 
    struct IDirectDrawSurface *stagingSurfaceBase; 
    struct IDirectDrawSurface3 *stagingSurface3; 
    struct IDirect3DTexture2 *stagingTexture2; 
    GraphicsTextureHandle textureHandle; 
    GraphicsResourceUsageSerial lastUsedCounter; 
    struct GraphicsTextureSourceAsset *sourceAsset; 
    GraphicsSubresourceIndex subresourceIndex; 
    struct DDPIXELFORMAT *pixelFormat; 
    enum GraphicsTextureDownsampleShift downsampleShift; 
    struct GrTexInfo glideInfo; 
    enum GraphicsTextureResidentTmuIndex residentTmuIndex; 
    GraphicsTextureMemoryAddress residentAddress; 
    struct GraphicsTextureResource *residentNext; 
};

union ShotDefinitionReferenceOrSavedId4 {
    struct ShotDefinition *definition; 
    enum PckShotDefinitionIdCatalog savedId; 
    dword raw; 
};

struct IDirect3D2_Vtbl {
    TH_LEGACY_HRESULT (*QueryInterface)(struct IDirect3D2 *, struct TH_LEGACY_GUID *, TH_LEGACY_LPVOID *);
    TH_LEGACY_ULONG (*AddRef)(struct IDirect3D2 *);
    TH_LEGACY_ULONG (*Release)(struct IDirect3D2 *);
    sdword (*EnumDevices)(struct IDirect3D2 *, sdword (*)(struct TH_LEGACY_GUID *, char *, char *, struct D3DDEVICEDESC_DX6 *, struct D3DDEVICEDESC_DX6 *, struct GraphicsAdapterRecord *), struct GraphicsAdapterRecord *); 
    TH_LEGACY_HRESULT (*CreateLight)(struct IDirect3D2 *, TH_LEGACY_LPVOID *, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*CreateMaterial)(struct IDirect3D2 *, TH_LEGACY_LPVOID *, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*CreateViewport)(struct IDirect3D2 *, struct IDirect3DViewport2 **, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*FindDevice)(struct IDirect3D2 *, TH_LEGACY_LPVOID, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*CreateDevice)(struct IDirect3D2 *, struct TH_LEGACY_GUID *, struct IDirectDrawSurface *, struct IDirect3DDevice2 **);
};

struct IDirectDrawSurface {
    struct IDirectDrawSurface_Vtbl *lpVtbl;
};

union ModelRuntimeSlotLinkOrState4 {
    struct ModelRuntimeSlot *modelRuntime; 
    dword classState; 
    dword serializedOffset; 
};

struct IDirectDraw_Vtbl {
    TH_LEGACY_HRESULT (*QueryInterface)(struct IDirectDraw *, struct TH_LEGACY_GUID *, TH_LEGACY_LPVOID *);
    TH_LEGACY_ULONG (*AddRef)(struct IDirectDraw *);
    TH_LEGACY_ULONG (*Release)(struct IDirectDraw *);
    TH_LEGACY_HRESULT (*Compact)(struct IDirectDraw *);
    TH_LEGACY_HRESULT (*CreateClipper)(struct IDirectDraw *, TH_LEGACY_DWORD, TH_LEGACY_LPVOID *, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*CreatePalette)(struct IDirectDraw *, TH_LEGACY_DWORD, TH_LEGACY_LPVOID, TH_LEGACY_LPVOID *, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*CreateSurface)(struct IDirectDraw *, struct DDSURFACEDESC_DX6 *, struct IDirectDrawSurface **, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*DuplicateSurface)(struct IDirectDraw *, struct IDirectDrawSurface *, struct IDirectDrawSurface **);
    sdword (*EnumDisplayModes)(struct IDirectDraw *, dword, struct DDSURFACEDESC_DX6 *, dword, sdword (*)(struct DDSURFACEDESC_DX6 *, dword)); 
    TH_LEGACY_HRESULT (*EnumSurfaces)(struct IDirectDraw *, TH_LEGACY_DWORD, struct DDSURFACEDESC_DX6 *, TH_LEGACY_LPVOID, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*FlipToGDISurface)(struct IDirectDraw *);
    TH_LEGACY_HRESULT (*GetCaps)(struct IDirectDraw *, TH_LEGACY_LPVOID, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*GetDisplayMode)(struct IDirectDraw *, struct DDSURFACEDESC_DX6 *);
    TH_LEGACY_HRESULT (*GetFourCCCodes)(struct IDirectDraw *, TH_LEGACY_DWORD *, TH_LEGACY_DWORD *);
    TH_LEGACY_HRESULT (*GetGDISurface)(struct IDirectDraw *, struct IDirectDrawSurface **);
    TH_LEGACY_HRESULT (*GetMonitorFrequency)(struct IDirectDraw *, TH_LEGACY_DWORD *);
    TH_LEGACY_HRESULT (*GetScanLine)(struct IDirectDraw *, TH_LEGACY_DWORD *);
    TH_LEGACY_HRESULT (*GetVerticalBlankStatus)(struct IDirectDraw *, TH_LEGACY_BOOL *);
    TH_LEGACY_HRESULT (*Initialize)(struct IDirectDraw *, struct TH_LEGACY_GUID *);
    TH_LEGACY_HRESULT (*RestoreDisplayMode)(struct IDirectDraw *);
    TH_LEGACY_HRESULT (*SetCooperativeLevel)(struct IDirectDraw *, TH_LEGACY_HWND, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*SetDisplayMode)(struct IDirectDraw *, TH_LEGACY_DWORD, TH_LEGACY_DWORD, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*WaitForVerticalBlank)(struct IDirectDraw *, TH_LEGACY_DWORD, TH_LEGACY_HANDLE);
};

struct GraphicsTextureSetEntry {
    struct GraphicsTextureResource *texture; 
    GraphicsTextureDimensionLog2 widthLog2; 
    GraphicsTextureDimensionLog2 heightLog2; 
    struct GraphicsTextureSourceAsset *sourceAsset; 
    struct GraphicsTextureSourceEntry *sourceEntry; 
    GraphicsSubresourceIndex subresourceIndex; 
    dword reserved18; 
    dword reserved1C; 
};

struct ModelRuntimeSlotClassState7C {
    byte reserved84_A7[36]; 
    dword classStateA8; 
    dword classStateAC; 
    sdword classStateB0; 
    sdword classStateB4; 
    dword classStateB8; 
    byte reservedBC_CF[20]; 
    sdword classStateD0; 
    byte reservedD4_DB[8]; 
    dword classStateDC; 
    dword classStateE0; 
    dword enabledStateE4; 
    dword enabledStateE8; 
    dword classStateEC; 
    union ArmyRuntimeReferenceOrSavedOffset4 linkedArmyRuntimeOrSavedOffset; 
    dword definitionDerivedValueF4; 
    dword classStateF8; 
    dword classStateFC; 
};

struct ModelRuntimeClassLinkState24 {
    union ModelRuntimeSlotLinkOrState4 modelLinkOrState60; 
    dword classState64; 
    dword classState68; 
    union ModelRuntimeArmyLinkOrState4 armyLinkOrState6C; 
    dword classState70; 
    dword classState74; 
    dword classState78; 
    dword classState7C; 
    dword classState80; 
};

struct ShotRuntimeOwnerAndTrajectoryLiveState18 {
    struct ArmyRuntimeSlot *ownerArmyRuntime; 
    Q12 directionComponent0Q12; 
    Q12 directionComponent1Q12; 
    Q12 directionComponent2Q12; 
    ShotAnimationFrameIndex animationFrameIndex; 
    ShotSecondaryEffectCountdownTicks secondaryEffectCountdownTicks; 
};

union ShotModelNodeReferenceOrSavedOffset4 {
    struct ModelRuntimeNode *modelNode; 
    dword savedIdOrOffset; 
    dword raw; 
};

union ShotModelRuntimeStateOrSavedOffset4 {
    dword runtimeState; 
    dword savedIdOrOffset; 
    dword raw; 
    void *runtimeStatePointer; 
};

struct ShotRuntimeSlot {
    union ShotDefinitionReferenceOrSavedId4 definitionOrSavedId; 
    Q12 launchSpeedQ12; 
    dword terrainRuntimeClassState; 
    ShotAnimationFrameAccumulatorQ4 animationFrameAccumulatorQ4; 
    union ShotModelNodeReferenceOrSavedOffset4 modelNodeOrSavedOffset; 
    union ShotModelRuntimeStateOrSavedOffset4 runtimeStateOrSavedOffset; 
    AngleTurn16Stored32 elevationOffsetAngle16; 
    ShotImpactEffectEmissionFlags impactEffectEmissionFlags; 
    ShotProjectileAgeTicks projectileAgeTicks; 
    ShotLifetimeRemainingTicks lifetimeTicksRemaining; 
    struct ShotRuntimeOwnerAndTrajectoryLiveState18 ownerAndTrajectory; 
};

struct IDirect3DViewport2 {
    struct IDirect3DViewport2_Vtbl *lpVtbl;
};

struct WorldRuntimeNodeCommon {
    struct WorldRuntimeNode *previousNode; 
    struct WorldRuntimeNode *nextNode; 
    struct WorldRuntimeContext *ownerWorld; 
};

struct EffectDefinitionTransitionPrefix8 {
    enum EffectLifecycleTransitionKind transitionKind; 
    dword reserved04; 
};

struct EffectDefinition {
    struct EffectDefinitionTransitionPrefix8 transitionPrefix; 
    enum PckEffectDefinitionIdCatalog definitionId; 
    EffectAnimationFrameCount animationFrameCount; 
    DefinitionReferencePresentFlag linkedEffectPresent; 
    struct EffectDefinition *linkedEffectDefinition; 
    DefinitionReferencePresentFlag linkedShotPresent; 
    struct ShotDefinition *linkedShotDefinition; 
    EffectMovementSpeedQ12 movementSpeedQ12; 
    dword runtimeValue24; 
    EffectFrameAdvanceThresholdQ4 frameAdvanceThresholdQ4; 
    TerrainGridMaskIndex terrainGridMaskIndex; 
    enum EffectCreationFlagBits creationFlags; 
    PackedArgb32 shadingColorArgb; 
    GraphicsTransitionTickCount shadingTransitionDurationTicks; 
    GraphicsTransitionTickCount shadingReleaseTransitionDurationTicks; 
    EffectShadingCountdownTicks shadingStartCountdownTicks; 
    EffectShadingCountdownTicks shadingStopCountdownTicks; 
    struct EffectDefinition *periodicEffectDefinition; 
    EffectPeriodicIntervalTicks periodicEffectIntervalTicks; 
    EffectAlphaFadeTicks alphaFadeInTicks; 
    EffectAlphaFadeTicks alphaFadeOutTicks; 
    dword unknown58; 
    dword unknown5C; 
    PackedArgb32 stateTintArgb; 
    Q12 modelScaleStartQ12; 
    Q12 modelScaleEndQ12; 
    SpatialSoundGainQ15 positionedSoundGainQ15; 
    SpatialSoundMaximumDistanceQ12 positionedSoundMaximumDistanceQ12; 
    void *ownedNestedResource; 
    OwnedNestedResourceFlag ownedNestedResourcePresent; 
    word resourcePathUtf16[34]; 
};

union ModelRuntimePayloadReference4 {
    struct ModelRuntimeSlot *modelRuntime; 
    struct ArmyRuntimeSlot *armyRuntime; 
    struct EffectRuntimeSlot *effectRuntime; 
    struct ShotRuntimeSlot *shotRuntime; 
    void *opaqueRuntime; 
    dword savedOffsetOrRaw; 
};

struct WorldRuntimeNodeModelPayload {
    AngleTurn32 worldRotationAngle0; 
    AngleTurn32 worldRotationAngle1; 
    AngleTurn32 worldRotationAngle2; 
    Q12 localTranslationXQ12; 
    Q12 localTranslationYQ12; 
    Q12 localTranslationZQ12; 
    AngleTurn32 localRotationAngle0; 
    AngleTurn32 localRotationAngle1; 
    AngleTurn32 localRotationAngle2; 
    struct GraphicsPaletteAsset *paletteAsset; 
    struct GraphicsTextureSet *textureSet; 
    byte reserved2C_33[8]; 
    struct ModelResourceHitTestAndRenderView210 *modelResource; 
    ModelMeshGroupMask meshGroupMask; 
};

struct ModelRuntimeNode {
    struct WorldRuntimeNodeCommon common; 
    struct WorldRuntimeNodeModelPayload modelPayload; 
    union ModelRuntimePayloadReference4 runtimePayload; 
    ModelRuntimeFlags runtimeFlags; 
    ModelTextureSubresourceIndex textureSubresourceBaseIndex; 
    Q12 subtreeBoundingRadiusQ12; 
    PackedArgb32 tintArgb; 
    struct GraphicsShadingRuntimeRecord *shadingRecord; 
    void *modelRuntimeLinkOrSavedOffset; 
    ModelTextureSubresourceIndex primaryAnimatedSubresourceIndex; 
    ModelTextureOffsetTexel primaryTextureOffsetU; 
    ModelTextureOffsetTexel primaryTextureOffsetV; 
    struct GraphicsFixedMatrix3x4 worldTransform; 
    dword runtimeStateA0; 
    enum ModelRuntimeClassId ownerClassId; 
    ModelTextureSubresourceIndex secondaryAnimatedSubresourceIndex; 
    ModelTextureOffsetTexel secondaryTextureOffsetU; 
    ModelTextureOffsetTexel secondaryTextureOffsetV; 
    ModelDepthBinMask depthBinMaskNear; 
    ModelDepthBinMask depthBinMaskFar; 
    int renderDepthBiasOrState; 
    Q12 modelScaleQ12; 
    struct ModelRuntimeNode *parentNode; 
    dword childCount; 
    struct ModelRuntimeNode *childNodes[13]; 
};

union ModelRuntimeNodeReferenceOrSavedOffset4 {
    struct ModelRuntimeNode *modelNode; 
    dword savedIdOrOffset; 
    dword raw; 
};

struct ModelRuntimeAttachmentDescriptor {
    struct ModelRuntimeSlot *childModelRuntimeOrSavedOffset00; 
    struct ModelAttachmentTransformRecord *sourceTransform04; 
    struct ModelRuntimeNode *parentModelNodeOrSavedOffset08; 
    ModelChildNodeIndex childNodeIndex0C; 
    AngleTurn32 childLocalRotationAngle0; 
    AngleTurn32 childLocalRotationAngle1; 
    AngleTurn32 childLocalRotationAngle2; 
    dword reserved1C; 
};

union ModelDefinitionReferenceOrSavedId4 {
    struct ModelDefinitionRecordPrefix *definition; 
    dword savedIdOrOffset; 
    dword raw; 
};

union ModelRuntimeSlotReferenceOrSavedOffset4 {
    struct ModelRuntimeSlot *modelRuntime; 
    dword savedIdOrOffset; 
    dword raw; 
};

struct ModelRuntimeSlot {
    union ModelDefinitionReferenceOrSavedId4 definitionOrSavedId; 
    union ModelRuntimeNodeReferenceOrSavedOffset4 rootModelNodeOrSavedOffset; 
    union ArmyRuntimeReferenceOrSavedOffset4 ownerArmyRuntimeOrSavedOffset; 
    dword attachmentCount0C; 
    byte reserved10_37[40]; 
    union ModelRuntimeSlotReferenceOrSavedOffset4 linkedModelRuntimeOrSavedOffset; 
    dword definitionValue60_3C; 
    dword definitionValue84_40; 
    dword definitionValue88_44; 
    dword definitionValue94_48; 
    dword definitionValue9C_4C; 
    dword definitionValueA4_50; 
    dword definitionValueAC_54; 
    dword definitionValueB4_58; 
    dword definitionValueBC_5C; 
    struct ModelRuntimeClassLinkState24 classLinkState; 
    struct ModelRuntimeSlotClassState7C classState; 
    byte reserved100_117[24]; 
    dword classState118; 
    dword classState11C; 
    byte reserved120_13F[32]; 
    struct ModelRuntimeAttachmentDescriptor attachments140[6]; 
};

union WorldRuntimeNodePayload {
    byte opaque[60]; 
    struct WorldRuntimeNodeModelPayload model; 
};

struct WorldRuntimeNode {
    struct WorldRuntimeNodeCommon common; 
    union WorldRuntimeNodePayload classPayload; 
    void *runtimePayload; 
    WorldRuntimeNodeFlags runtimeFlags; 
};

struct TH_LEGACY_RECT {
    TH_LEGACY_LONG left;
    TH_LEGACY_LONG top;
    TH_LEGACY_LONG right;
    TH_LEGACY_LONG bottom;
};

struct GraphicsTextureSourceEntry {
    AssetDimension logicalWidth;
    AssetDimension logicalHeight;
    GraphicsPaletteIndex paletteIndex; 
    AssetRelativeOffset dataOffset;
    GraphicsPixelOrigin originX; 
    GraphicsPixelOrigin originY; 
    AssetDimension pixelWidth;
    AssetDimension pixelHeight;
};

struct IDirectDraw {
    struct IDirectDraw_Vtbl *lpVtbl;
};

struct IDirect3DDevice2_Vtbl {
    TH_LEGACY_HRESULT (*QueryInterface)(struct IDirect3DDevice2 *, struct TH_LEGACY_GUID *, TH_LEGACY_LPVOID *);
    TH_LEGACY_ULONG (*AddRef)(struct IDirect3DDevice2 *);
    TH_LEGACY_ULONG (*Release)(struct IDirect3DDevice2 *);
    TH_LEGACY_HRESULT (*GetCaps)(struct IDirect3DDevice2 *, TH_LEGACY_LPVOID, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*SwapTextureHandles)(struct IDirect3DDevice2 *, TH_LEGACY_LPVOID, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*GetStats)(struct IDirect3DDevice2 *, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*AddViewport)(struct IDirect3DDevice2 *, struct IDirect3DViewport2 *);
    TH_LEGACY_HRESULT (*DeleteViewport)(struct IDirect3DDevice2 *, struct IDirect3DViewport2 *);
    TH_LEGACY_HRESULT (*NextViewport)(struct IDirect3DDevice2 *, struct IDirect3DViewport2 *, struct IDirect3DViewport2 **, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*EnumTextureFormats)(struct IDirect3DDevice2 *, TH_LEGACY_LPVOID, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*BeginScene)(struct IDirect3DDevice2 *);
    TH_LEGACY_HRESULT (*EndScene)(struct IDirect3DDevice2 *);
    TH_LEGACY_HRESULT (*GetDirect3D)(struct IDirect3DDevice2 *, struct IDirect3D2 **);
    TH_LEGACY_HRESULT (*SetCurrentViewport)(struct IDirect3DDevice2 *, struct IDirect3DViewport2 *);
    TH_LEGACY_HRESULT (*GetCurrentViewport)(struct IDirect3DDevice2 *, struct IDirect3DViewport2 **);
    TH_LEGACY_HRESULT (*SetRenderTarget)(struct IDirect3DDevice2 *, struct IDirectDrawSurface *, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*GetRenderTarget)(struct IDirect3DDevice2 *, struct IDirectDrawSurface **);
    TH_LEGACY_HRESULT (*Begin)(struct IDirect3DDevice2 *, TH_LEGACY_DWORD, TH_LEGACY_DWORD, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*BeginIndexed)(struct IDirect3DDevice2 *, TH_LEGACY_DWORD, TH_LEGACY_DWORD, TH_LEGACY_LPVOID, TH_LEGACY_DWORD, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*Vertex)(struct IDirect3DDevice2 *, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*Index)(struct IDirect3DDevice2 *, TH_LEGACY_WORD);
    TH_LEGACY_HRESULT (*End)(struct IDirect3DDevice2 *, TH_LEGACY_DWORD);
    sdword (*GetRenderState)(struct IDirect3DDevice2 *, enum D3DRENDERSTATETYPE_DX6, dword *); 
    sdword (*SetRenderState)(struct IDirect3DDevice2 *, enum D3DRENDERSTATETYPE_DX6, dword); 
    TH_LEGACY_HRESULT (*GetLightState)(struct IDirect3DDevice2 *, TH_LEGACY_DWORD, TH_LEGACY_DWORD *);
    TH_LEGACY_HRESULT (*SetLightState)(struct IDirect3DDevice2 *, TH_LEGACY_DWORD, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*SetTransform)(struct IDirect3DDevice2 *, TH_LEGACY_DWORD, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*GetTransform)(struct IDirect3DDevice2 *, TH_LEGACY_DWORD, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*MultiplyTransform)(struct IDirect3DDevice2 *, TH_LEGACY_DWORD, TH_LEGACY_LPVOID);
    sdword (*DrawPrimitive)(struct IDirect3DDevice2 *, dword, dword, struct D3DTLVERTEX_DX6 *, dword, dword); 
    TH_LEGACY_HRESULT (*DrawIndexedPrimitive)(struct IDirect3DDevice2 *, TH_LEGACY_DWORD, TH_LEGACY_DWORD, TH_LEGACY_LPVOID, TH_LEGACY_DWORD, TH_LEGACY_WORD *, TH_LEGACY_DWORD, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*SetClipStatus)(struct IDirect3DDevice2 *, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*GetClipStatus)(struct IDirect3DDevice2 *, TH_LEGACY_LPVOID);
};

struct D3DVIEWPORT2 {
    TH_LEGACY_DWORD dwSize;
    TH_LEGACY_DWORD dwX;
    TH_LEGACY_DWORD dwY;
    TH_LEGACY_DWORD dwWidth;
    TH_LEGACY_DWORD dwHeight;
    float dvClipX;
    float dvClipY;
    float dvClipWidth;
    float dvClipHeight;
    float dvMinZ;
    float dvMaxZ;
};

struct D3DTLVERTEX_DX6 {
    float sx;
    float sy;
    float sz;
    float rhw;
    dword color;
    dword specular;
    float tu;
    float tv;
};

struct GraphicsTextureSourceTableDescriptor {
    AssetSubresourceCount subresourceCount;
    AssetPaletteBankCount paletteBankCount;
    AssetRelativeOffset subresourceTableOffset;
};

struct DDPIXELFORMAT {
    TH_LEGACY_DWORD dwSize;
    TH_LEGACY_DWORD dwFlags;
    TH_LEGACY_DWORD dwFourCC; 
    TH_LEGACY_DWORD dwRGBBitCount; 
    TH_LEGACY_DWORD dwRBitMask; 
    TH_LEGACY_DWORD dwGBitMask; 
    TH_LEGACY_DWORD dwBBitMask; 
    TH_LEGACY_DWORD dwRGBAlphaBitMask; 
};

struct IDirectDrawSurface_Vtbl {
    TH_LEGACY_HRESULT (*QueryInterface)(struct IDirectDrawSurface *, struct TH_LEGACY_GUID *, TH_LEGACY_LPVOID *);
    TH_LEGACY_ULONG (*AddRef)(struct IDirectDrawSurface *);
    TH_LEGACY_ULONG (*Release)(struct IDirectDrawSurface *);
    TH_LEGACY_HRESULT (*AddAttachedSurface)(struct IDirectDrawSurface *, struct IDirectDrawSurface *);
    TH_LEGACY_HRESULT (*AddOverlayDirtyRect)(struct IDirectDrawSurface *, struct TH_LEGACY_RECT *);
    TH_LEGACY_HRESULT (*Blt)(struct IDirectDrawSurface *, struct TH_LEGACY_RECT *, struct IDirectDrawSurface *, struct TH_LEGACY_RECT *, TH_LEGACY_DWORD, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*BltBatch)(struct IDirectDrawSurface *, TH_LEGACY_LPVOID, TH_LEGACY_DWORD, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*BltFast)(struct IDirectDrawSurface *, TH_LEGACY_DWORD, TH_LEGACY_DWORD, struct IDirectDrawSurface *, struct TH_LEGACY_RECT *, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*DeleteAttachedSurface)(struct IDirectDrawSurface *, TH_LEGACY_DWORD, struct IDirectDrawSurface *);
    TH_LEGACY_HRESULT (*EnumAttachedSurfaces)(struct IDirectDrawSurface *, TH_LEGACY_LPVOID, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*EnumOverlayZOrders)(struct IDirectDrawSurface *, TH_LEGACY_DWORD, TH_LEGACY_LPVOID, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*Flip)(struct IDirectDrawSurface *, struct IDirectDrawSurface *, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*GetAttachedSurface)(struct IDirectDrawSurface *, struct DDSCAPS *, struct IDirectDrawSurface **);
    TH_LEGACY_HRESULT (*GetBltStatus)(struct IDirectDrawSurface *, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*GetCaps)(struct IDirectDrawSurface *, struct DDSCAPS *);
    TH_LEGACY_HRESULT (*GetClipper)(struct IDirectDrawSurface *, TH_LEGACY_LPVOID *);
    TH_LEGACY_HRESULT (*GetColorKey)(struct IDirectDrawSurface *, TH_LEGACY_DWORD, struct DDCOLORKEY *);
    TH_LEGACY_HRESULT (*GetDC)(struct IDirectDrawSurface *, TH_LEGACY_HANDLE *);
    TH_LEGACY_HRESULT (*GetFlipStatus)(struct IDirectDrawSurface *, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*GetOverlayPosition)(struct IDirectDrawSurface *, TH_LEGACY_LONG *, TH_LEGACY_LONG *);
    TH_LEGACY_HRESULT (*GetPalette)(struct IDirectDrawSurface *, TH_LEGACY_LPVOID *);
    TH_LEGACY_HRESULT (*GetPixelFormat)(struct IDirectDrawSurface *, struct DDPIXELFORMAT *);
    TH_LEGACY_HRESULT (*GetSurfaceDesc)(struct IDirectDrawSurface *, struct DDSURFACEDESC_DX6 *);
    TH_LEGACY_HRESULT (*Initialize)(struct IDirectDrawSurface *, struct IDirectDraw *, struct DDSURFACEDESC_DX6 *);
    TH_LEGACY_HRESULT (*IsLost)(struct IDirectDrawSurface *);
    TH_LEGACY_HRESULT (*Lock)(struct IDirectDrawSurface *, struct TH_LEGACY_RECT *, struct DDSURFACEDESC_DX6 *, TH_LEGACY_DWORD, TH_LEGACY_HANDLE);
    TH_LEGACY_HRESULT (*ReleaseDC)(struct IDirectDrawSurface *, TH_LEGACY_HANDLE);
    TH_LEGACY_HRESULT (*Restore)(struct IDirectDrawSurface *);
    TH_LEGACY_HRESULT (*SetClipper)(struct IDirectDrawSurface *, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*SetColorKey)(struct IDirectDrawSurface *, TH_LEGACY_DWORD, struct DDCOLORKEY *);
    TH_LEGACY_HRESULT (*SetOverlayPosition)(struct IDirectDrawSurface *, TH_LEGACY_LONG, TH_LEGACY_LONG);
    TH_LEGACY_HRESULT (*SetPalette)(struct IDirectDrawSurface *, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*Unlock)(struct IDirectDrawSurface *, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*UpdateOverlay)(struct IDirectDrawSurface *, struct TH_LEGACY_RECT *, struct IDirectDrawSurface *, struct TH_LEGACY_RECT *, TH_LEGACY_DWORD, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*UpdateOverlayDisplay)(struct IDirectDrawSurface *, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*UpdateOverlayZOrder)(struct IDirectDrawSurface *, TH_LEGACY_DWORD, struct IDirectDrawSurface *);
};

struct IDirect3DTexture2 {
    struct IDirect3DTexture2_Vtbl *lpVtbl;
};

struct DDSCAPS {
    TH_LEGACY_DWORD dwCaps;
};

struct DDSURFACEDESC_DX6 {
    TH_LEGACY_DWORD dwSize;
    TH_LEGACY_DWORD dwFlags;
    TH_LEGACY_DWORD dwHeight;
    TH_LEGACY_DWORD dwWidth;
    TH_LEGACY_LONG lPitch; 
    TH_LEGACY_DWORD dwBackBufferCount;
    TH_LEGACY_DWORD dwMipMapCount; 
    TH_LEGACY_DWORD dwAlphaBitDepth;
    TH_LEGACY_DWORD dwReserved;
    TH_LEGACY_LPVOID lpSurface;
    struct DDCOLORKEY ddckCKDestOverlay;
    struct DDCOLORKEY ddckCKDestBlt;
    struct DDCOLORKEY ddckCKSrcOverlay;
    struct DDCOLORKEY ddckCKSrcBlt;
    struct DDPIXELFORMAT ddpfPixelFormat;
    struct DDSCAPS ddsCaps;
};

union EffectDefinitionReferenceOrSavedId4 {
    struct EffectDefinition *definition; 
    enum PckEffectDefinitionIdCatalog savedId; 
    dword raw; 
};

struct GraphicsShadingRuntimeRecord {
    GraphicsWorldCoordinateQ12 worldXQ12; 
    GraphicsWorldCoordinateQ12 worldYQ12; 
    GraphicsWorldCoordinateQ12 worldZQ12; 
    PackedRgb24 packedColorRgbActive; 
    qword squaredRadiusQ24; 
    GraphicsTransitionTickCount radiusTransitionDurationTicks; 
    GraphicsTransitionTickCount radiusTransitionElapsedTicks; 
    GraphicsRadiusQ12 targetRadiusQ12; 
    byte opaque24_3B[24]; 
    dword serializationToggleDword; 
};

struct IDirectDrawSurface3 {
    struct IDirectDrawSurface3_Vtbl *lpVtbl;
};

struct EffectRuntimeSlot {
    union EffectDefinitionReferenceOrSavedId4 definitionOrSavedId; 
    union EffectModelNodeReferenceOrSavedOffset4 modelNodeOrSavedOffset; 
    EffectAnimationFrameCount animationFramesRemaining; 
    DefinitionReferencePresentFlag linkedEffectPresent; 
    DefinitionReferencePresentFlag linkedShotPresent; 
    struct EffectRuntimeLifecycleState10 lifecycleOwnerAndDefinition; 
    enum EffectRuntimeCompletionAction completionAction; 
    EffectShadingCountdownTicks shadingStartCountdownTicksRemaining; 
    EffectShadingCountdownTicks shadingStopCountdownTicksRemaining; 
    EffectPeriodicIntervalTicks periodicEffectCountdownTicks; 
    dword terrainRuntimeClassState; 
    PackedArgb32 stateTintArgb; 
    EffectAgeTicks effectAgeTicks; 
};

struct GraphicsTextureSet {
    struct GraphicsTextureSourceAsset *sourceAsset; 
    dword subresourceCount; 
    struct GraphicsTextureSetEntry entries[1]; 
};

struct GraphicsTextureSourceAsset {
    struct GeneratedAssetCommonPrefix common; 
    struct GraphicsTextureSourceTableDescriptor tableDescriptor; 
    byte opaqueTablePayloadBC_1FF[324]; 
};

typedef union UiCommandPayloadTextBatch48 UiCommandPayloadTextBatch48, *PUiCommandPayloadTextBatch48;

typedef struct UiCommandPayloadTriple12 UiCommandPayloadTriple12, *PUiCommandPayloadTriple12;

struct UiCommandPayloadTriple12 {
    dword payloadDword04; 
    dword payloadDword08; 
    dword payloadDword0C; 
};

union UiCommandPayloadTextBatch48 {
    byte textBytes[48]; 
    struct UiCommandPayloadTriple12 triples[4]; 
};

typedef union NetworkEndpointAddressHeader4 NetworkEndpointAddressHeader4, *PNetworkEndpointAddressHeader4;

typedef struct NetworkEndpointFamilyPortFields4 NetworkEndpointFamilyPortFields4, *PNetworkEndpointFamilyPortFields4;

typedef enum NetworkAddressFamily {
    NETWORK_ADDRESS_FAMILY_UNSPECIFIED=0,
    NETWORK_ADDRESS_FAMILY_IPV4=2
} NetworkAddressFamily;

typedef word NetworkPortNetworkOrder;

struct NetworkEndpointFamilyPortFields4 {
    enum NetworkAddressFamily addressFamily; 
    NetworkPortNetworkOrder portNetworkOrder; 
};

union NetworkEndpointAddressHeader4 {
    struct NetworkEndpointFamilyPortFields4 fields; 
    dword packedFamilyAndPort; 
};

typedef union PckHuffmanSymbolState PckHuffmanSymbolState, *PPckHuffmanSymbolState;

typedef dword PckHuffmanFrequencyCount;

typedef dword PckHuffmanPackedCodeAndBitLength;

union PckHuffmanSymbolState {
    PckHuffmanFrequencyCount frequencyCount; 
    PckHuffmanPackedCodeAndBitLength packedCodeAndBitLength; 
};

typedef union SpriteAssetReferenceOrSavedId4 SpriteAssetReferenceOrSavedId4, *PSpriteAssetReferenceOrSavedId4;

typedef struct SpriteAssetHeader SpriteAssetHeader, *PSpriteAssetHeader;

typedef dword AssetRegistryId;

typedef struct GeneratedAssetRegistryHeader GeneratedAssetRegistryHeader, *PGeneratedAssetRegistryHeader;

typedef dword AssetRecordCount;

typedef dword SpriteAssetId;

union SpriteAssetReferenceOrSavedId4 {
    struct SpriteAssetHeader *spriteAsset; 
    AssetRegistryId savedId; 
    dword raw; 
};

struct GeneratedAssetRegistryHeader {
    struct GeneratedAssetCommonPrefix common;
    AssetRecordCount groupCount;
    struct SpriteAssetHeader *previousRegistryAsset;
    SpriteAssetId registryId; 
};

struct SpriteAssetHeader {
    struct GeneratedAssetRegistryHeader registryHeader;
    byte reservedBC_1FF[324];
};

typedef union Utf16DecimalDigitPair4 Utf16DecimalDigitPair4, *PUtf16DecimalDigitPair4;

union Utf16DecimalDigitPair4 {
    word codeUnits[2]; 
    dword packedDigits; 
};

typedef union WidePathBuffer256 WidePathBuffer256, *PWidePathBuffer256;

union WidePathBuffer256 {
    word codeUnits[256]; 
    dword firstTwoCodeUnits; 
};

typedef union FrontendUiSemanticScratch280 FrontendUiSemanticScratch280, *PFrontendUiSemanticScratch280;

typedef struct FrontendDisplayModeCandidateScratch28 FrontendDisplayModeCandidateScratch28, *PFrontendDisplayModeCandidateScratch28;

typedef struct FrontendTaskAssignmentGeneratedFactionTextState280 FrontendTaskAssignmentGeneratedFactionTextState280, *PFrontendTaskAssignmentGeneratedFactionTextState280;

typedef struct FrontendDisplayModeEnumerationState280 FrontendDisplayModeEnumerationState280, *PFrontendDisplayModeEnumerationState280;

typedef struct FrontendTaskAssignmentGeneratedFactionTextRow50 FrontendTaskAssignmentGeneratedFactionTextRow50, *PFrontendTaskAssignmentGeneratedFactionTextRow50;

typedef union FrontendDisplayModeScratch28 FrontendDisplayModeScratch28, *PFrontendDisplayModeScratch28;

typedef struct FrontendDisplayModePersistentSelection10 FrontendDisplayModePersistentSelection10, *PFrontendDisplayModePersistentSelection10;

typedef int FrontendDisplayAdapterIndex;

typedef dword FrontendDisplayDimensionPixels;

typedef dword FrontendColorDepthBits;

struct FrontendTaskAssignmentGeneratedFactionTextRow50 {
    word textUtf16[40]; 
};

struct FrontendDisplayModePersistentSelection10 {
    FrontendDisplayAdapterIndex adapterIndex; 
    FrontendDisplayDimensionPixels width; 
    FrontendDisplayDimensionPixels height; 
    FrontendColorDepthBits bitsPerPixel; 
};

union FrontendDisplayModeScratch28 {
    dword bitsPerPixelCandidates4[4]; 
    dword packedResolutionCandidates10[10]; 
    dword rawDwords[10]; 
};

struct FrontendDisplayModeEnumerationState280 {
    union FrontendDisplayModeScratch28 candidates; 
    byte reserved0028_003F[24]; 
    struct FrontendDisplayModePersistentSelection10 persistentSelection; 
    byte reserved0050_027F[560]; 
};

struct FrontendDisplayModeCandidateScratch28 {
    dword candidateValues10[10]; 
};

struct FrontendTaskAssignmentGeneratedFactionTextState280 {
    struct FrontendTaskAssignmentGeneratedFactionTextRow50 rows[8]; 
};

union FrontendUiSemanticScratch280 {
    struct FrontendDisplayModeCandidateScratch28 displayModeScratch; 
    struct FrontendTaskAssignmentGeneratedFactionTextState280 taskAssignmentText; 
    struct FrontendDisplayModeEnumerationState280 displayEnumeration; 
    byte raw[640]; 
};

typedef qword ArmyUnlockedArmourAggregateEaxPreservedEdxCarrier64;

typedef struct GridPathNearestCellCfRegisterResult GridPathNearestCellCfRegisterResult, *PGridPathNearestCellCfRegisterResult;

typedef int FieldGridCellCoordinate;

struct GridPathNearestCellCfRegisterResult {
    FieldGridCellCoordinate selectedColumn; 
    FieldGridCellCoordinate selectedRow; 
    dword preservedEdxCompatibility; 
};

typedef qword AiWorkspaceCountEaxPreservedEdxCarrier64;

typedef struct ModelTechnologyHierarchyCfVolatileContinuityResult ModelTechnologyHierarchyCfVolatileContinuityResult, *PModelTechnologyHierarchyCfVolatileContinuityResult;

typedef int ModelDefinitionHierarchyNodeAddress32;

struct ModelTechnologyHierarchyCfVolatileContinuityResult {
    ModelDefinitionHierarchyNodeAddress32 preservedEaxDefinitionNodeAddress; 
    dword preservedEcxCallerValue; 
    dword preservedEdxCallerValue; 
};

typedef qword AiCandidateScoreCfEaxPreservedEdxCarrier64;

typedef struct GridPathBacktrackCfRegisterResult GridPathBacktrackCfRegisterResult, *PGridPathBacktrackCfRegisterResult;

struct GridPathBacktrackCfRegisterResult {
    FieldGridCellCoordinate selectedColumn; 
    FieldGridCellCoordinate selectedRow; 
    FieldGridRegionMask routeStateMask; 
};

typedef struct AiPreservedFactionIndexEdxResult AiPreservedFactionIndexEdxResult, *PAiPreservedFactionIndexEdxResult;

struct AiPreservedFactionIndexEdxResult {
    FactionRuntimeIndex preservedEdxFactionIndex; 
};

typedef qword AiCombatTargetSelectionResult64;

typedef struct AiTechnologyPlanningLoopRegisterContinuityResult AiTechnologyPlanningLoopRegisterContinuityResult, *PAiTechnologyPlanningLoopRegisterContinuityResult;

struct AiTechnologyPlanningLoopRegisterContinuityResult {
    dword preservedEaxTechnologyPanelIndex; 
    dword preservedEcxSourceArmyEntriesRemaining; 
    dword preservedEdxFactionRecordOffset; 
};

typedef struct GameFactionCapabilityCfVolatileContinuityResult GameFactionCapabilityCfVolatileContinuityResult, *PGameFactionCapabilityCfVolatileContinuityResult;

struct GameFactionCapabilityCfVolatileContinuityResult {
    dword preservedEaxCallerValue; 
    dword preservedEcxCallerValue; 
    dword preservedEdxCallerValue; 
};

typedef struct EntityPathingDestinationRegisterResult EntityPathingDestinationRegisterResult, *PEntityPathingDestinationRegisterResult;

struct EntityPathingDestinationRegisterResult {
    Q12 primaryWorldXQ12; 
    Q12 primaryWorldYQ12; 
    Q12 fallbackWorldXQ12; 
    Q12 fallbackWorldYQ12; 
};

typedef qword EnergyQ4AggregateEaxPreservedEdxCarrier64;

typedef qword DepthBinMaskEaxPreservedEdxCarrier64;

typedef qword AiWorkspaceEntryValueEaxPreservedEdxCarrier64;

typedef struct ModelRaycastNearestHitCfRegisterResult ModelRaycastNearestHitCfRegisterResult, *PModelRaycastNearestHitCfRegisterResult;

struct ModelRaycastNearestHitCfRegisterResult {
    Q12 nearestDistanceQ12; 
    struct ModelRuntimeNode *nearestModelNode; 
};

typedef struct AiWorkspace09AnchorCfRegisterResult AiWorkspace09AnchorCfRegisterResult, *PAiWorkspace09AnchorCfRegisterResult;

struct AiWorkspace09AnchorCfRegisterResult {
    Q12 worldYQ12; 
    Q12 worldXQ12; 
};

typedef struct AiPlanningGridScanLoopContinuityResult AiPlanningGridScanLoopContinuityResult, *PAiPlanningGridScanLoopContinuityResult;

struct AiPlanningGridScanLoopContinuityResult {
    dword preservedEcxInteriorGridRowCount; 
    dword preservedEdxGridScratchRowStrideBytes; 
};

typedef struct AiFactionPlanningCallerLoopRegisterContinuityResult AiFactionPlanningCallerLoopRegisterContinuityResult, *PAiFactionPlanningCallerLoopRegisterContinuityResult;

struct AiFactionPlanningCallerLoopRegisterContinuityResult {
    FactionRuntimeIndex preservedEaxFactionIndex; 
    dword preservedEcxRemainingFactionCount; 
};

typedef struct AiPlanningDispatchRegisterContinuityResult AiPlanningDispatchRegisterContinuityResult, *PAiPlanningDispatchRegisterContinuityResult;

typedef uint AiPlanningPhaseIndex;

struct AiPlanningDispatchRegisterContinuityResult {
    FactionRuntimeIndex preservedEdxFactionIndex; 
    AiPlanningPhaseIndex preservedEcxPlanningPhase; 
};

typedef struct GridPathMarkedRegionCellRegisterResult GridPathMarkedRegionCellRegisterResult, *PGridPathMarkedRegionCellRegisterResult;

struct GridPathMarkedRegionCellRegisterResult {
    FieldGridCellCoordinate selectedColumn; 
    FieldGridCellCoordinate selectedRow; 
};

typedef qword AiCandidateScoreEaxPreservedEdxCarrier64;

typedef struct AiPlanningGridScratchEdxContinuityResult AiPlanningGridScratchEdxContinuityResult, *PAiPlanningGridScratchEdxContinuityResult;

struct AiPlanningGridScratchEdxContinuityResult {
    dword preservedEdxGridScratchRowStrideBytes; 
};

typedef struct GridInfluenceDiagonalScanRegisterResult GridInfluenceDiagonalScanRegisterResult, *PGridInfluenceDiagonalScanRegisterResult;

struct GridInfluenceDiagonalScanRegisterResult {
    FieldGridCellCoordinate preservedEaxGridMetric; 
    FieldGridCellCoordinate preservedEcxGridMetric; 
    FieldGridCellCoordinate preservedEdxGridMetric; 
    dword processedCellCount; 
};

typedef enum TerrainRelaxationMode {
    TERRAIN_RELAXATION_SIGN_GATED=0,
    TERRAIN_RELAXATION_UNGATED_LAND_TOOL=1
} TerrainRelaxationMode;

typedef enum EngineDriveTypeCode {
    ENGINE_DRIVE_REMOVABLE=40,
    ENGINE_DRIVE_OTHER=41,
    ENGINE_DRIVE_REMOTE=42,
    ENGINE_DRIVE_CDROM=43
} EngineDriveTypeCode;

typedef enum FrontendCommandSyncPendingState {
    FRONTEND_COMMAND_SYNC_CLEAR=0,
    FRONTEND_COMMAND_SYNC_PENDING=1
} FrontendCommandSyncPendingState;

typedef enum InGameCameraCommandKeyCode {
    EncodedDigit1=196657,
    EncodedDigit2=196658,
    EncodedDigit3=196659,
    EncodedDigit4=196660,
    EncodedDigit5=196661,
    EncodedDigit6=196662,
    EncodedDigit7=196663,
    EncodedLowercaseC=196707,
    EncodedLowercaseS=196723
} InGameCameraCommandKeyCode;

typedef enum DirectSoundBufferCaps {
    DSBCAPS_CTRLPAN=64,
    DSBCAPS_CTRLVOLUME=128
} DirectSoundBufferCaps;

typedef enum ArenaBlockStateMagic {
    ARENA_BLOCK_ALLOCATED=1515870810,
    ARENA_BLOCK_FREE=2779096485
} ArenaBlockStateMagic;

typedef enum FileSystemOpenFlags {
    FILESYSTEM_OPEN_CREATE_OR_TRUNCATE=1,
    FILESYSTEM_OPEN_EXCLUSIVE_SHARE=2,
    FILESYSTEM_OPEN_EXISTING_OR_CREATE=4,
    FILESYSTEM_OPEN_WRITE_ACCESS=8
} FileSystemOpenFlags;

typedef enum UiTextEditStateFlags {
    UI_TEXT_EDIT_VALUE_VALID=1,
    UI_TEXT_EDIT_DRAW_FRAMED_CHROME=8,
    UI_TEXT_EDIT_CARET_VISIBLE_PHASE=16,
    UI_TEXT_EDIT_OVERWRITE_MODE=32,
    UI_TEXT_EDIT_POINTER_SELECTION_ACTIVE=64,
    UI_TEXT_EDIT_READ_ONLY=128,
    UI_TEXT_EDIT_DRAW_TILED_INTERIOR=256,
    UI_TEXT_EDIT_ACTION_ON_ENTER_ONLY=512,
    UI_TEXT_EDIT_PLAY_INTERACTION_SOUND=1024
} UiTextEditStateFlags;

typedef enum UiScrollableStateFlags {
    UI_SCROLL_HORIZONTAL_BAR_AT_TOP=1,
    UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM=2,
    UI_SCROLL_VERTICAL_BAR_AT_LEFT=4,
    UI_SCROLL_VERTICAL_BAR_AT_RIGHT=8,
    UI_SCROLL_PRIMARY_INTERACTION_ACTIVE=8192,
    UI_SCROLL_HORIZONTAL_DECREMENT_ACTIVE=65536,
    UI_SCROLL_HORIZONTAL_TRACK_BEFORE_THUMB_ACTIVE=131072,
    UI_SCROLL_HORIZONTAL_THUMB_ACTIVE=262144,
    UI_SCROLL_HORIZONTAL_TRACK_AFTER_THUMB_ACTIVE=524288,
    UI_SCROLL_HORIZONTAL_INCREMENT_ACTIVE=1048576,
    UI_SCROLL_VERTICAL_DECREMENT_ACTIVE=16777216,
    UI_SCROLL_VERTICAL_TRACK_BEFORE_THUMB_ACTIVE=33554432,
    UI_SCROLL_VERTICAL_THUMB_ACTIVE=67108864,
    UI_SCROLL_VERTICAL_TRACK_AFTER_THUMB_ACTIVE=134217728,
    UI_SCROLL_VERTICAL_INCREMENT_ACTIVE=268435456
} UiScrollableStateFlags;

typedef enum TechnologyCategory {
    TECHNOLOGY_CATEGORY_A=0,
    TECHNOLOGY_CATEGORY_B=1,
    TECHNOLOGY_CATEGORY_C=2,
    TECHNOLOGY_CATEGORY_D=3
} TechnologyCategory;

typedef enum PersistentToggleState {
    PERSISTENT_TOGGLE_DISABLED=0,
    PERSISTENT_TOGGLE_ENABLED=1
} PersistentToggleState;

typedef enum UiNodeFlags {
    UI_NODE_PREFERRED_FOCUS_TARGET=2,
    UI_NODE_HAS_KEYBOARD_FOCUS=4,
    UI_NODE_SUPPRESSED=8,
    UI_NODE_FALLBACK_FOCUS_TARGET=32,
    UI_NODE_ALLOW_CHILD_HIT_TEST_OUTSIDE_BOUNDS=64,
    UI_NODE_REPEAT_OR_DOUBLE_CLICK=128,
    UI_NODE_TOOLTIP_ELIGIBLE=256,
    UI_NODE_TOOLTIP_REFERENCE_DIRECT_UTF16=512
} UiNodeFlags;

typedef enum UiTimedListStateFlags {
    UI_TIMED_LIST_ACTION_DELAY_PENDING=2
} UiTimedListStateFlags;

typedef enum UiSelectableStateFlags {
    UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE=1,
    UI_SELECTABLE_SELECTED_OR_CHECKED=2,
    UI_SELECTABLE_TOGGLE_ON_ACTIVATION=16,
    UI_SELECTABLE_IGNORE_FOCUSED_SPACE_ACTIVATION=8192
} UiSelectableStateFlags;

typedef enum InGameTargetingObservedActionState {
    INGAME_TARGETING_OBSERVED_IDLE=0,
    INGAME_TARGETING_OBSERVED_ADVANCE_OR_RESOLVE=7,
    INGAME_TARGETING_OBSERVED_CANCEL_AND_RESTORE=27
} InGameTargetingObservedActionState;

typedef enum UiTextListStateFlags {
    UI_TEXT_LIST_TYPE_SEARCH_ENABLED=1,
    UI_TEXT_LIST_DEFERRED_ACTION_PENDING=2,
    UI_TEXT_LIST_SELECTION_CONFIRMED=4,
    UI_TEXT_LIST_PLAY_SELECTION_SOUND=8
} UiTextListStateFlags;

typedef enum GraphicsCursorButtonState {
    NONE=0,
    LEFT=1,
    MIDDLE=2,
    LEFT_MIDDLE=3,
    RIGHT=4,
    LEFT_RIGHT=5,
    MIDDLE_RIGHT=6,
    LEFT_MIDDLE_RIGHT=7
} GraphicsCursorButtonState;

typedef enum FileSystemDos83ValidationFlags {
    FILESYSTEM_DOS83_ALLOW_WILDCARDS=1,
    FILESYSTEM_DOS83_COMPONENT_ONLY=2,
    FILESYSTEM_DOS83_ALLOW_PATH_CONTINUATION=4
} FileSystemDos83ValidationFlags;

typedef enum FileSystemEnumerationMode {
    FILESYSTEM_ENUMERATE_FILES=0,
    FILESYSTEM_ENUMERATE_VOLUME_LABEL=1,
    FILESYSTEM_ENUMERATE_DIRECTORIES=2
} FileSystemEnumerationMode;

typedef enum FileSystemSeekOrigin {
    FILESYSTEM_SEEK_BEGIN=0,
    FILESYSTEM_SEEK_CURRENT=1,
    FILESYSTEM_SEEK_END=2
} FileSystemSeekOrigin;

typedef enum GraphicsCursorEventType {
    MOTION_OR_WHEEL=0,
    LEFT_PRESS=1,
    MIDDLE_PRESS=2,
    RIGHT_PRESS=3,
    LEFT_RELEASE=5,
    MIDDLE_RELEASE=6,
    RIGHT_RELEASE=7
} GraphicsCursorEventType;

typedef enum FactionRuntimeLifecycleObservedState {
    FACTION_RUNTIME_LIFECYCLE_ACTIVE=1,
    FACTION_RUNTIME_LIFECYCLE_ENDING_PENDING=2,
    FACTION_RUNTIME_LIFECYCLE_ENDED_OR_TRANSITIONED=3
} FactionRuntimeLifecycleObservedState;

typedef enum WinSockIpv4AddressLength {
    WINSOCK_IPV4_ADDRESS_BYTES=4
} WinSockIpv4AddressLength;

typedef enum RuntimeRegistrationRecordFlags {
    RUNTIME_REGISTRATION_RECORD_ALLOCATED=1073741824
} RuntimeRegistrationRecordFlags;

typedef enum PckCompressionMethod {
    PCK_COMPRESSION_HUFFMAN_RLE=0,
    PCK_COMPRESSION_STORED=1,
    PCK_COMPRESSION_FIELD_GRID=2
} PckCompressionMethod;

typedef enum UiRequiredTextEditStateFlags {
    UI_REQUIRED_TEXT_VALUE_VALID=1,
    UI_REQUIRED_TEXT_DRAW_FRAMED_CHROME=8,
    UI_REQUIRED_TEXT_CARET_VISIBLE_PHASE=16,
    UI_REQUIRED_TEXT_OVERWRITE_MODE=32,
    UI_REQUIRED_TEXT_POINTER_SELECTION_ACTIVE=64,
    UI_REQUIRED_TEXT_READ_ONLY=128,
    UI_REQUIRED_TEXT_DRAW_TILED_INTERIOR=256,
    UI_REQUIRED_TEXT_ACTION_ON_ENTER_ONLY=512,
    UI_REQUIRED_TEXT_PLAY_INTERACTION_SOUND=1024,
    UI_REQUIRED_TEXT_ESCAPE_CLEARS_AND_QUEUES_ACTION=2048
} UiRequiredTextEditStateFlags;

typedef enum FrontendSnapshotTransferFlags {
    FRONTEND_SNAPSHOT_SOURCE_AVAILABLE=1,
    FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE=2,
    FRONTEND_SNAPSHOT_HOST_PUBLICATION_READY=4
} FrontendSnapshotTransferFlags;

typedef enum PersistentMouseLinkPanelOptionFlags {
    PERSISTENT_MOUSE_LINK_ROTATION_ZOOM=1,
    PERSISTENT_MOUSE_LINK_ROTATION_TILT=2,
    PERSISTENT_UI_HIDE_PANEL=4
} PersistentMouseLinkPanelOptionFlags;

typedef enum PckAssetTypeTag {
    PCK_ASSET_TYPE_TEC=6514036,
    PCK_ASSET_TYPE_FNC=6516326,
    PCK_ASSET_TYPE_FLD=6581350,
    PCK_ASSET_TYPE_EFF=6710885,
    PCK_ASSET_TYPE_PAL=7102832,
    PCK_ASSET_TYPE_MDL=7103597,
    PCK_ASSET_TYPE_SAM=7168371,
    PCK_ASSET_TYPE_FLM=7171174,
    PCK_ASSET_TYPE_ROM=7171954,
    PCK_ASSET_TYPE_ARM=7172705,
    PCK_ASSET_TYPE_CGN=7235427,
    PCK_ASSET_TYPE_SPR=7499891,
    PCK_ASSET_TYPE_STR=7500915,
    PCK_ASSET_TYPE_SHT=7628915,
    PCK_ASSET_TYPE_LEV=7759212,
    PCK_ASSET_TYPE_GFX=7890535
} PckAssetTypeTag;

typedef enum InGameNotificationInteractionState {
    NONE=0,
    PAYLOAD_ACTIVE=7
} InGameNotificationInteractionState;

typedef enum GraphicsPaletteTextureAssetMagic {
    GRAPHICS_PALETTE_TEXTURE_MAGIC_GFX=7890535
} GraphicsPaletteTextureAssetMagic;

typedef enum FileSystemCreateDirectoryFlags {
    FILESYSTEM_CREATE_DIRECTORY_RECURSIVE=1
} FileSystemCreateDirectoryFlags;

typedef enum UiTransferJoinAvailability {
    UI_TRANSFER_JOIN_UNAVAILABLE=0,
    UI_TRANSFER_JOIN_AVAILABLE=4294967295
} UiTransferJoinAvailability;

typedef enum WaveFormatTag {
    WAVE_FORMAT_PCM=1
} WaveFormatTag;

typedef enum FrontendModelPointerContextFlags {
    FRONTEND_MODEL_POINTER_CONTEXT_SUPPRESS_BUILTIN_ACTION_RESOLUTION=16,
    FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_SECONDARY_CALLBACK=32,
    FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_BUILTIN_ACTION_RESOLUTION=64,
    FRONTEND_MODEL_POINTER_CONTEXT_OBSERVED_ACTION_BRANCH_00000100=256,
    FRONTEND_MODEL_POINTER_CONTEXT_OBSERVED_ACTION_BRANCH_00000200=512,
    FRONTEND_MODEL_POINTER_CONTEXT_COMPARE_HITS_BY_METRIC_ONLY=4096,
    FRONTEND_MODEL_POINTER_CONTEXT_OBSERVED_ACTION_BRANCH_00008000=32768,
    FRONTEND_MODEL_POINTER_CONTEXT_ALLOW_MODEL_WITHOUT_RUNTIME_FLAG_20=4194304,
    FRONTEND_MODEL_POINTER_CONTEXT_OBSERVED_BUTTON_BRANCH_04000000=67108864,
    FRONTEND_MODEL_POINTER_CONTEXT_OBSERVED_CODE_OVERRIDE_40000000=1073741824,
    FRONTEND_MODEL_POINTER_CONTEXT_OBSERVED_CODE_OVERRIDE_80000000=2147483648
} FrontendModelPointerContextFlags;

typedef enum PckTechnologyIdCatalog {
    TEC_000_BASIC_TECHNOLOGY=0,
    TEC_001_ARMS_FACTORIES=1,
    TEC_002_WEASEL=2,
    TEC_003_WEASEL_ARMOUR_PLUS_25_PERCENT=3,
    TEC_004_WEASEL_ARMOUR_PLUS_25=4,
    TEC_005_ARV_DINGO=5,
    TEC_006_ARV_DINGO_ARMOUR_PLUS_25_PERCENT=6,
    TEC_007_ARV_DINGO_ARMOUR_PLUS_20_PERCENT=7,
    TEC_008_AV_WOLF=8,
    TEC_009_AV_WOLF_ARMOUR_PLUS_25_PERCENT=9,
    TEC_010_AV_WOLF_ARMOUR_PLUS_20_PERCENT=10,
    TEC_011_PIONEER_VEHICLE=11,
    TEC_012_RESERVED=12,
    TEC_013_RESERVED=13,
    TEC_014_RESERVED=14,
    TEC_015_TANK_BADGER=15,
    TEC_016_TANK_BADGER_ARMOUR_PLUS_25_PERCENT=16,
    TEC_017_TANK_BADGER_ARMOUR_PLUS_20_PERCENT=17,
    TEC_018_PANTHER_TECHNOLOGY=18,
    TEC_019_TANK_PANTHER_ARMOUR_PLUS_25_PERCENT=19,
    TEC_020_TANK_PANTHER_ARMOUR_PLUS_25_PERCENT=20,
    TEC_021_GOLIATH_TECHNOLOGY=21,
    TEC_022_GOLIATH_ARMOUR_PLUS_30_PERCENT=22,
    TEC_023_GOLIATH_ARMOUR_PLUS_20_PERCENT=23,
    TEC_024_RESERVED=24,
    TEC_025_RESERVED=25,
    TEC_026_RESERVED=26,
    TEC_027_RESERVED=27,
    TEC_028_RESERVED=28,
    TEC_029_RESERVED=29,
    TEC_030_EMU_WALKER=30,
    TEC_031_EMU_ARMOUR_PLUS_30_PERCENT=31,
    TEC_032_EMU_ARMOUR_PLUS_25_PERCENT=32,
    TEC_033_ORANG_TECHNOLOGY=33,
    TEC_034_ORANG_ARMOUR_PLUS_25_PERCENT=34,
    TEC_035_ORANG_ARMOUR_PLUS_25_PERCENT=35,
    TEC_036_GORILLA_TECHNOLOGY=36,
    TEC_037_GORILLA_ARMOUR_PLUS_25_PERCENT=37,
    TEC_038_GORILLA_ARMOUR_PLUS_25_PERCENT=38,
    TEC_039_RESERVED=39,
    TEC_040_RESERVED=40,
    TEC_041_RESERVED=41,
    TEC_042_RESERVED=42,
    TEC_043_RESERVED=43,
    TEC_044_RESERVED=44,
    TEC_045_GLD_MINUS_1_MOSQUITO=45,
    TEC_046_GLD_MINUS_1_MOSQUITO_ARMOUR_PLUS_20_PERCENT=46,
    TEC_047_GLD_MINUS_1_MOSQUITO_ARMOUR_PLUS_25_PERCENT=47,
    TEC_048_GLD_MINUS_2_DRAGONFLY=48,
    TEC_049_GLD_MINUS_2_DRAGONFLY_ARMOUR_PLUS_25_PERCENT=49,
    TEC_050_GLD_MINUS_2_DRAGONFLY_ARMOUR_PLUS_20_PERCENT=50,
    TEC_051_GLD_MINUS_3_HORNET=51,
    TEC_052_GLD_MINUS_3_HORNET_ARMOUR_PLUS_25_PERCENT=52,
    TEC_053_GLD_MINUS_3_HORNET_ARMOUR_PLUS_20_PERCENT=53,
    TEC_054_BUMBLEBEE_SERIES=54,
    TEC_055_SPF_BUMBLEBEE_ARMOUR_PLUS_30_PERCENT=55,
    TEC_056_SPF_BUMBLEBEE_ARMOUR_PLUS_20_PERCENT=56,
    TEC_057_RESERVED=57,
    TEC_058_RESERVED=58,
    TEC_059_RESERVED=59,
    TEC_060_DOLPHIN_CLASS=60,
    TEC_061_DOLPHIN_CLASS_ARMOUR_PLUS_25_PERCENT=61,
    TEC_062_DOLPHIN_CLASS_ARMOUR_PLUS_25_PERCENT=62,
    TEC_063_HURRICANE_CLASS=63,
    TEC_064_HURRICANE_CLASS_ARMOUR_PLUS_30_PERCENT=64,
    TEC_065_HURRICANE_CLASS_ARMOUR_PLUS_20_PERCENT=65,
    TEC_066_TYPHOON_CLASS=66,
    TEC_067_TYPHOON_CLASS_ARMOUR_PLUS_25_PERCENT=67,
    TEC_068_TYPHOON_CLASS_ARMOUR_PLUS_25_PERCENT=68,
    TEC_069_LANDING_CRAFT=69,
    TEC_070_RESERVED=70,
    TEC_071_RESERVED=71,
    TEC_072_RESERVED=72,
    TEC_073_RESERVED=73,
    TEC_074_RESERVED=74,
    TEC_075_SP_CROW=75,
    TEC_076_SP_CROW_ARMOUR_PLUS_30_PERCENT=76,
    TEC_077_SP_CROW_ARMOUR_PLUS_20_PERCENT=77,
    TEC_078_B_MINUS_12_HAWK=78,
    TEC_079_B_MINUS_12_HAWK_ARMOUR_PLUS_25_PERCENT=79,
    TEC_080_B_MINUS_12_HAWK_ARMOUR_PLUS_25_PERCENT=80,
    TEC_081_B_MINUS_18_EAGLE=81,
    TEC_082_B_MINUS_18_EAGLE_ARMOUR_PLUS_30_PERCENT=82,
    TEC_083_B_MINUS_18_EAGLE_ARMOUR_PLUS_20_PERCENT=83,
    TEC_084_RESERVED=84,
    TEC_085_RESERVED=85,
    TEC_086_RESERVED=86,
    TEC_087_RESERVED=87,
    TEC_088_RESERVED=88,
    TEC_089_RESERVED=89,
    TEC_090_LASER=90,
    TEC_091_LASER_RANGE_PLUS_15_PERCENT=91,
    TEC_092_LASER_DAMAGE_PLUS_50_PERCENT=92,
    TEC_093_DUAL_LASER=93,
    TEC_094_DUAL_LASER_RANGE_PLUS_15=94,
    TEC_095_DUAL_LASER_DAMAGE_PLUS_50_PERCENT=95,
    TEC_096_LASER_GUN=96,
    TEC_097_LASER_GUN_RANGE_PLUS_5_PERCENT=97,
    TEC_098_LASER_GUN_DAMAGE_PLUS_50_PERCENT=98,
    TEC_099_LASER_TURRET=99,
    TEC_100_LASER_TURRET_RANGE_PLUS_10_PERCENT=100,
    TEC_101_LASER_TURRET_DAMAGE_PLUS_50_PERCENT=101,
    TEC_102_ION_GUN=102,
    TEC_103_ION_GUN_RANGE_PLUS_20_PERCENT=103,
    TEC_104_ION_GUN_DAMAGE_PLUS_50_PERCENT=104,
    TEC_105_ION_THROWER=105,
    TEC_106_ION_THROWER_RANGE_PLUS_20_PERCENT=106,
    TEC_107_ION_THROWER_DAMAGE_PLUS_50_PERCENT=107,
    TEC_108_ION_CANNON=108,
    TEC_109_ION_CANNON_RANGE_PLUS_15_PERCENT=109,
    TEC_110_ION_CANNON_DAMAGE_PLUS_50_PERCENT=110,
    TEC_111_RESERVED=111,
    TEC_112_RESERVED=112,
    TEC_113_RESERVED=113,
    TEC_114_RESERVED=114,
    TEC_115_RESERVED=115,
    TEC_116_RESERVED=116,
    TEC_117_RESERVED=117,
    TEC_118_RESERVED=118,
    TEC_119_RESERVED=119,
    TEC_120_DOUBLE_MG=120,
    TEC_121_DOUBLE_MG_RANGE_PLUS_25_PERCENT=121,
    TEC_122_DOUBLE_MG_DAMAGE_PLUS_50_PERCENT=122,
    TEC_123_AUTOMATIC_CANNON=123,
    TEC_124_AUTOMATIC_CANNON_RANGE_PLUS_20_PERCENT=124,
    TEC_125_AUTOMATIC_CANNON_DAMAGE_PLUS_50_PERCENT=125,
    TEC_126_CANNON=126,
    TEC_127_CANNON_RANGE_PLUS_15_PERCENT=127,
    TEC_128_CANNON_DAMAGE_PLUS_50_PERCENT=128,
    TEC_129_TWIN_CANNON=129,
    TEC_130_TWIN_CANNON_RANGE_PLUS_15_PERCENT=130,
    TEC_131_TWIN_CANNON_DAMAGE_PLUS_50_PERCENT=131,
    TEC_132_FLAK=132,
    TEC_133_FLAK_RANGE_PLUS_10_PERCENT=133,
    TEC_134_FLAK_DAMAGE_PLUS_50_PERCENT=134,
    TEC_135_ANTI_MINUS_AIRCRAFT_GUNS=135,
    TEC_136_ANTI_MINUS_AIRCRAFT_GUN_RANGE_PLUS_10_PERCENT=136,
    TEC_137_ANTI_MINUS_AIRCRAFT_GUN_DAMAGE_PLUS_50_PERCENT=137,
    TEC_138_MORTAR=138,
    TEC_139_MORTAR_RANGE_PLUS_10=139,
    TEC_140_MORTAR_DAMAGE_PLUS_50_PERCENT=140,
    TEC_141_RESERVED=141,
    TEC_142_RESERVED=142,
    TEC_143_RESERVED=143,
    TEC_144_RESERVED=144,
    TEC_145_RESERVED=145,
    TEC_146_RESERVED=146,
    TEC_147_RESERVED=147,
    TEC_148_RESERVED=148,
    TEC_149_RESERVED=149,
    TEC_150_ROCKET_LAUNCHER=150,
    TEC_151_ROCKET_LAUNCHER_RANGE_PLUS_10_PERCENT=151,
    TEC_152_ROCKET_LAUNCHER_DAMAGE_PLUS_50_PERCENT=152,
    TEC_153_ROCKET_BATTERY=153,
    TEC_154_ROCKET_BATTERY_RANGE_PLUS_10_PERCENT=154,
    TEC_155_ROCKET_BATTERY_DAMAGE_PLUS_50_PERCENT=155,
    TEC_156_ROCKET_FIRING_RAMP=156,
    TEC_157_ROCKET_FIRING_RAMP_RANGE_PLUS_10_PERCENT=157,
    TEC_158_ROCKET_FIRING_RAMP_DAMAGE_PLUS_50_PERCENT=158,
    TEC_159_GUIDED_MISSILES=159,
    TEC_160_GUIDED_MISSILES_RANGE_PLUS_15_PERCENT=160,
    TEC_161_GUIDED_MISSILES_DAMAGE_PLUS_50_PERCENT=161,
    TEC_162_HEAT_MINUS_SEEKING_MISSILES=162,
    TEC_163_HEAT_MINUS_SEEKING_MISSILES_RANGE_PLUS_10_PERCENT=163,
    TEC_164_HEAT_SEEKING_MISSILES_DAMAGE_PLUS_50_PERCENT=164,
    TEC_165_HOWITZER=165,
    TEC_166_HOWITZER_ROTATION_PLUS=166,
    TEC_167_HOWITZER_DAMAGE_PLUS=167,
    TEC_168_ARTILLERY=168,
    TEC_169_ARTILLERY_ROTATION_PLUS=169,
    TEC_170_ARTILLERY_DAMAGE_PLUS=170,
    TEC_171_RESERVED=171,
    TEC_172_RESERVED=172,
    TEC_173_RESERVED=173,
    TEC_174_RESERVED=174,
    TEC_175_RESERVED=175,
    TEC_176_RESERVED=176,
    TEC_177_RESERVED=177,
    TEC_178_RESERVED=178,
    TEC_179_RESERVED=179,
    TEC_180_FLAME_THROWER=180,
    TEC_181_FLAME_THROWER_RANGE_PLUS_10_PERCENT=181,
    TEC_182_FLAME_THROWER_DAMAGE=182,
    TEC_183_RADAR=183,
    TEC_184_RADAR_RANGE_PLUS_10_PERCENT=184,
    TEC_185_RADAR_RANGE_PLUS_10_PERCENT=185,
    TEC_186_LASER_COIL=186,
    TEC_187_LASER_COIL_RANGE_PLUS_10=187,
    TEC_188_LASER_COIL_DAMAGE_PLUS_50_PERCENT=188,
    TEC_189_AR_MINUS_M_SILO=189,
    TEC_190_AR_MINUS_M_SILO_RANGE_PLUS_10_PERCENT=190,
    TEC_191_AR_MINUS_M_SILO_RANGE_PLUS_10_PERCENT=191,
    TEC_192_REPAIR_MODULE=192,
    TEC_193_REPAIR_MODULE_PLUS=193,
    TEC_194_REPAIR_MODULE_PLUS_PLUS=194,
    TEC_195_RESERVED=195,
    TEC_196_RESERVED=196,
    TEC_197_RESERVED=197,
    TEC_198_RESERVED=198,
    TEC_199_RESERVED=199,
    TEC_200_SMALL_CALIBRE_TURRET=200,
    TEC_201_SMALL_CALIBRE_TURRET_ARMOUR_PLUS_25_PERCENT=201,
    TEC_202_SMALL_CALIBRE_TURRET_ARMOUR_PLUS_20_PERCENT=202,
    TEC_203_TURRET=203,
    TEC_204_TURRET_ARMOUR_PLUS_25_PERCENT=204,
    TEC_205_TURRET_ARMOUR_PLUS_20_PERCENT=205,
    TEC_206_LARGE_CALIBRE_TURRET=206,
    TEC_207_LARGE_CALIBRE_TURRET_ARMOUR_PLUS_25_PERCENT=207,
    TEC_208_LARGE_CALIBRE_TURRET_ARMOUR_PLUS_20_PERCENT=208,
    TEC_209_XENITE_MINE=209,
    TEC_210_IMPROVE_XENITE_MINE_1=210,
    TEC_211_IMPROVE_XENITE_MINE_2=211,
    TEC_212_TRITIUM_PUMP=212,
    TEC_213_IMPROVE_TRITIUM_PUMP_1=213,
    TEC_214_IMPROVE_TRITIUM_PUMP_2=214,
    TEC_215_TANK_OBSTACLE=215,
    TEC_216_WALL=216,
    TEC_217_HIGH_WALL=217,
    TEC_218_RESERVED=218,
    TEC_219_RESERVED=219,
    TEC_220_POWER_PLANT=220,
    TEC_221_RESERVED=221,
    TEC_222_RESERVED=222,
    TEC_223_RESERVED=223,
    TEC_224_RESERVED=224,
    TEC_225_RESERVED=225,
    TEC_226_RESERVED=226,
    TEC_227_RESERVED=227,
    TEC_228_RESERVED=228,
    TEC_229_RESERVED=229,
    TEC_230_RESERVED=230,
    TEC_231_RESERVED=231,
    TEC_232_RESERVED=232,
    TEC_233_RESERVED=233,
    TEC_234_RESERVED=234,
    TEC_235_RESERVED=235,
    TEC_236_RESERVED=236,
    TEC_237_RESERVED=237,
    TEC_238_RESERVED=238,
    TEC_239_RESERVED=239,
    TEC_240_RESERVED=240,
    TEC_241_RESERVED=241,
    TEC_242_RESERVED=242,
    TEC_243_RESERVED=243,
    TEC_244_RESERVED=244,
    TEC_245_RESERVED=245,
    TEC_246_RESERVED=246,
    TEC_247_RESERVED=247,
    TEC_248_RESERVED=248,
    TEC_249_RESERVED=249,
    TEC_250_RESERVED=250,
    TEC_251_RESERVED=251,
    TEC_252_RESERVED=252,
    TEC_253_RESERVED=253,
    TEC_254_RESERVED=254,
    TEC_255_RESERVED=255
} PckTechnologyIdCatalog;

typedef enum UiTransferPacketPackedType {
    FRONTEND_PACKET_10000_HANDSHAKE=65536,
    FRONTEND_PACKET_10003_JOIN_ACK=65539,
    FRONTEND_PACKET_10004_SNAPSHOT_REQUEST=65540,
    FRONTEND_PACKET_10006_CAPABILITY_HEARTBEAT=65542,
    FRONTEND_PACKET_10007_PLAYER_REMOVAL=65543,
    FRONTEND_PACKET_10009=65545,
    FRONTEND_PACKET_10011=65553,
    FRONTEND_PACKET_10012=65554,
    FRONTEND_PACKET_10013=65555,
    FRONTEND_PACKET_10021=65569,
    FRONTEND_PACKET_10022=65570,
    FRONTEND_PACKET_10023=65571,
    FRONTEND_PACKET_10031=65585,
    FRONTEND_PACKET_10032=65586,
    FRONTEND_PACKET_10033=65587,
    FRONTEND_PACKET_20002=131074,
    FRONTEND_PACKET_30005_PLAYER_SNAPSHOT=196613,
    FRONTEND_PACKET_40008=262152,
    FRONTEND_PACKET_50001_SESSION_ADVERTISEMENT=327681,
    FRONTEND_PACKET_8000A=524298,
    FRONTEND_PACKET_80030=524336
} UiTransferPacketPackedType;

typedef enum PersistentMapMouseOptionFlags {
    PERSISTENT_MAP_AUTOMATIC_ZOOM_OFF=1,
    PERSISTENT_MAP_AUTOMATIC_ROTATION_OFF=2,
    PERSISTENT_MOUSE_RIGHT_BUTTON_DOES_NOT_SCROLL=4
} PersistentMapMouseOptionFlags;

typedef enum UiCommandActivationStateFlags {
    UI_COMMAND_ACTIVATION_RELATION_RESET_REQUEST_MASK=12,
    UI_COMMAND_ACTIVATION_LOW_INPUT_NIBBLE_MASK=15,
    UI_COMMAND_ACTIVATION_REPEAT_OR_DOUBLE_CLICK=262144,
    UI_COMMAND_ACTIVATION_ALTERNATE_BUTTON=2147483648
} UiCommandActivationStateFlags;

typedef enum InGameNotificationPayloadKind {
    NONE=0,
    ARMY_CREATED=1,
    TECHNOLOGY_UNLOCK_POSITION=2,
    FACTION_IMPACT_ANCHOR=3
} InGameNotificationPayloadKind;

typedef enum UiListStateFlags {
    UI_LIST_DEFERRED_ACTION_PENDING=2,
    UI_LIST_SELECTION_CONFIRMED=4,
    UI_LIST_PLAY_SELECTION_SOUND=8
} UiListStateFlags;

typedef enum SessionNetworkRoleFlags {
    SESSION_NETWORK_ROLE_LOCAL=0,
    SESSION_NETWORK_ROLE_CLIENT=1,
    SESSION_NETWORK_ROLE_HOST=2,
    SESSION_NETWORK_ROLE_NETWORKED_MASK=3
} SessionNetworkRoleFlags;

typedef enum FactionRelationState {
    FACTION_RELATION_MERGE=11
} FactionRelationState;

typedef enum SoftwareFramebufferPixelSize {
    SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_16BIT=2,
    SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_32BIT=4
} SoftwareFramebufferPixelSize;

typedef enum UiTimedListRecordFlags {
    UI_TIMED_LIST_RECORD_OBSERVED_BIT0=1,
    UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL=2,
    UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY=2147483648
} UiTimedListRecordFlags;

typedef enum GraphicsBooleanState {
    GRAPHICS_STATE_DISABLED=0,
    GRAPHICS_STATE_ENABLED=1
} GraphicsBooleanState;

typedef enum RuntimeModelClassPriority {
    RUNTIME_MODEL_CLASS_PRIORITY_LOW=0,
    RUNTIME_MODEL_CLASS_PRIORITY_MEDIUM=1,
    RUNTIME_MODEL_CLASS_PRIORITY_HIGH=2
} RuntimeModelClassPriority;

typedef enum UiNumericTextEditStateFlags {
    UI_NUMERIC_TEXT_VALUE_VALID=1,
    UI_NUMERIC_TEXT_SIGNED_VALUE=2,
    UI_NUMERIC_TEXT_HEXADECIMAL_FORMAT=4,
    UI_NUMERIC_TEXT_DRAW_FRAMED_CHROME=8,
    UI_NUMERIC_TEXT_CARET_VISIBLE_PHASE=16,
    UI_NUMERIC_TEXT_OVERWRITE_MODE=32,
    UI_NUMERIC_TEXT_POINTER_SELECTION_ACTIVE=64,
    UI_NUMERIC_TEXT_READ_ONLY=128,
    UI_NUMERIC_TEXT_DRAW_TILED_INTERIOR=256,
    UI_NUMERIC_TEXT_ACTION_ON_ENTER_ONLY=512,
    UI_NUMERIC_TEXT_PLAY_INTERACTION_SOUND=1024
} UiNumericTextEditStateFlags;

typedef enum LocaleTelephoneCountryCode {
    LOCALE_COUNTRY_GENERIC=0,
    LOCALE_COUNTRY_USA=1,
    LOCALE_COUNTRY_CANADA=2,
    LOCALE_COUNTRY_NETHERLANDS=31,
    LOCALE_COUNTRY_BELGIUM=32,
    LOCALE_COUNTRY_FRANCE=33,
    LOCALE_COUNTRY_SPAIN=34,
    LOCALE_COUNTRY_ITALY=39,
    LOCALE_COUNTRY_GREAT_BRITAIN=44,
    LOCALE_COUNTRY_DENMARK=45,
    LOCALE_COUNTRY_GERMANY=49
} LocaleTelephoneCountryCode;

typedef enum GraphicsPaletteTextureFormatVersion {
    GRAPHICS_PALETTE_TEXTURE_FORMAT_VERSION_1=1
} GraphicsPaletteTextureFormatVersion;

typedef dword TerrainRelaxationPassCount;

typedef dword FrontendRandomSeed;

typedef dword ArmyBuildXeniteCostQ4;

typedef int WorldMotionSplineKeyframeCount;

typedef int ArmyGraphicsAssetAddress32;

typedef int UiPointerWheelDelta;

typedef int CubicSplineSegmentIndex;

typedef int FrontendFactionAssignmentIndex;

typedef int RichTextCommandOrdinal;

typedef dword GraphicsPackedPixelMask;

typedef dword AudioBitsPerSampleStack32;

typedef int SoftwareColorTransformQ16;

typedef int SelectionPanelCellIndex;

typedef dword FactionRuntimeFlags;

typedef qword FixedLengthAngleEdxEax8;

typedef dword UiTransferXorChecksum;

typedef uint RichTextMarkupCapacityCodeUnits;

typedef dword RomRegistrySlotValue;

typedef int GraphicsSceneExtentFixed;

typedef int UiStopMessageCode;

typedef dword FrontendReadyOrWaitState;

typedef qword UiGridDimensionsEdxEax8;

typedef dword TextResourceStringCount;

typedef int RichTextCommandSelector;

typedef int ModelLocalPointRecordAddress32;

typedef dword FrontendConsensusValue;

typedef dword GraphicsCursorClockValue;

typedef int WorldMotionSplineValueQ12;

typedef int DepthIntervalRadius32;

typedef uint FrontendSelectionTransferModeFlags;

typedef dword UiTextCodeUnitIndex;

typedef dword UiListRowIndex;

typedef word UiTechnologyValueTextBuffer16Utf16[16];

typedef dword PersistentSettingsDwordValue;

typedef dword MovieFrameIndex;

typedef dword NetworkIpv4AddressNetworkOrder;

typedef dword InGameNotificationPriority;

typedef dword UiTextCodeUnitCount;

typedef dword LevelCampaignAssociationIndex;

typedef dword GraphicsDiagnosticCounter;

typedef dword LevelAssetPointerArrayByteCount;

typedef dword PckDecodedByteCount;

typedef dword FileSystemOutputCapacityBytes;

typedef int GraphicsPrimitiveBackendCoordinate;

typedef dword MovieWorkerActiveFlag;

typedef word Win32CalendarYear16;

typedef dword GraphicsCursorFrameIndex;

typedef dword LocaleCalendarDayStack32;

typedef dword FieldCellFlagMask;

typedef dword PckEntryCount;

typedef dword RomRecordByteSize;

typedef dword PersistentDisplayAdapterIndex;

typedef qword FixedPlanarPointEdxEax8;

typedef dword GraphicsCursorFrameCount;

typedef dword SprAttachmentSelectorOrdinal;

typedef dword ArmyPlacementDispatchArg2;

typedef dword ArmyPlacementDispatchArg3;

typedef dword ArmyPlacementDispatchArg0;

typedef dword PckStoredByteCount;

typedef dword MdlNodeFlags;

typedef dword GraphicsAssetRelativeByteOffset;

typedef dword UiCommandModeIndex;

typedef dword GraphicsProjectionShift;

typedef dword InGameNotificationMovieId;

typedef dword PckOutputCapacityBytes;

typedef dword MoviePaletteBankCount;

typedef int UiNumericValue32;

typedef dword FileSystemFilePosition;

typedef struct InGamePersistentSettingsPage3508 InGamePersistentSettingsPage3508, *PInGamePersistentSettingsPage3508;

typedef struct InGamePersistentSettingsPage3508 *InGamePersistentSettingsPageSourceNodePtr;

typedef struct UiPageStackControl UiPageStackControl, *PUiPageStackControl;

typedef struct UiNodeBase UiNodeBase, *PUiNodeBase;

typedef struct UiSelectableControl UiSelectableControl, *PUiSelectableControl;

typedef struct UiNumericTextControl UiNumericTextControl, *PUiNumericTextControl;

typedef dword UiPageCount;

typedef struct UiNodeVtable UiNodeVtable, *PUiNodeVtable;

typedef int UiSerializedRelocationDelta;

typedef int UiPixelCoordinate;

typedef dword UiKeyboardStateMask;

typedef dword UiKeyboardEventCode;

typedef dword UiNodeFlagMask;

typedef int UiActionId;

typedef dword UiAnchorFractionQ31;

typedef dword UiPixelOffset;

typedef struct DirectSoundVoiceSet DirectSoundVoiceSet, *PDirectSoundVoiceSet;

typedef struct IDirectSoundBuffer IDirectSoundBuffer, *PIDirectSoundBuffer;

typedef struct IDirectSoundBuffer_Vtbl IDirectSoundBuffer_Vtbl, *PIDirectSoundBuffer_Vtbl;

typedef struct WAVEFORMATEX WAVEFORMATEX, *PWAVEFORMATEX;

typedef struct IDirectSound IDirectSound, *PIDirectSound;

typedef struct DSBUFFERDESC_DX6 DSBUFFERDESC_DX6, *PDSBUFFERDESC_DX6;

typedef word AudioChannelCount;

typedef dword AudioSampleRateHz;

typedef dword AudioByteRate;

typedef word AudioBlockAlignBytes;

typedef word AudioBitsPerSample;

typedef word WaveFormatExtraByteCount;

typedef struct IDirectSound_Vtbl IDirectSound_Vtbl, *PIDirectSound_Vtbl;

typedef dword DirectSoundBufferDescriptionByteSize;

typedef dword AudioBufferByteCount;

struct IDirectSound {
    struct IDirectSound_Vtbl *lpVtbl;
};

struct IDirectSoundBuffer {
    struct IDirectSoundBuffer_Vtbl *lpVtbl;
};

struct DirectSoundVoiceSet {
    struct IDirectSoundBuffer *voices[8]; 
};

struct UiNodeBase {
    struct UiNodeBase *nextSibling; 
    struct UiNodeBase *firstChild; 
    struct UiNodeBase *parent; 
    struct UiNodeVtable *vtable; 
    sdword left; 
    sdword top; 
    sdword right; 
    sdword bottom; 
    sdword leftOffset; 
    sdword topOffset; 
    sdword rightOffset; 
    sdword bottomOffset; 
    UiAnchorFractionQ31 leftAnchorQ31; 
    UiAnchorFractionQ31 topAnchorQ31; 
    UiAnchorFractionQ31 rightAnchorQ31; 
    UiAnchorFractionQ31 bottomAnchorQ31; 
    sdword layoutWidth; 
    sdword layoutHeight; 
    enum UiNodeFlags nodeFlags; 
};

struct UiNumericTextControl {
    struct UiNodeBase base; 
    enum UiNumericTextEditStateFlags editStateFlags; 
    UiActionId actionId; 
    UiPixelOffset horizontalScrollPixels; 
    UiNumericValue32 currentValue; 
    UiTextCodeUnitIndex cursorIndex; 
    UiTextCodeUnitIndex selectionStart; 
    UiTextCodeUnitIndex selectionEnd; 
    struct DirectSoundVoiceSet *activationSound; 
    word textBuffer[16]; 
    UiNumericValue32 minimumValue; 
    UiNumericValue32 maximumValue; 
};

struct IDirectSound_Vtbl {
    TH_LEGACY_HRESULT (*QueryInterface)(struct IDirectSound *, struct TH_LEGACY_GUID *, TH_LEGACY_LPVOID *);
    TH_LEGACY_ULONG (*AddRef)(struct IDirectSound *);
    TH_LEGACY_ULONG (*Release)(struct IDirectSound *);
    TH_LEGACY_HRESULT (*CreateSoundBuffer)(struct IDirectSound *, struct DSBUFFERDESC_DX6 *, struct IDirectSoundBuffer **, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*GetCaps)(struct IDirectSound *, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*DuplicateSoundBuffer)(struct IDirectSound *, struct IDirectSoundBuffer *, struct IDirectSoundBuffer **);
    TH_LEGACY_HRESULT (*SetCooperativeLevel)(struct IDirectSound *, TH_LEGACY_HWND, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*Compact)(struct IDirectSound *);
    TH_LEGACY_HRESULT (*GetSpeakerConfig)(struct IDirectSound *, TH_LEGACY_DWORD *);
    TH_LEGACY_HRESULT (*SetSpeakerConfig)(struct IDirectSound *, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*Initialize)(struct IDirectSound *, struct TH_LEGACY_GUID *);
};

struct UiSelectableControl {
    struct UiNodeBase base; 
    enum UiSelectableStateFlags stateFlags; 
    UiActionId actionId; 
};

struct UiNodeVtable {
    void (*relocate)(UiSerializedRelocationDelta, struct UiNodeBase *); 
    void *method04; 
    void (*drawClipped)(UiPixelCoordinate, UiPixelCoordinate, UiPixelCoordinate, UiPixelCoordinate, struct UiNodeBase *); 
    void (*layout)(struct UiNodeBase *); 
    void (*nonRightPress)(UiPointerWheelDelta, UiPixelCoordinate, UiPixelCoordinate, struct UiNodeBase *); 
    void (*nonRightRelease)(UiPointerWheelDelta, UiPixelCoordinate, UiPixelCoordinate, struct UiNodeBase *); 
    void (*rightPress)(UiPointerWheelDelta, UiPixelCoordinate, UiPixelCoordinate, struct UiNodeBase *); 
    void (*rightRelease)(UiPointerWheelDelta, UiPixelCoordinate, UiPixelCoordinate, struct UiNodeBase *); 
    void (*nonRightDrag)(UiPointerWheelDelta, UiPixelCoordinate, UiPixelCoordinate, struct UiNodeBase *); 
    void (*rightDrag)(UiPointerWheelDelta, UiPixelCoordinate, UiPixelCoordinate, struct UiNodeBase *); 
    GraphicsCursorFrameIndex (*pointerMove)(UiPixelCoordinate, UiPixelCoordinate, struct UiNodeBase *); 
    UiNodeBase * (*hitTest)(UiPixelCoordinate, UiPixelCoordinate, struct UiNodeBase *); 
    void (*keyboardEventCf)(UiKeyboardStateMask, UiKeyboardEventCode, struct UiNodeBase *); 
    void (*applyFlags)(UiNodeFlagMask, UiNodeFlagMask, struct UiNodeBase *); 
    void (*suppressActionId)(UiActionId, struct UiNodeBase *); 
    void (*unsuppressActionId)(UiActionId, struct UiNodeBase *); 
    void (*tick)(struct UiNodeBase *); 
    void (*pointerWheel)(UiPointerWheelDelta, UiPixelCoordinate, UiPixelCoordinate, struct UiNodeBase *); 
};

struct WAVEFORMATEX {
    enum WaveFormatTag wFormatTag; 
    AudioChannelCount nChannels; 
    AudioSampleRateHz nSamplesPerSec; 
    AudioByteRate nAvgBytesPerSec; 
    AudioBlockAlignBytes nBlockAlign; 
    AudioBitsPerSample wBitsPerSample; 
    WaveFormatExtraByteCount cbSize; 
};

struct IDirectSoundBuffer_Vtbl {
    TH_LEGACY_HRESULT (*QueryInterface)(struct IDirectSoundBuffer *, struct TH_LEGACY_GUID *, TH_LEGACY_LPVOID *);
    TH_LEGACY_ULONG (*AddRef)(struct IDirectSoundBuffer *);
    TH_LEGACY_ULONG (*Release)(struct IDirectSoundBuffer *);
    TH_LEGACY_HRESULT (*GetCaps)(struct IDirectSoundBuffer *, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*GetCurrentPosition)(struct IDirectSoundBuffer *, TH_LEGACY_DWORD *, TH_LEGACY_DWORD *);
    TH_LEGACY_HRESULT (*GetFormat)(struct IDirectSoundBuffer *, struct WAVEFORMATEX *, TH_LEGACY_DWORD, TH_LEGACY_DWORD *);
    TH_LEGACY_HRESULT (*GetVolume)(struct IDirectSoundBuffer *, TH_LEGACY_LONG *);
    TH_LEGACY_HRESULT (*GetPan)(struct IDirectSoundBuffer *, TH_LEGACY_LONG *);
    TH_LEGACY_HRESULT (*GetFrequency)(struct IDirectSoundBuffer *, TH_LEGACY_DWORD *);
    TH_LEGACY_HRESULT (*GetStatus)(struct IDirectSoundBuffer *, TH_LEGACY_DWORD *);
    TH_LEGACY_HRESULT (*Initialize)(struct IDirectSoundBuffer *, struct IDirectSound *, struct DSBUFFERDESC_DX6 *);
    TH_LEGACY_HRESULT (*Lock)(struct IDirectSoundBuffer *, TH_LEGACY_DWORD, TH_LEGACY_DWORD, TH_LEGACY_LPVOID *, TH_LEGACY_DWORD *, TH_LEGACY_LPVOID *, TH_LEGACY_DWORD *, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*Play)(struct IDirectSoundBuffer *, TH_LEGACY_DWORD, TH_LEGACY_DWORD, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*SetCurrentPosition)(struct IDirectSoundBuffer *, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*SetFormat)(struct IDirectSoundBuffer *, struct WAVEFORMATEX *);
    TH_LEGACY_HRESULT (*SetVolume)(struct IDirectSoundBuffer *, TH_LEGACY_LONG);
    TH_LEGACY_HRESULT (*SetPan)(struct IDirectSoundBuffer *, TH_LEGACY_LONG);
    TH_LEGACY_HRESULT (*SetFrequency)(struct IDirectSoundBuffer *, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*Stop)(struct IDirectSoundBuffer *);
    TH_LEGACY_HRESULT (*Unlock)(struct IDirectSoundBuffer *, TH_LEGACY_LPVOID, TH_LEGACY_DWORD, TH_LEGACY_LPVOID, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*Restore)(struct IDirectSoundBuffer *);
};

struct DSBUFFERDESC_DX6 {
    DirectSoundBufferDescriptionByteSize dwSize; 
    enum DirectSoundBufferCaps dwFlags; 
    AudioBufferByteCount dwBufferBytes; 
    TH_LEGACY_DWORD dwReserved;
    struct WAVEFORMATEX *lpwfxFormat;
};

struct UiPageStackControl {
    struct UiNodeBase base; 
    UiPageCount pageCount; 
    struct UiNodeBase *pages; 
};

struct InGamePersistentSettingsPage3508 {
    struct UiPageStackControl settingsPageStack; 
    byte reserved0054_1A9F[6732]; 
    struct UiNodeBase sourceNode; 
    byte reserved1AEC_2DCB[4832]; 
    struct UiSelectableControl musicEnabledControl; 
    byte reserved2E20_2E2B[12]; 
    struct UiSelectableControl soundEffectsEnabledControl; 
    byte reserved2E80_2E8B[12]; 
    struct UiSelectableControl reverseStereoControl; 
    byte reserved2EE0_2FFF[288]; 
    struct UiNumericTextControl soundEffectsGainControl; 
    byte reserved3094_317B[232]; 
    struct UiNumericTextControl movieDefaultAudioGainControl; 
    byte reserved3210_32F7[232]; 
    struct UiNumericTextControl musicGainControl; 
    byte reserved338C_3473[232]; 
    struct UiNumericTextControl movieAlternateAudioGainControl; 
};

typedef dword DisplayModeHookArgument0;

typedef word Win32CalendarMonth16;

typedef dword DisplayModeHookArgument1;

typedef dword SerializedRelativeByteOffset;

typedef dword PersistentDisplayDimensionPixels;

typedef dword TerrainExploredPercent;

typedef int WorldMotionSplineTimeQ12;

typedef dword MovieAudioGainQ15;

typedef dword PersistentShadingSubresourceCount;

typedef dword UiPixelExtent;

typedef int WorldRuntimeExtendedMapControlAddress32;

typedef dword RomRecordId;

typedef dword MovieSubresourceCount;

typedef int FrontendConsensusSourceAddress32;

typedef dword ModelRuntimePoolRelativeOffset;

typedef dword GraphicsViewAngle16;

typedef dword GraphicsDisplayModeCount;

typedef int SelectionPanelSegmentCount;

typedef dword GraphicsSubresourceEndIndex;

typedef dword FactionActiveMask;

typedef dword MovieSubresourceTableOffset;

typedef dword FactionProgressAmountQ4;

typedef dword NetworkPlayerCount;

typedef int PriorityPairHeapCount;

typedef int PersistentModelLodDepthQ8;

typedef struct InGameCatalogDetailPage32C InGameCatalogDetailPage32C, *PInGameCatalogDetailPage32C;

typedef struct InGameCatalogDetailPage32C *InGameCatalogDetailPageCatalogListPtr;

typedef struct UiPointerListControl UiPointerListControl, *PUiPointerListControl;

typedef dword UiTextResourceId;

typedef dword UiListRowCount;

struct UiPointerListControl {
    struct UiNodeBase base; 
    enum UiListStateFlags listStateFlags; 
    void **rowSlots; 
    UiListRowCount rowCount; 
    UiPixelExtent rowHeight; 
    UiActionId actionId; 
    void **selectedRowSlot; 
};

struct InGameCatalogDetailPage32C {
    struct UiNodeBase pageRoot; 
    byte reserved004C_014F[260]; 
    struct UiPointerListControl catalogList; 
    byte reserved01B4_027B[200]; 
    UiTextResourceId activeDetailTextResourceId; 
    byte reserved0280_0283[4]; 
    struct UiPageStackControl detailPageStack; 
    byte reserved02D8_02DF[8]; 
    struct UiNodeBase action1210Control; 
};

typedef qword FixedEulerPairEdxEax8;

typedef dword CubicSplineEquationCount;

typedef int GraphicsScreenCoordinate;

typedef word Win32CalendarDay16;

typedef dword KeyboardVirtualKeyCode;

typedef int GraphicsPlaneNormalFixed;

typedef dword UiFrameRefreshCountdownFrames;

typedef dword UInt64Half32;

typedef word SoftwareColorLaneUnsigned16;

typedef word Win32Millisecond16;

typedef dword UiTransferRemainingByteCount;

typedef dword FrontendMessageValueA;

typedef dword FrontendMessageValueB;

typedef dword FrontendMessageValueC;

typedef dword WideNumberFormatCodeUnitCount;

typedef dword TextOutputCapacityBytes;

typedef dword PaletteBankIndex;

typedef dword RomVisibilityFrontendValue;

typedef dword PersistentShadingTextureDimension;

typedef dword PlayerState8090Value;

typedef dword Win32MessageId32;

typedef int PlayerOrFactionRuntimeId32;

typedef dword MovieOpenFlags;

typedef dword LevelAssetRelativeByteOffset;

typedef dword ScenarioCatalogByteOffset;

typedef dword GraphicsPrimitiveDispatchFlags;

typedef dword PackedFramebufferPixel;

typedef dword PackedFileExtensionCode32;

typedef dword TextResourceLocaleBlockByteSize;

typedef dword DepthBinMask32;

typedef dword LocaleClockHourStack32;

typedef dword InGameSimulationStepBatchTicks;

typedef dword FrontendReadyFlagMask;

typedef dword SelectionMarkerCoordinateValue32;

typedef dword LocaleCountryCode;

typedef int GraphicsPrimitiveTextureCoordinateFixed;

typedef dword FactionNotificationCodeBase;

typedef dword PlayerState8094Value;

typedef dword UiTransferMailboxTickCounter;

typedef int Win32CursorCoordinate32;

typedef dword LevelPackedHeadingPitch;

typedef int FixedMathVectorComponent32;

typedef dword UiTransferSequenceToken;

typedef int FrontendPlayerRuntimeId;

typedef int Win32LParam32;

typedef dword GraphicsPrimitiveQueueCapacity;

typedef dword CubicSplineMatrixIndex;

typedef dword MovieSharedStreamHandleFlag;

typedef int ModelMeshGroupAddress32;

typedef qword GraphicsLogicalDimensionsEdxEax8;

typedef dword GraphicsAssetAllocationByteSize;

typedef dword WinMmTimerPeriodMilliseconds;

typedef dword FactionArmyContributionValue;

typedef qword FixedLengthElevationEdxEax8;

typedef dword GridPathUnreachableReferenceColumn32;

typedef dword WideNumberIntegerDigitLimit;

typedef dword GraphicsAssetSubresourceCount;

typedef dword ArmySelectionDetailTemplateVariantIndex;

typedef dword UiPackedTextStyle;

typedef int InGameSaveGamePageControlAddress32;

typedef dword ArmyPlacementDispatchArg7;

typedef dword PackedUiCommandAndPlayerId;

typedef Q12 ArmyPlacementClearancePaddingQ12;

typedef dword MovieFrameCount;

typedef int NetworkTimeoutMicroseconds;

typedef dword FrontendRoleStateFlags;

typedef word Win32Hour16;

typedef int WideNumberSignedValue32;

typedef dword ArmyPlacementMode;

typedef dword ArmyMoveAuxiliaryValue0;

typedef qword GraphicsProjectedPointEdxEax8;

typedef dword GraphicsPixelChannelBitShift;

typedef dword ArmyMoveAuxiliaryValue1;

typedef dword InGamePointerCallbackValue2;

typedef dword InGamePointerCallbackValue3;

typedef dword InGamePointerCallbackValue0;

typedef dword InGamePointerCallbackValue1;

typedef dword UiTransferSenderContext;

typedef int RomRuntimeNodeRecordAddress32;

typedef dword LevelStartingResourceAmountQ4;

typedef int GraphicsPrimitiveScreenCoordinate;

typedef int ModelLookupKeyIndex;

typedef dword TimerCallbackSlotByteOffset;

typedef int AiCandidateScore32;

typedef dword DosDriveLetterCode32;

typedef qword Win32DriveCapacityEdxEax8;

typedef dword FrontendBackendSessionValue;

typedef dword GraphicsElapsedTickCount;

typedef int TerrainOverlayCellRuntimeValue;

typedef dword FrontendNetworkTickInterval;

typedef word WinSockSocketCount16;

typedef int UiPointerRegionCode;

typedef dword NetworkPortHostOrder;

typedef dword ModelRuntimeCount;

typedef dword ModelLookupKeyClass;

typedef dword ScenarioCatalogSourceByteCount;

typedef dword SessionTransferTimeoutTicks;

typedef int GraphicsDistanceAttenuationTableAddress32;

typedef dword GraphicsIntegerScale;

typedef int FactionResourceIncomeQ4;

typedef dword UiPageIndex;

typedef dword FrontendSnapshotChunkByteOffset;

typedef dword UiTransferMailboxByteOffset;

typedef int CameraScrollStepPixels;

typedef dword FactionPackedRelationStates;

typedef dword FactionResourceAmountQ4;

typedef word WinSockVersionWord;

typedef dword GraphicsBitsPerPixel;

typedef dword FncExportIndex;

typedef int SoftwareMaskRadiusStep;

typedef int ImpactDamageValue32;

typedef dword ArenaPayloadByteCount;

typedef dword SprRelocationCount;

typedef dword FileIoByteCount;

typedef dword GraphicsPaletteBankCount;

typedef dword FrontendPlayerIndex;

typedef int FieldGridDimensionCells;

typedef dword RandomSeed;

typedef int GraphicsProjectedCoordinate;

typedef dword FrontendResultsFactionFieldByteOffset;

typedef dword FrontendReturnCallbackContext32;

typedef dword LevelStartCameraMagnitudeQ12;

typedef dword WideNumberFractionalDigitCount;

typedef dword TerrainHeightBrushDeltaSource;

typedef dword MovieStreamOffset;

typedef dword PckHuffmanRunLength;

typedef dword KeyboardEventRingIndex;

typedef int DepthIntervalCenter32;

typedef dword AudioMixerGainQ15;

typedef dword PersistentSettingsMutationCount;

typedef word Win32DayOfWeek16;

typedef int PlayerStateLookupValue1;

typedef int PlayerStateLookupValue0;

typedef dword FrontendPlayerValue8C;

typedef word UiSelectionDetailTextBuffer64Utf16[64];

typedef dword UiNumericCursorFrameIndex;

typedef dword ArmyBuildDurationQ5;

typedef dword UiPointerButtonMask;

typedef qword RuntimeImagePointerByteSizeEdxEax8;

typedef int FactionRelationCounter;

typedef dword TerrainDirectionalScanStep;

typedef dword InGameLoadedResourcePointerCount;

typedef dword RichTextOpcode1APayloadValue32;

typedef dword RomRecordTableIndex;

typedef dword CommandPayloadDword04;

typedef undefined8 MmxPacked64;

typedef dword FactionAnchorCooldownTicks;

typedef dword CommandPayloadDword0C;

typedef dword CommandPayloadDword08;

typedef dword Win32WindowMessageId;

typedef dword PckHeaderDwordCount;

typedef int InGameCommandHandlerAddress32;

typedef dword FrontendPackedTextCommandState;

typedef dword MovieStreamFileOffset;

typedef dword MovieFrameIntervalMilliseconds;

typedef int AiArmyAssetRecordAddress32;

typedef dword LocaleClockMinuteStack32;

typedef qword FixedSinCosEdxEax8;

typedef dword TextResourceId;

typedef struct FrontendPersistentSettingsPage417C FrontendPersistentSettingsPage417C, *PFrontendPersistentSettingsPage417C;

typedef struct FrontendPersistentSettingsPage417C *FrontendPersistentSettingsPageSourceNodePtr;

struct FrontendPersistentSettingsPage417C {
    struct UiNodeBase pageRoot; 
    byte reserved004C_019B[336]; 
    struct UiPageStackControl settingsPageStack; 
    byte reserved01F0_2487[8856]; 
    struct UiNodeBase sourceNode; 
    byte reserved24D4_3BBB[5864]; 
    struct UiSelectableControl musicEnabledControl; 
    byte reserved3C10_3C1B[12]; 
    struct UiSelectableControl soundEffectsEnabledControl; 
    byte reserved3C70_3C7B[12]; 
    struct UiSelectableControl reverseStereoControl; 
    byte reserved3CD0_3DEF[288]; 
    struct UiNumericTextControl soundEffectsGainControl; 
    byte reserved3E84_3F6B[232]; 
    struct UiNumericTextControl movieDefaultAudioGainControl; 
    byte reserved4000_40E7[232]; 
    struct UiNumericTextControl musicGainControl; 
};

typedef uint UiCommandRuntimeFlagMask;

typedef int FixedVectorStateAddress32;

typedef dword WorldMotionValue74;

typedef dword WorldMotionValue70;

typedef int FixedMathScale32;

typedef dword ShotRuntimeState14;

typedef dword WorldMotionValue78;

typedef int ArmyLinkedChildDefinitionContextAddress32;

typedef int AiPlacementWorkspaceRecordAddress32;

typedef dword Win32MessageTimestamp32;

typedef int FactionProgressScore;

typedef dword GameSpeedPercent;

typedef dword WinSockFdSetCount;

typedef dword EngineFileHandle;

typedef dword NetworkBackendSessionReturnValue32;

typedef dword TextResourcePageIndex;

typedef dword UiTransferPayloadByteCount;

typedef dword UiTransferMailboxByteCount;

typedef int InGameMapViewControlAddress32;

typedef dword FrontendHeartbeatTickCount;

typedef dword MoviePixelDimension;

typedef dword UiActionHandlerPageIndex;

typedef int SoftwareMaskThresholdStep;

typedef dword TerrainRegionCollectionCount;

typedef qword FixedDirectionXZEdxEax8;

typedef word Win32Second16;

typedef dword FactionArmyAssetCount;

typedef dword Win32WParam32;

typedef int DwordBlockRecordCount;

typedef dword FrontendTextCommandValue0;

typedef dword PersistentColorDepthBits;

typedef dword SoundAssetIndex;

typedef dword FrontendTextCommandValue2;

typedef dword FrontendTextCommandValue1;

typedef dword LevelPlayerAiClassOrMode;

typedef dword CommandLineOptionLengthBytes;

typedef dword UiBooleanState32;

typedef dword FrontendPlayerValue84;

typedef dword FrontendPlayerValue88;

typedef dword FrontendPlayerCount;

typedef dword FrontendCallbackArgument5;

typedef dword GraphicsShadingRecordCount;

typedef struct FrontendNetworkSetupPageState4BCC FrontendNetworkSetupPageState4BCC, *PFrontendNetworkSetupPageState4BCC;

typedef struct FrontendNetworkSetupPageState4BCC *FrontendNetworkSetupPageBackendListPtr;

struct FrontendNetworkSetupPageState4BCC {
    struct UiNodeBase rootNode; 
    byte reserved004C_036B[800]; 
    struct UiNodeBase compactLayoutControl; 
    byte reserved03B8_0507[336]; 
    struct UiPageStackControl primaryPageStack; 
    byte reserved055C_4A6F[17684]; 
    struct UiPointerListControl backendList; 
    byte reserved4AD4_4B67[148]; 
    struct UiPointerListControl sessionList; 
};

typedef dword UiFrameCount;

typedef dword MdlChildCount;

typedef qword MovieFrameDimensionsEdxEax8;

typedef int UiPixelMetric;

typedef dword FrontendStatusCode;

typedef int ModelLinkedDefinitionListAddress32;

typedef int FactionRuntimeRecordByteOffset;

typedef int SelectionPanelNumericValue32;

typedef int LevelStartCameraCoordinateQ12;

typedef dword KeyboardCharacterCode;

typedef int FactionAiPressureScore;

typedef dword FactionCapabilityFlags;

typedef dword NetworkByteCount;

typedef dword FactionRelationTick;

typedef dword TerrainDirectionRecordCount;

typedef byte ColorChannelByte;

typedef dword ArmyPlacementContext;

typedef int SerializedImageRelocationDelta;

typedef int FrontendScenarioSelectionControlAddress32;

typedef dword ScenarioCatalogRecordCount;

typedef int FixedVectorStepMultiplier32;

typedef dword RichTextNestedStreamPointerValue32;

typedef dword FactionRelationCapabilityState;

typedef uint TerrainMaterialByteValue;

typedef dword PersistentSettingsByteOffset;

typedef dword TerrainProjectedHeightThresholdQ20;

typedef dword ContinuationEntryAddress32;

typedef word NetworkDatagramByteCount16;

typedef dword TechnologyIndexOrRestoreCode;

typedef dword FrontendCapabilityFlags;

typedef word WinSockProtocolNumber16;

typedef dword FrontendCapturedStateFlag;

typedef dword WinMmTimerId;

typedef dword GraphicsProjectionScale;

typedef int GraphicsPrimitiveDepthFixed;

typedef dword FrontendProtocolMagic;

typedef dword FactionImageByteOffset;

typedef int SelectionMarkerIndex;

typedef dword RecentTextSerialCounter;

typedef dword PersistentSettingsByteCount;

typedef byte PaletteEntryFlagsByte;

typedef dword GraphicsRenderFlagMask;

typedef dword WideNumberDenominator32;

typedef int CameraScreenDeltaPixels;

typedef dword UiFrameDelayFrames;

typedef int FactionResourceScoreComponent;

typedef dword MovieAudioTrackCount;

typedef dword RecentTextHistoryEntryLimit;

typedef dword UiDirtyRectCount;

typedef dword PckDwordCopyCount;

typedef dword ModelRuntimeAttachmentIndex;

typedef word SoftwareColorLaneFixed16;

typedef dword FrontendIndexedSelectionArgument;

typedef dword LocaleCalendarMonthStack32;

typedef dword TimerFrequencyHz;

typedef dword PersistentShadingGridHalfSize;

typedef dword FactionTechnologyCount;

typedef int FrontendRootRuntimeAddress32;

typedef dword UiTransferRetryTickCount;

typedef dword PckCompactFieldImageByteCount;

typedef uint RichTextCommandFlagBits;

typedef dword GraphicsPixelDimension;

typedef int GridPathPassCount;

typedef dword GridPathUnreachableReferenceRow32;

typedef uint ArmyPlacementCollisionFilterFlags;

typedef dword WorldMotionSplineChannelByteOffset;

typedef dword UiPointerListFieldByteOffset;

typedef dword ModelAttachmentOrdinal;

typedef int FrontendBooleanState32;

typedef int AiSourceClassCount;

typedef dword UiActionQueueUsedBytes;

typedef dword AudioChannelCountStack32;

typedef dword MusicTrackClassId;

typedef dword GraphicsSubresourceOffset;

typedef dword ArmyRuntimeSavedOffset;

typedef dword LocaleCalendarYearStack32;

typedef int GraphicsObjectAddress32;

typedef dword GraphicsPixelChannelBitCount;

typedef dword MemoryByteCount;

typedef qword UiScrollableContentDimensionsEdxEax8;

typedef word Win32Minute16;

typedef dword LevelAssetRecordCount;

typedef dword OldUnitRecordCount;

typedef struct PckHuffmanNode PckHuffmanNode, *PPckHuffmanNode;

typedef struct PckHuffmanNode *PckHuffmanNodePtr;

typedef dword PckHuffmanWeight;

struct PckHuffmanNode {
    PckHuffmanWeight weight; 
    PckHuffmanNodePtr zeroChild; 
    PckHuffmanNodePtr oneChild; 
    PckHuffmanNodePtr parent; 
};

typedef dword UiControlCount;

typedef int InGameCommandPanelSourceAddress32;

typedef int NetworkTimeoutSeconds;

typedef dword PckRuntimePayloadOffset;

typedef dword RichTextCommandPayload32;

typedef dword InGameCommandPayloadTripletValue32;

typedef dword AiCandidateEntryKind;

typedef dword FieldGridOccupancyBlockCount;

typedef dword MovieStreamByteCount;

typedef dword SelectionMarkerLaneMask;

typedef dword ArmyPlacementCandidateCount;

typedef dword ModelNodePoolRelativeOffset;

typedef struct InGameCommandTextEntryPage2320 InGameCommandTextEntryPage2320, *PInGameCommandTextEntryPage2320;

typedef struct InGameCommandTextEntryPage2320 *InGameCommandTextEntryPageTextEditPtr;

typedef struct InGameCommandTextEditControlCC InGameCommandTextEditControlCC, *PInGameCommandTextEditControlCC;

struct InGameCommandTextEditControlCC {
    struct UiNodeBase base; 
    enum UiTextEditStateFlags editStateFlags; 
    UiActionId actionId; 
    UiPixelOffset horizontalScrollPixels; 
    dword valueOrCapacity58; 
    UiTextCodeUnitIndex cursorIndex; 
    UiTextCodeUnitIndex selectionStart; 
    UiTextCodeUnitIndex selectionEnd; 
    struct DirectSoundVoiceSet *activationSound; 
    word textBuffer[48]; 
};

struct InGameCommandTextEntryPage2320 {
    struct UiPageStackControl commandPageStack; 
    byte reserved0054_0057[4]; 
    struct InGameCommandTextEditControlCC commandTextEdit; 
    byte reserved0124_1E2B[7432]; 
    struct UiSelectableControl packedStateModeLowControl; 
    byte reserved1E80_1E8B[12]; 
    struct UiSelectableControl packedStateModeHighControl; 
    byte reserved1EE0_1EEB[12]; 
    struct UiSelectableControl packedStateModeFallbackControl; 
    byte reserved1F40_208B[332]; 
    struct UiSelectableControl selectionSlot0Control; 
    byte reservedSelectionSlotGap0[12]; 
    struct UiSelectableControl selectionSlot1Control; 
    byte reservedSelectionSlotGap1[12]; 
    struct UiSelectableControl selectionSlot2Control; 
    byte reservedSelectionSlotGap2[12]; 
    struct UiSelectableControl selectionSlot3Control; 
    byte reservedSelectionSlotGap3[12]; 
    struct UiSelectableControl selectionSlot4Control; 
    byte reservedSelectionSlotGap4[12]; 
    struct UiSelectableControl selectionSlot5Control; 
    byte reservedSelectionSlotGap5[12]; 
    struct UiSelectableControl selectionSlot6Control; 
};

typedef dword FactionContributionScaleQ8;

typedef dword PckLoadCapacityFlags;

typedef int SoftwareMaskRuntimeAddress32;

typedef dword FactionRelationStateNibble;

typedef dword FrontendPlayerRuntimeBlockCount;

typedef dword ModelCommandTarget0;

typedef int DamageAmount32;

typedef dword ModelCommandTarget1;

typedef dword InGameConditionRecordCount;

typedef dword RichTextInlinePayloadValue32;

typedef byte PckHuffmanBitOffset;

typedef struct SelectionInfoEntitySlots SelectionInfoEntitySlots, *PSelectionInfoEntitySlots;

struct SelectionInfoEntitySlots {
    struct GameEntityRuntime *entries[32]; 
};

typedef struct WinSockProtoEnt32 WinSockProtoEnt32, *PWinSockProtoEnt32;

struct WinSockProtoEnt32 {
    byte *name;
    byte **aliases;
    WinSockProtocolNumber16 protocolNumber; 
    word alignmentPadding;
};

typedef struct GraphicsCursorInputEvent18 GraphicsCursorInputEvent18, *PGraphicsCursorInputEvent18;

struct GraphicsCursorInputEvent18 {
    enum GraphicsCursorEventType eventType00; 
    enum GraphicsCursorButtonState buttonState04; 
    UiPixelCoordinate pointerX08; 
    UiPixelCoordinate pointerY0C; 
    UiPointerWheelDelta wheelDelta10; 
    GraphicsCursorClockValue clockValue14; 
};

typedef struct UiTextEditControl UiTextEditControl, *PUiTextEditControl;

struct UiTextEditControl {
    struct UiNodeBase base; 
    enum UiTextEditStateFlags editStateFlags; 
    UiActionId actionId; 
    UiPixelOffset horizontalScrollPixels; 
    dword valueOrCapacity58; 
    UiTextCodeUnitIndex cursorIndex; 
    UiTextCodeUnitIndex selectionStart; 
    UiTextCodeUnitIndex selectionEnd; 
    struct DirectSoundVoiceSet *activationSound; 
    word textPrefix6C[10]; 
};

typedef struct KeyboardInputEvent KeyboardInputEvent, *PKeyboardInputEvent;

struct KeyboardInputEvent {
    UiKeyboardEventCode keyCode00; 
    UiKeyboardStateMask stateMask04; 
};

typedef struct LevelAssetRuntimeImagePrefix370 LevelAssetRuntimeImagePrefix370, *PLevelAssetRuntimeImagePrefix370;

typedef struct LevelAssetHeader LevelAssetHeader, *PLevelAssetHeader;

typedef struct LevelPlayerSlotRecord LevelPlayerSlotRecord, *PLevelPlayerSlotRecord;

typedef struct LevelAssetPathOffsets LevelAssetPathOffsets, *PLevelAssetPathOffsets;

typedef struct LevelAssetResourceTableDirectory24 LevelAssetResourceTableDirectory24, *PLevelAssetResourceTableDirectory24;

typedef dword XeniteAmountQ4;

typedef dword TritiumAmountQ4;

struct LevelAssetResourceTableDirectory24 {
    LevelAssetPointerArrayByteCount loadedResourcePointerArrayBytes; 
    LevelAssetRecordCount armyAssetPathCount; 
    LevelAssetRelativeByteOffset armyAssetPathTableOffset; 
    LevelAssetRecordCount modelAssetPathCount; 
    LevelAssetRelativeByteOffset modelAssetPathTableOffset; 
    LevelAssetRecordCount effectAssetPathCount; 
    LevelAssetRelativeByteOffset effectAssetPathTableOffset; 
    LevelAssetRecordCount shotAssetPathCount; 
    LevelAssetRelativeByteOffset shotAssetPathTableOffset; 
};

struct LevelAssetPathOffsets {
    AssetRelativeOffset levelPathOffset;
    AssetRelativeOffset groundTextureBasePathOffset;
    AssetRelativeOffset surfaceTextureBasePathOffset;
    AssetRelativeOffset skyTextureBasePathOffset;
    AssetRelativeOffset armyTextureBasePathOffset;
    AssetRelativeOffset shotTextureBasePathOffset;
    AssetRelativeOffset effectTextureBasePathOffset;
    AssetRelativeOffset endingMovieBasePathOffset;
    AssetRelativeOffset soundBasePathOffset;
    AssetRelativeOffset technologyPathOffset;
};

struct LevelAssetHeader {
    struct GeneratedAssetCommonPrefix common;
    struct LevelAssetPathOffsets pathOffsets;
    dword opaqueHeaderDwordD8; 
    struct LevelAssetResourceTableDirectory24 resourceTables; 
    byte opaque100_16F[112]; 
    UiTextResourceId titleTextResourceIndex; 
    byte opaque174_18F[28]; 
    LevelCampaignAssociationIndex campaignAssociationIndex; 
    byte opaque194_1FF[108]; 
};

struct LevelPlayerSlotRecord {
    LevelStartCameraCoordinateQ12 startCameraXQ12; 
    LevelStartCameraCoordinateQ12 startCameraYQ12; 
    LevelStartCameraCoordinateQ12 startCameraZQ12; 
    LevelStartCameraMagnitudeQ12 startCameraMagnitudeQ12; 
    LevelPackedHeadingPitch packedHeadingLow16PitchHigh16; 
    XeniteAmountQ4 startXeniteQ4; 
    TritiumAmountQ4 startTritiumQ4; 
    LevelPlayerAiClassOrMode aiClassOrMode; 
};

struct LevelAssetRuntimeImagePrefix370 {
    struct LevelAssetHeader header; 
    struct LevelPlayerSlotRecord playerSlots[7]; 
    byte opaqueRuntimeTail2E0_36F[144]; 
};

typedef struct FrontendRootPageState26C4 FrontendRootPageState26C4, *PFrontendRootPageState26C4;

struct FrontendRootPageState26C4 {
    struct UiNodeBase rootNode; 
    byte reserved004C_036B[800]; 
    struct UiNodeBase compactLayoutControl; 
    byte reserved03B8_0507[336]; 
    struct UiPageStackControl primaryPageStack; 
    byte reserved055C_2677[8476]; 
    struct UiNodeBase returnToMainActionControl; 
};

typedef struct TerrainDirectionRecord TerrainDirectionRecord, *PTerrainDirectionRecord;

struct TerrainDirectionRecord {
    dword angleAComponent0ScaledQ28;
    dword angleAComponent1ScaledQ28;
    dword angleBComponent0ScaledQ28;
    dword packedAngleA_low16_AngleB_high16;
    dword scaleA;
    dword scaleB;
    word rateA;
    word rateB;
    dword reserved1C;
};

typedef struct ArmyRuntimeOrderHandlerMatrix11x24 ArmyRuntimeOrderHandlerMatrix11x24, *PArmyRuntimeOrderHandlerMatrix11x24;

struct ArmyRuntimeOrderHandlerMatrix11x24 {
    void (*runtimeUpdate[24])(struct WorldRuntimeContext *, struct ArmyRuntimeSlot *);
    void (*classMethodD[24])(struct WorldRuntimeContext *, struct ArmyRuntimeSlot *);
    void (*modelUnrebase[24])(struct ModelRuntimeSlot *);
    void (*modelRebaseOrLoadRepair[24])(struct ModelRuntimeSlot *);
    void (*modelClassInitialize[24])(struct ModelDefinitionRecordPrefix *, struct ModelRuntimeSlot *);
    void (*modelReleaseOrCommit[24])(struct ModelDefinitionRecordPrefix *, struct ModelRuntimeSlot *);
    void (*placementValidationCf[24])(struct WorldRuntimeContext *, struct ArmyRuntimeSlot *);
    void (*placementAssetClassDispatch[24])(dword, dword, dword, dword, int, int, struct ModelDefinitionRecordPrefix *, dword, struct WorldRuntimeContext *);
    void (*classCommand[24])(struct WorldRuntimeContext *, struct ArmyRuntimeSlot *);
    void (*gridInfluenceAdd[24])(struct GameEntityRuntime *);
    void (*gridInfluenceRemove[24])(struct GameEntityRuntime *);
};

typedef struct UiTimedListControl UiTimedListControl, *PUiTimedListControl;

typedef struct UiTimedListTreeRecord16 UiTimedListTreeRecord16, *PUiTimedListTreeRecord16;

typedef struct UiTimedListRuntimeExtendedView88 UiTimedListRuntimeExtendedView88, *PUiTimedListRuntimeExtendedView88;

struct UiTimedListTreeRecord16 {
    dword recordCountOrRowPayload00; 
    dword rowPayload04; 
    struct UiTimedListTreeRecord16 *nestedRecordBlockOrParentLink08; 
    enum UiTimedListRecordFlags recordFlags0C; 
};

struct UiTimedListControl {
    struct UiNodeBase base; 
    enum UiTimedListStateFlags listStateAndDelay; 
    struct UiTimedListTreeRecord16 *recordTree; 
    UiListRowCount rowCount; 
    UiPixelExtent rowHeight; 
    UiActionId actionId; 
    struct UiTimedListTreeRecord16 *selectedRecord; 
    void (*recordSelectionCallback)(struct UiTimedListTreeRecord16 *, struct UiTimedListRuntimeExtendedView88 *); 
    struct GraphicsTextureSourceAsset *rowTextureSource; 
    dword observedDrawParameter6C; 
};

struct UiTimedListRuntimeExtendedView88 {
    struct UiTimedListControl base; 
    dword observedDrawParameter70; 
    dword observedDrawParameter74; 
    dword observedDrawParameter78; 
    dword observedDrawParameter7C; 
    dword observedDrawParameter80; 
    dword observedDrawParameter84; 
};

typedef struct FrontendResultsColumnDrawDispatchTable18 FrontendResultsColumnDrawDispatchTable18, *PFrontendResultsColumnDrawDispatchTable18;

struct FrontendResultsColumnDrawDispatchTable18 {
    void *reserved00Target; 
    void *reserved01Target; 
    void *colourTarget; 
    void *economyTarget; 
    void *militaryTarget; 
    void *pointsTarget; 
    void *playerTarget; 
    void *factionTarget; 
    void *factionField98Target; 
    void *factionField9CTarget; 
    void *factionFieldA0Target; 
    void *factionFieldA4Target; 
    void *factionFieldA8Target; 
    void *factionFieldACTarget; 
    void *factionFieldB0Target; 
    void *factionFieldB4Target; 
    void *factionFieldB8Target; 
    void *factionFieldBCTarget; 
};

typedef struct FrontendSessionDiscoveryRecordB0 FrontendSessionDiscoveryRecordB0, *PFrontendSessionDiscoveryRecordB0;

typedef struct FrontendPacket50001SessionAdvertisement FrontendPacket50001SessionAdvertisement, *PFrontendPacket50001SessionAdvertisement;

typedef struct UiTransferEndpointDescriptor UiTransferEndpointDescriptor, *PUiTransferEndpointDescriptor;

typedef struct UiTransferPacketHeader UiTransferPacketHeader, *PUiTransferPacketHeader;

struct UiTransferPacketHeader {
    enum UiTransferPacketPackedType packedTypeAndUnitCount; 
    UiTransferSequenceToken sequenceToken; 
    UiTransferSenderContext senderContext; 
    UiTransferXorChecksum xorChecksum; 
};

struct FrontendPacket50001SessionAdvertisement {
    struct UiTransferPacketHeader header; 
    UiTransferPayloadByteCount payloadByteCount; 
    enum UiTransferJoinAvailability joinAvailableFlag; 
    word sessionTitleUtf16[20]; 
    word hostDescriptionUtf16[44]; 
    word playerCountTextUtf16[4]; 
};

struct UiTransferEndpointDescriptor {
    union NetworkEndpointAddressHeader4 addressHeader; 
    NetworkIpv4AddressNetworkOrder ipv4AddressNetworkOrder; 
    byte zeroPadding[8]; 
};

struct FrontendSessionDiscoveryRecordB0 {
    struct FrontendPacket50001SessionAdvertisement advertisement; 
    struct UiTransferEndpointDescriptor senderEndpoint; 
};

typedef struct AiCandidateWorkspaceEntry AiCandidateWorkspaceEntry, *PAiCandidateWorkspaceEntry;

struct AiCandidateWorkspaceEntry {
    dword weightedScoreAndKind; 
    dword entityIdAndMultiplicity; 
};

typedef struct UiCommandQueueRecord UiCommandQueueRecord, *PUiCommandQueueRecord;

struct UiCommandQueueRecord {
    PackedUiCommandAndPlayerId packedCommandAndPlayerId; 
    dword payloadDword04;
    dword payloadDword08;
    dword payloadDword0C;
};

typedef struct LevelArchivePathTemplate18 LevelArchivePathTemplate18, *PLevelArchivePathTemplate18;

struct LevelArchivePathTemplate18 {
    word prefixCodeUnits[5]; 
    union Utf16DecimalDigitPair4 decimalDigits; 
    word suffixCodeUnits[5]; 
};

typedef struct RuntimeModelClassPriorityTable24 RuntimeModelClassPriorityTable24, *PRuntimeModelClassPriorityTable24;

struct RuntimeModelClassPriorityTable24 {
    enum RuntimeModelClassPriority modelClass00Priority; 
    enum RuntimeModelClassPriority modelClass01Priority; 
    enum RuntimeModelClassPriority modelClass02Priority; 
    enum RuntimeModelClassPriority modelClass03Priority; 
    enum RuntimeModelClassPriority modelClass04Priority; 
    enum RuntimeModelClassPriority modelClass05Priority; 
    enum RuntimeModelClassPriority modelClass06Priority; 
    enum RuntimeModelClassPriority modelClass07Priority; 
    enum RuntimeModelClassPriority modelClass08Priority; 
    enum RuntimeModelClassPriority modelClass09Priority; 
    enum RuntimeModelClassPriority modelClass10Priority; 
    enum RuntimeModelClassPriority modelClass11Priority; 
    enum RuntimeModelClassPriority modelClass12Priority; 
    enum RuntimeModelClassPriority modelClass13Priority; 
    enum RuntimeModelClassPriority modelClass14Priority; 
    enum RuntimeModelClassPriority modelClass15Priority; 
    enum RuntimeModelClassPriority modelClass16Priority; 
    enum RuntimeModelClassPriority modelClass17Priority; 
    enum RuntimeModelClassPriority modelClass18Priority; 
    enum RuntimeModelClassPriority modelClass19Priority; 
    enum RuntimeModelClassPriority modelClass20Priority; 
    enum RuntimeModelClassPriority modelClass21Priority; 
    enum RuntimeModelClassPriority modelClass22Priority; 
    enum RuntimeModelClassPriority modelClass23Priority; 
};

typedef struct WinSockServEnt32 WinSockServEnt32, *PWinSockServEnt32;

struct WinSockServEnt32 {
    byte *name;
    byte **aliases;
    NetworkPortNetworkOrder portNetworkOrder; 
    word alignmentPadding;
    byte *protocolName;
};

typedef struct UiTextListControl UiTextListControl, *PUiTextListControl;

struct UiTextListControl {
    struct UiNodeBase base; 
    enum UiTextListStateFlags listStateFlags; 
    word **rowTextSlots; 
    UiListRowCount rowCount; 
    UiPixelExtent rowHeight; 
    UiActionId actionId; 
    word **selectedRowSlot; 
    struct DirectSoundVoiceSet *activationSound; 
};

typedef struct SprRelocationBlockHeader20 SprRelocationBlockHeader20, *PSprRelocationBlockHeader20;

struct SprRelocationBlockHeader20 {
    SerializedRelativeByteOffset blockByteSize;
    dword reserved04;
    SprRelocationCount fixedRecordCount40; 
    SprRelocationCount pointerRelocationCount;
    byte reserved10_1F[16];
};

typedef struct InGameConditionRecord InGameConditionRecord, *PInGameConditionRecord;

typedef struct InGameConditionRuntime InGameConditionRuntime, *PInGameConditionRuntime;

typedef union InGameConditionPayloadPointer InGameConditionPayloadPointer, *PInGameConditionPayloadPointer;

typedef enum InGameConditionKind {
    MODEL_RUNTIME=0,
    SHOT_RUNTIME=1,
    EFFECT_RUNTIME=2
} InGameConditionKind;

union InGameConditionPayloadPointer {
    struct ModelRuntimeSlot *modelRuntime; 
    struct ShotRuntimeSlot *shotRuntime; 
    struct EffectRuntimeSlot *effectRuntime; 
    void *raw; 
};

struct InGameConditionRecord {
    void *primaryRuntimePointer00; 
    void *secondaryRuntimePointer04; 
    struct InGameConditionRuntime *ownerRuntime08; 
    byte reserved0C_2F[36]; 
    struct GraphicsPaletteAsset *paletteAsset30; 
    struct GraphicsTextureSet *textureSet34; 
    byte reserved38_3F[8]; 
    struct SpriteAssetHeader *spriteAsset40; 
    byte reserved44_47[4]; 
    union InGameConditionPayloadPointer payload48; 
    enum RuntimeRegistrationRecordFlags flags; 
    byte reserved50_5B[12]; 
    void *auxiliaryPointer5C; 
    byte reserved60_A3[68]; 
    enum InGameConditionKind conditionKindA4; 
    byte reservedA8_C3[28]; 
    void *nestedBaseC4; 
    dword nestedCountC8; 
    void *nestedPointersCC[13]; 
};

struct InGameConditionRuntime {
    byte reserved00_4F[80]; 
    struct WorldRuntimeContext *worldContext50; 
    byte reserved54_57[4]; 
    struct InGameConditionRecord *records58; 
    byte reserved5C_AB[80]; 
    InGameConditionRecordCount recordCountAC; 
    byte reservedB0_D7[40]; 
    struct InGameConditionRecord *tailRecordD8; 
};

typedef struct InGameNotificationPayload18 InGameNotificationPayload18, *PInGameNotificationPayload18;

struct InGameNotificationPayload18 {
    Q12 primaryWorldCoordinateQ12_00; 
    Q12 secondaryWorldCoordinateQ12_04; 
    AngleTurn32 primaryOrientationAngle08; 
    dword orientationOrPresentationValue0C; 
    dword reserved10; 
    enum InGameNotificationPayloadKind payloadKind14; 
};

typedef struct WinSockHostEnt32 WinSockHostEnt32, *PWinSockHostEnt32;

struct WinSockHostEnt32 {
    byte *canonicalName;
    byte **aliases;
    enum NetworkAddressFamily addressType; 
    enum WinSockIpv4AddressLength addressLength; 
    byte **addressList;
};

typedef struct WinSockFdSet64 WinSockFdSet64, *PWinSockFdSet64;

struct WinSockFdSet64 {
    WinSockFdSetCount count; 
    dword sockets[64];
};

typedef struct UiTransferAuxiliaryEndpointRecord80 UiTransferAuxiliaryEndpointRecord80, *PUiTransferAuxiliaryEndpointRecord80;

struct UiTransferAuxiliaryEndpointRecord80 {
    struct UiTransferEndpointDescriptor endpoint; 
    dword transferTimeoutTicks; 
    byte reserved0014_007F[108]; 
};

typedef struct FrontendModelPointerContextRuntimeState118 FrontendModelPointerContextRuntimeState118, *PFrontendModelPointerContextRuntimeState118;

struct FrontendModelPointerContextRuntimeState118 {
    struct UiNodeBase base; 
    enum FrontendModelPointerContextFlags contextFlags; 
    byte reserved50_5F[16]; 
    Q12 hitReferenceWorldXQ12; 
    Q12 hitReferenceWorldYQ12; 
    Q12 hitReferenceWorldZQ12; 
    byte reserved6C_D7[108]; 
    struct ModelRuntimeNode *candidateModelListHead; 
    dword observedContextValueDC; 
    struct ModelRuntimeNode *selectedModelNode; 
    int selectedHitMetric; 
    dword callbackArgumentE8; 
    dword callbackArgumentEC; 
    dword callbackArgumentF0; 
    byte reservedF4_103[16]; 
    dword (*resolvedActionCallback104)(dword, dword, dword, int, struct ModelRuntimeNode *, struct FrontendModelPointerContextRuntimeState118 *); 
    dword (*resolvedActionCallback108)(dword, dword, dword, int, struct ModelRuntimeNode *, struct FrontendModelPointerContextRuntimeState118 *); 
    dword (*resolvedActionCallback10C)(dword, dword, dword, int, struct ModelRuntimeNode *, struct FrontendModelPointerContextRuntimeState118 *); 
    dword (*resolvedActionCallback110)(dword, dword, dword, int, struct ModelRuntimeNode *, struct FrontendModelPointerContextRuntimeState118 *); 
    dword (*resolvedActionCallback114)(dword, dword, dword, int, struct ModelRuntimeNode *, struct FrontendModelPointerContextRuntimeState118 *); 
};

typedef struct RecentTextHistoryView RecentTextHistoryView, *PRecentTextHistoryView;

typedef struct RecentTextHistoryPointerList RecentTextHistoryPointerList, *PRecentTextHistoryPointerList;

typedef struct RecentTextHistorySlot RecentTextHistorySlot, *PRecentTextHistorySlot;

struct RecentTextHistorySlot {
    word text[128]; 
};

struct RecentTextHistoryPointerList {
    dword count; 
    struct RecentTextHistorySlot *entries[8]; 
};

struct RecentTextHistoryView {
    byte reserved00_57[88]; 
    struct RecentTextHistoryPointerList recentTextPointerList; 
    byte reserved7C_FF[132]; 
};

typedef struct InGameNotificationQueueRecord20 InGameNotificationQueueRecord20, *PInGameNotificationQueueRecord20;

struct InGameNotificationQueueRecord20 {
    InGameNotificationMovieId notificationMovieId00; 
    InGameNotificationPriority priority04; 
    struct InGameNotificationPayload18 payload08; 
};

typedef struct MdlSerializedNodeHeader38 MdlSerializedNodeHeader38, *PMdlSerializedNodeHeader38;

struct MdlSerializedNodeHeader38 {
    AssetRecordByteCount nodeByteSize;
    MdlNodeFlags nodeFlags; 
    AngleTurn32 localRotationAngle0;
    AngleTurn32 localRotationAngle1;
    AngleTurn32 localRotationAngle2;
    MdlChildCount childCount;
    SerializedRelativeByteOffset childSerializedOffsets[6]; 
    union SpriteAssetReferenceOrSavedId4 spriteAssetReference; 
    OwnedNestedResourceFlag ownedNestedResourcePresent;
};

typedef struct WinSockAddress WinSockAddress, *PWinSockAddress;

struct WinSockAddress {
    union NetworkEndpointAddressHeader4 addressHeader; 
    NetworkIpv4AddressNetworkOrder ipv4AddressNetworkOrder; 
    byte zeroPadding[8]; 
};

typedef struct WorldMotionSplineKeyframe WorldMotionSplineKeyframe, *PWorldMotionSplineKeyframe;

struct WorldMotionSplineKeyframe {
    WorldMotionSplineValueQ12 channel0Q12; 
    WorldMotionSplineValueQ12 channel1Q12; 
    WorldMotionSplineValueQ12 channel2Q12; 
    WorldMotionSplineValueQ12 channel3Q12; 
    WorldMotionSplineValueQ12 channel4Q12; 
    WorldMotionSplineValueQ12 channel5Q12; 
    WorldMotionSplineTimeQ12 timeQ12; 
    dword reserved1C;
};

typedef struct PatchArchivePathTemplate18 PatchArchivePathTemplate18, *PPatchArchivePathTemplate18;

struct PatchArchivePathTemplate18 {
    word prefixCodeUnits[5]; 
    union Utf16DecimalDigitPair4 decimalDigits; 
    word suffixCodeUnits[5]; 
};

typedef struct TerrainMaterialSuffixEntry TerrainMaterialSuffixEntry, *PTerrainMaterialSuffixEntry;

struct TerrainMaterialSuffixEntry {
    word lowercaseLetterUtf16;
    word terminator;
};

typedef struct UiListControl UiListControl, *PUiListControl;

struct UiListControl {
    struct UiNodeBase base; 
    enum UiListStateFlags listStateFlags; 
    void **rowSlots; 
    UiListRowCount rowCount; 
    UiPixelExtent rowHeight; 
    UiActionId actionId; 
    void **selectedRowSlot; 
    dword reserved64; 
    struct DirectSoundVoiceSet *activationSound; 
};

typedef struct ScenarioCampaignDataPathTemplate2A ScenarioCampaignDataPathTemplate2A, *PScenarioCampaignDataPathTemplate2A;

struct ScenarioCampaignDataPathTemplate2A {
    word prefixCodeUnits[14]; 
    union Utf16DecimalDigitPair4 decimalDigits; 
    word suffixCodeUnits[5]; 
};

typedef struct SprPointerRelocationRecord40 SprPointerRelocationRecord40, *PSprPointerRelocationRecord40;

struct SprPointerRelocationRecord40 {
    dword pointerOrSerializedOffset00;
    byte reserved04_0B[8];
    dword pointerOrSerializedOffset0C;
    byte reserved10_17[8];
    dword pointerOrSerializedOffset18;
    byte reserved1C_3F[36];
};

typedef struct GraphicsProjectedPoint2i GraphicsProjectedPoint2i, *PGraphicsProjectedPoint2i;

struct GraphicsProjectedPoint2i {
    GraphicsProjectedCoordinate x; 
    GraphicsProjectedCoordinate y; 
};

typedef struct EntityPathingPriorityPair EntityPathingPriorityPair, *PEntityPathingPriorityPair;

struct EntityPathingPriorityPair {
    pointer entity;
    sdword priority;
};

typedef struct FrontendResultsRowMetrics FrontendResultsRowMetrics, *PFrontendResultsRowMetrics;

struct FrontendResultsRowMetrics {
    byte reserved00_5B[92]; 
    UiPixelMetric headerBaselineOffsetPixels; 
    UiPixelMetric rowAdvancePixels; 
};

typedef struct UiTransferPacket UiTransferPacket, *PUiTransferPacket;

struct UiTransferPacket {
    struct UiTransferPacketHeader header; 
    struct UiCommandQueueRecord commands[3]; 
};

typedef struct WinSockTimeVal32 WinSockTimeVal32, *PWinSockTimeVal32;

struct WinSockTimeVal32 {
    NetworkTimeoutSeconds seconds; 
    NetworkTimeoutMicroseconds microseconds; 
};

typedef struct WinSockData11 WinSockData11, *PWinSockData11;

struct WinSockData11 {
    WinSockVersionWord version; 
    WinSockVersionWord highestVersion; 
    byte description[257];
    byte systemStatus[129];
    WinSockSocketCount16 maximumSockets; 
    NetworkDatagramByteCount16 maximumUdpDatagram; 
    byte vendorInfoAlignmentPadding[2];
    byte *vendorInfo;
};

typedef struct UiScrollableControl UiScrollableControl, *PUiScrollableControl;

struct UiScrollableControl {
    struct UiNodeBase base; 
    enum UiScrollableStateFlags scrollStateFlags; 
    UiPixelExtent viewportWidth; 
    UiPixelExtent viewportHeight; 
    UiPixelExtent contentWidth; 
    UiPixelExtent contentHeight; 
    UiPixelOffset contentOriginX; 
    UiPixelOffset contentOriginY; 
    UiPixelOffset scrollOffsetX; 
    UiPixelOffset scrollOffsetY; 
    UiPixelCoordinate pointerAnchorX; 
    UiPixelCoordinate pointerAnchorY; 
    UiPixelCoordinate horizontalThumbLeft; 
    UiPixelCoordinate verticalThumbTop; 
    UiPixelCoordinate horizontalThumbRight; 
    UiPixelCoordinate verticalThumbBottom; 
    UiPixelOffset autoScrollStepX; 
    UiPixelOffset autoScrollStepY; 
};

typedef struct InGameTargetingContext InGameTargetingContext, *PInGameTargetingContext;

struct InGameTargetingContext {
    struct UiNodeBase base; 
    byte reserved4C_4F[4]; 
    enum InGameTargetingObservedActionState actionState; 
    byte reserved54_FF[172]; 
};

typedef struct ModelRaycastTriangleDescriptor ModelRaycastTriangleDescriptor, *PModelRaycastTriangleDescriptor;

struct ModelRaycastTriangleDescriptor {
    struct GraphicsFixedVec3 *vertex0; 
    byte reservedVertex0Metadata04_0B[8]; 
    struct GraphicsFixedVec3 *vertex1; 
    byte reservedVertex1Metadata10_17[8]; 
    struct GraphicsFixedVec3 *vertex2; 
    byte reservedVertex2Metadata1C_23[8]; 
    GraphicsPlaneNormalFixed planeNormalX; 
    GraphicsPlaneNormalFixed planeNormalY; 
    GraphicsPlaneNormalFixed planeNormalZ; 
    byte reserved30_3F[16]; 
};

typedef struct GraphicsOffscreenSceneExtents GraphicsOffscreenSceneExtents, *PGraphicsOffscreenSceneExtents;

struct GraphicsOffscreenSceneExtents {
    GraphicsSceneExtentFixed horizontalExtent; 
    GraphicsSceneExtentFixed verticalExtent; 
};

typedef struct UiSettingsValueControl UiSettingsValueControl, *PUiSettingsValueControl;

struct UiSettingsValueControl {
    struct UiNodeBase base; 
    byte reserved4C_57[12]; 
    dword boundValue; 
    byte reserved5C_67[12]; 
};

typedef struct SprGroupRelocationHeader20 SprGroupRelocationHeader20, *PSprGroupRelocationHeader20;

struct SprGroupRelocationHeader20 {
    SerializedRelativeByteOffset nextGroupByteOffset; 
    SprRelocationCount relocationBlockCount;
    byte reserved08_1F[24];
};

typedef struct GraphicsOffscreenViewParameters GraphicsOffscreenViewParameters, *PGraphicsOffscreenViewParameters;

struct GraphicsOffscreenViewParameters {
    GraphicsWorldCoordinateQ12 originX; 
    GraphicsWorldCoordinateQ12 originY; 
    GraphicsWorldCoordinateQ12 originZ; 
    GraphicsProjectionScale projectionScale; 
    GraphicsViewAngle16 viewAngle0; 
    GraphicsViewAngle16 viewAngle1; 
    GraphicsProjectionShift projectionShift; 
};

typedef struct NetworkSessionContext NetworkSessionContext, *PNetworkSessionContext;

struct NetworkSessionContext {
    dword transportContext00; 
    dword ipv4AddressNetworkOrder; 
    byte reserved08_FF[248];
};

typedef struct InGameCameraCommandDispatchTable InGameCameraCommandDispatchTable, *PInGameCameraCommandDispatchTable;

typedef struct InGameCameraCommandDispatchRecord InGameCameraCommandDispatchRecord, *PInGameCameraCommandDispatchRecord;

struct InGameCameraCommandDispatchRecord {
    enum InGameCameraCommandKeyCode keyCode;
    dword requiredModifierMask;
    ContinuationEntryAddress32 continuationEntryAddress; 
};

struct InGameCameraCommandDispatchTable {
    struct InGameCameraCommandDispatchRecord records[16];
    dword terminatorKeyCode;
    byte alignmentPadding[12];
};

typedef struct InGameRuntimeRootImageC3E4 InGameRuntimeRootImageC3E4, *PInGameRuntimeRootImageC3E4;

typedef struct UiRootNode UiRootNode, *PUiRootNode;

typedef struct MovieRuntime MovieRuntime, *PMovieRuntime;

typedef struct SelectionPlayerPairRecord SelectionPlayerPairRecord, *PSelectionPlayerPairRecord;

typedef enum UiRootFlags {
    UI_ROOT_DISABLE_POINTER_HIT_TEST=256 
} UiRootFlags;

typedef struct UiRootCallbacks UiRootCallbacks, *PUiRootCallbacks;

typedef struct MovieFileHeader MovieFileHeader, *PMovieFileHeader;

typedef enum MovieStreamState {
    MOVIE_STREAM_IDLE=0,
    MOVIE_STREAM_FILL_REQUESTED=1,
    MOVIE_STREAM_READ_FAILED=2,
    MOVIE_STREAM_SHUTDOWN=3
} MovieStreamState;

struct SelectionPlayerPairRecord {
    dword pairKey; 
    dword pairValue; 
};

struct MovieFileHeader {
    struct GeneratedAssetCommonPrefix common; 
    MoviePixelDimension widthPixels; 
    MoviePixelDimension heightPixels; 
    MovieFrameCount frameCount; 
    MovieAudioTrackCount audioTrackCount; 
    MovieStreamByteCount videoStreamBytes; 
    dword audioTrackBytes[14]; 
    MovieFrameIntervalMilliseconds frameIntervalMilliseconds; 
    byte reserved100_1FF[256]; 
};

struct UiRootNode {
    struct UiNodeBase base; 
    enum UiRootFlags rootFlags; 
    struct UiRootCallbacks *callbacks; 
    struct UiRootNode *previousRoot; 
};

struct InGameRuntimeRootImageC3E4 {
    struct UiRootNode rootUi0000; 
    byte opaque0058_017B[292]; 
    struct UiPageStackControl primaryPageStack017C;
    byte opaque01D0_022B[92]; 
    struct MovieRuntime *activeEndMovieRuntime022C;
    dword endMoviePlaybackState0230; 
    byte opaque0234_02F7[196]; 
    struct UiPageStackControl endMoviePageStack02F8;
    byte opaque034C_08D3[1416]; 
    struct MovieRuntime *levelMovieRuntime08D4;
    byte opaque08D8_08E3[12]; 
    struct UiNodeBase playerStatusNode08E4;
    byte opaque0930_093B[12]; 
    dword playerStatusLayoutMetric093C; 
    byte opaque0940_09B7[120]; 
    struct RecentTextHistoryPointerList recentTextHistory09B8;
    byte opaque09DC_0A03[40]; 
    dword optionalUiLayoutState0A04; 
    byte opaque0A08_0A2F[40]; 
    struct WorldRuntimeContext worldRuntime0A30;
    void (*worldOverlayCallback0B8C)(dword, struct WorldRuntimeContext *); 
    byte opaque0B90_0B9F[16]; 
    struct SelectionPlayerPairRecord *localPlayerPairRecords0BA0;
    dword localPlayerPairCount0BA4;
    Q12 fieldRegionOriginWorldXQ12_0BA8;
    Q12 fieldRegionOriginWorldYQ12_0BAC;
    byte opaque0BB0_0BCF[32]; 
    struct UiPageStackControl technologyPageStack0BD0;
    byte opaque0C24_24DF[6332]; 
    dword localNetworkUiStateFlags24E0;
    byte opaque24E4_40AB[7112]; 
    struct UiPageStackControl optionalUiPageStack40AC; 
    byte opaque4100_452F[1072]; 
    struct UiPageStackControl optionalUiPageStack4530; 
    byte opaque4584_4643[192]; 
    struct UiPageStackControl optionalUiPageStack4644; 
    byte opaque4698_4937[672]; 
    dword observedTerrainCompositeFlags4938;
    byte opaque493C_49B3[120]; 
    int primaryResourceDisplayCurrent49B4;
    int primaryResourceDisplayLimit49B8;
    byte opaque49BC_4A4B[144]; 
    int secondaryResourceDisplayCurrent4A4C;
    int secondaryResourceDisplayLimit4A50;
    byte opaque4A54_4AE3[144]; 
    int transientContributionDisplay4AE4;
    int progressLimitDisplay4AE8;
    byte opaque4AEC_4B27[60]; 
    int combinedProgressOrArmyScaleDisplay4B28;
    byte opaque4B2C_4D53[552]; 
    dword observedRelationTransitionFlags4D54;
    byte opaque4D58_9A6B[19732]; 
    FixedPlanarPointEdxEax8 fieldGridPosition9A6C;
    byte opaque9A74_9B4B[216]; 
    enum InGameNotificationInteractionState sessionNotificationInteractionState9B4C; 
    dword observedSessionNotificationValue9B50;
    dword notificationPlaybackCompletionCode9B54; 
    byte opaque9B58_9E3F[744]; 
    struct InGameNotificationPayload18 activeNotificationPayload9E40; 
    byte opaque9E58_9E5F[8]; 
    struct InGameNotificationQueueRecord20 notificationQueue9E60[4]; 
    byte opaque9EE0_9FAB[204]; 
    struct UiPageStackControl selectionDetailPageStack9FAC;
    byte opaqueA000_A05F[96]; 
    dword selectionDetailArmyAssetValueA060;
    byte opaqueA064_A067[4]; 
    struct GameEntityRuntime *selectionDetailEntityA068;
    byte opaqueA06C_C3E3[9080]; 
};

struct MovieRuntime {
    struct GeneratedAssetCommonPrefix textureCommon; 
    MovieSubresourceCount subresourceCount; 
    MoviePaletteBankCount paletteBankCount; 
    MovieSubresourceTableOffset subresourceTableOffset; 
    dword reservedBC; 
    struct MovieFileHeader *fileHeader; 
    MovieFrameIndex currentFrameIndex; 
    MovieStreamOffset videoStreamOffset; 
    struct DirectSoundVoiceSet *audioVoiceSet; 
    struct IDirectSoundBuffer *activeAudioBuffer; 
    void *streamHandle; 
    MovieSharedStreamHandleFlag streamHandleIsSharedPackage; 
    byte *loadedVideoEnd; 
    MovieStreamByteCount remainingVideoBytes; 
    MovieStreamFileOffset streamFileOffset; 
    MovieOpenFlags openFlags; 
    MovieAudioGainQ15 audioGainQ15; 
    MovieWorkerActiveFlag workerActive; 
    enum MovieStreamState streamState; 
    void *refillSemaphore; 
    dword reservedFC; 
    byte reserved100_1FF[256]; 
    struct GraphicsTextureSourceEntry sourceEntry; 
    dword argbPixels[1]; 
};

struct UiRootCallbacks {
    void (*closeCf)(struct UiRootNode *); 
    void (*frameUpdate)(struct UiRootNode *); 
    void (*method08)(struct UiRootNode *); 
    void (*keyboardFallbackCf)(dword, dword, struct UiRootNode *); 
};

typedef struct InGameScheduledConditionDispatchTable14 InGameScheduledConditionDispatchTable14, *PInGameScheduledConditionDispatchTable14;

struct InGameScheduledConditionDispatchTable14 {
    void *noneOrUnused;
    void *noActiveEntityWithDefinition;
    void *noActiveEntityWithDefinitionAndClassCommandGroupA;
    void *noActiveEntityWithDefinitionAndRuntimeId;
    void *factionInactiveOrRelationAtLeast8;
    void *primaryResourceCurrentAtLeast;
    void *secondaryResourceCurrentAtLeast;
    void *activeArmyScaleValueAtLeast;
    void *matchingDefinitionAndRuntimeIdActiveEntityCountAtLeast;
    void *factionTerrainOccupancyMaskF9PercentAtLeast;
    void *countdownElapsed;
    void *primaryResourceLimitAtMost0FA0;
    void *noActiveEntityWithClassIdOutsideClassCommandGroupA;
    void *booleanPostfixExpression;
};

typedef struct ScenarioLevelDataPathTemplate24 ScenarioLevelDataPathTemplate24, *PScenarioLevelDataPathTemplate24;

struct ScenarioLevelDataPathTemplate24 {
    word prefixCodeUnits[11]; 
    union Utf16DecimalDigitPair4 decimalDigits; 
    word suffixCodeUnits[5]; 
};

typedef struct ModelRuntimeSlotSerializedScalarView200 ModelRuntimeSlotSerializedScalarView200, *PModelRuntimeSlotSerializedScalarView200;

typedef struct ModelRuntimeSlotClassStateSerializedScalar7C ModelRuntimeSlotClassStateSerializedScalar7C, *PModelRuntimeSlotClassStateSerializedScalar7C;

typedef struct ModelRuntimeAttachmentSerializedScalarDescriptor20 ModelRuntimeAttachmentSerializedScalarDescriptor20, *PModelRuntimeAttachmentSerializedScalarDescriptor20;

typedef dword EnergyDemandQ4;

struct ModelRuntimeAttachmentSerializedScalarDescriptor20 {
    ModelRuntimePoolRelativeOffset childModelRuntimeSavedOffset00; 
    struct ModelAttachmentTransformRecord *sourceTransform04; 
    ModelNodePoolRelativeOffset parentModelNodeSavedOffset08; 
    ModelChildNodeIndex childNodeIndex0C; 
    AngleTurn32 childLocalRotationAngle0; 
    AngleTurn32 childLocalRotationAngle1; 
    AngleTurn32 childLocalRotationAngle2; 
    dword reserved1C; 
};

struct ModelRuntimeSlotClassStateSerializedScalar7C {
    byte reserved84_A7[36]; 
    dword classStateA8; 
    dword classStateAC; 
    sdword classStateB0; 
    sdword classStateB4; 
    dword classStateB8; 
    byte reservedBC_CF[20]; 
    sdword classStateD0; 
    byte reservedD4_DB[8]; 
    dword classStateDC; 
    dword classStateE0; 
    dword enabledStateE4; 
    dword enabledStateE8; 
    dword classStateEC; 
    dword linkedArmyRuntimeSavedOffset; 
    EnergyDemandQ4 energyDemandQ4; 
    dword classStateF8; 
    dword classStateFC; 
};

struct ModelRuntimeSlotSerializedScalarView200 {
    enum PckModelDefinitionIdCatalog definitionSavedId; 
    dword rootModelNodeSavedOffset; 
    dword ownerArmyRuntimeSavedOffset; 
    dword attachmentCount0C; 
    byte reserved10_37[40]; 
    dword linkedModelRuntimeSavedOffset; 
    dword definitionValue60_3C; 
    dword definitionValue84_40; 
    dword definitionValue88_44; 
    dword definitionValue94_48; 
    dword definitionValue9C_4C; 
    dword definitionValueA4_50; 
    dword definitionValueAC_54; 
    dword definitionValueB4_58; 
    dword definitionValueBC_5C; 
    struct ModelRuntimeClassLinkState24 classLinkState; 
    struct ModelRuntimeSlotClassStateSerializedScalar7C classState; 
    byte reserved100_117[24]; 
    dword classState118; 
    dword classState11C; 
    byte reserved120_13F[32]; 
    struct ModelRuntimeAttachmentSerializedScalarDescriptor20 attachments140[6]; 
};

typedef struct EffectRuntimeSlotSerializedScalarView40 EffectRuntimeSlotSerializedScalarView40, *PEffectRuntimeSlotSerializedScalarView40;

struct EffectRuntimeSlotSerializedScalarView40 {
    enum PckEffectDefinitionIdCatalog definitionSavedId; 
    dword modelNodeSavedOffset; 
    EffectAnimationFrameCount animationFramesRemaining; 
    DefinitionReferencePresentFlag linkedEffectPresent; 
    DefinitionReferencePresentFlag linkedShotPresent; 
    struct EffectRuntimeLifecycleState10 lifecycleOwnerAndDefinition; 
    enum EffectCreationFlagBits creationFlags; 
    EffectShadingCountdownTicks shadingStartCountdownTicksRemaining; 
    EffectShadingCountdownTicks shadingStopCountdownTicksRemaining; 
    EffectPeriodicIntervalTicks periodicEffectCountdownTicks; 
    dword terrainRuntimeClassState; 
    PackedArgb32 stateTintArgb; 
    dword serializationToggleDword; 
};

typedef struct ResourceRegistrationRuntimeImageSerializedScalarViewDC ResourceRegistrationRuntimeImageSerializedScalarViewDC, *PResourceRegistrationRuntimeImageSerializedScalarViewDC;

typedef struct ResourceRegistrationRecordSerializedScalarView100 ResourceRegistrationRecordSerializedScalarView100, *PResourceRegistrationRecordSerializedScalarView100;

typedef struct ResourceRegistrationRecord100 ResourceRegistrationRecord100, *PResourceRegistrationRecord100;

typedef enum ResourceRegistrationDomainIndex {
    RESOURCE_DOMAIN_ARMY_RUNTIME=0,
    RESOURCE_DOMAIN_SHOT_RUNTIME=1,
    RESOURCE_DOMAIN_EFFECT_RUNTIME=2
} ResourceRegistrationDomainIndex;

typedef union ResourceRegistrationPointerOrSavedOffset4 ResourceRegistrationPointerOrSavedOffset4, *PResourceRegistrationPointerOrSavedOffset4;

struct ResourceRegistrationRecordSerializedScalarView100 {
    dword primarySavedIdOrOffset; 
    dword secondarySavedIdOrOffset; 
    dword ownerRuntimeSavedOffset; 
    byte reserved000C_002F[36]; 
    dword paletteAssetSavedIdOrOffset; 
    dword textureSetSavedIdOrOffset; 
    byte reserved0038_003F[8]; 
    dword spriteAssetSavedIdOrOffset; 
    byte reserved0044_0047[4]; 
    dword runtimePayloadSavedOffset; 
    enum RuntimeRegistrationRecordFlags flags; 
    byte reserved0050_005B[12]; 
    dword auxiliarySavedIdOrOffset; 
    byte reserved0060_00A3[68]; 
    enum ResourceRegistrationDomainIndex domainIndex; 
    byte reserved00A8_00C3[28]; 
    dword nestedBaseSavedOffset; 
    dword nestedCountC8; 
    dword nestedSavedOffsets13[13]; 
};

union ResourceRegistrationPointerOrSavedOffset4 {
    void *runtimePointer; 
    dword savedIdOrOffset; 
    dword raw; 
};

struct ResourceRegistrationRuntimeImageSerializedScalarViewDC {
    byte reserved0000_004F[80]; 
    dword levelRuntimeRecordIndex50; 
    byte reserved0054_0057[4]; 
    struct ResourceRegistrationRecordSerializedScalarView100 *records58; 
    byte reserved005C_00AB[80]; 
    dword recordCountAC; 
    byte reserved00B0_00D7[40]; 
    struct ResourceRegistrationRecord100 *tailRecordD8; 
};

struct ResourceRegistrationRecord100 {
    union ResourceRegistrationPointerOrSavedOffset4 primaryPointerOrSavedOffset; 
    union ResourceRegistrationPointerOrSavedOffset4 secondaryPointerOrSavedOffset; 
    union ResourceRegistrationPointerOrSavedOffset4 ownerRuntimeOrSavedOffset; 
    byte reserved000C_002F[36]; 
    struct GraphicsPaletteAsset *paletteAsset; 
    struct GraphicsTextureSet *textureSet; 
    byte reserved0038_003F[8]; 
    struct SpriteAssetHeader *spriteAsset; 
    byte reserved0044_0047[4]; 
    union ResourceRegistrationRuntimePayloadReference4 runtimePayload; 
    enum RuntimeRegistrationRecordFlags flags; 
    byte reserved0050_005B[12]; 
    union ResourceRegistrationPointerOrSavedOffset4 auxiliaryPointerOrSavedOffset; 
    byte reserved0060_00A3[68]; 
    enum ResourceRegistrationDomainIndex domainIndex; 
    byte reserved00A8_00C3[28]; 
    union ResourceRegistrationPointerOrSavedOffset4 nestedBasePointerOrSavedOffset; 
    dword nestedCountC8; 
    union ResourceRegistrationPointerOrSavedOffset4 nestedPointerOrOffsetArray13[13]; 
};

typedef struct ShotRuntimeSlotSerializedScalarView40 ShotRuntimeSlotSerializedScalarView40, *PShotRuntimeSlotSerializedScalarView40;

typedef struct ShotRuntimeOwnerAndTrajectoryState18 ShotRuntimeOwnerAndTrajectoryState18, *PShotRuntimeOwnerAndTrajectoryState18;

struct ShotRuntimeOwnerAndTrajectoryState18 {
    struct ArmyRuntimeSlot *ownerArmyRuntime; 
    Q12 directionComponent0Q12; 
    Q12 directionComponent1Q12; 
    Q12 directionComponent2Q12; 
    ShotAnimationFrameIndex animationFrameIndex; 
    dword definitionValue26C; 
};

struct ShotRuntimeSlotSerializedScalarView40 {
    enum PckShotDefinitionIdCatalog definitionSavedId; 
    Q12 launchSpeedQ12; 
    dword terrainRuntimeClassState; 
    ShotAnimationFrameAccumulatorQ4 animationFrameAccumulatorQ4; 
    dword modelNodeSavedOffset; 
    dword runtimeStateSavedOffset; 
    AngleTurn16Stored32 elevationOffsetAngle16; 
    ShotImpactEffectEmissionFlags impactEffectEmissionFlags; 
    ShotProjectileAgeTicks projectileAgeTicks; 
    ShotLifetimeRemainingTicks lifetimeTicksRemaining; 
    struct ShotRuntimeOwnerAndTrajectoryState18 ownerAndTrajectory; 
};

typedef struct ArmyArticulatedRuntimeSlotView ArmyArticulatedRuntimeSlotView, *PArmyArticulatedRuntimeSlotView;

struct ArmyArticulatedRuntimeSlotView {
    void *definitionOrAsset; 
    struct ModelRuntimeNode *modelNodeRuntime; 
    struct GameEntityRuntime *linkedEntityRuntime; 
    FactionRuntimeIndex factionIndex; 
    struct ArmyRuntimeMovementControlState movementControl; 
    ArmyMovementStateFlags movementStateFlags; 
    struct ArmyRuntimeSlot *commandTargetArmyRuntime; 
    Q12 commandCoordinate0Q12; 
    Q12 commandCoordinate1Q12; 
    Q12 commandCoordinate2Q12; 
    ArmyCommandModeFlags commandModeFlags; 
    ArmyCommandGeneration commandGeneration; 
    Q12 actionVector0Q12; 
    Q12 actionVector1Q12; 
    Q12 actionVector2Q12; 
    dword runtimeState40; 
    dword runtimeState44; 
    dword runtimeState48; 
    dword runtimeState4C; 
    byte reserved50_57[8]; 
    Q12 movementPosition0Q12; 
    Q12 movementPosition1Q12; 
    dword classState60; 
    dword ownerValue64; 
    dword ownerValue68; 
    struct ArmyRuntimeSlot *linkedArmyRuntimeOrSavedOffset; 
    Q12 fallbackWorldYQ12; 
    Q12 fallbackWorldXQ12; 
    Q12 movementTarget0Q12; 
    Q12 movementTarget1Q12; 
    dword definitionClassValue80; 
    dword definitionClassValue84; 
    dword definitionClassValue88; 
    dword runtimeState8C; 
    dword runtimeState90; 
    dword runtimeState94; 
    dword runtimeState98; 
    int articulatedCoordinateOrState9C; 
    int articulatedHeightOrStateA0; 
    dword runtimeStateA4; 
    dword runtimeStateA8; 
    struct ArmyRuntimeArticulatedContactState14 articulatedContact; 
    struct ArmyRuntimeLinkedChildOverloadedState10 linkedChildOverloadedState; 
    struct ArmyRuntimeLinkedChildSpawnParameters0C linkedChildSpawnParameters; 
    struct ArmyRuntimeLinkedChildPendingCounts4 linkedChildPendingCounts; 
    byte reservedE0_EB[12]; 
    ArmyRuntimeFlags runtimeFlags; 
    struct ArmyRuntimeSlot *linkedArmyRuntime; 
    ArmyRuntimeTimer runtimeTimer; 
    byte reservedF8_FF[8]; 
    int stateOrTechnologyId; 
    dword runtimeState104; 
    ArmySelectionMetric selectionMetric0; 
    ArmySelectionMetric selectionMetric1; 
    ArmySelectionMetric selectionMetric2; 
    ArmySelectionMetric selectionMetric3; 
    ArmySelectionMetric selectionMetric4; 
    ArmySelectionMetric selectionMetric5; 
};

typedef struct ArmyRuntimeLinkedChildMaskSlotView ArmyRuntimeLinkedChildMaskSlotView, *PArmyRuntimeLinkedChildMaskSlotView;

typedef struct ArmyRuntimeLinkedChildMaskArticulatedContactState14 ArmyRuntimeLinkedChildMaskArticulatedContactState14, *PArmyRuntimeLinkedChildMaskArticulatedContactState14;

typedef struct ArmyRuntimeLinkedChildSlotMaskState4 ArmyRuntimeLinkedChildSlotMaskState4, *PArmyRuntimeLinkedChildSlotMaskState4;

struct ArmyRuntimeLinkedChildSlotMaskState4 {
    dword linkedChildSlotMask; 
};

struct ArmyRuntimeLinkedChildMaskArticulatedContactState14 {
    enum ArmyTerrainContactDispatchMode terrainContactMode; 
    Q12 lateralOffsetQ12; 
    struct ArmyRuntimeLinkedChildSlotMaskState4 linkedChildSlotMaskState; 
    Q12 fallbackPosition0Q12; 
    Q12 fallbackPosition1Q12; 
};

struct ArmyRuntimeLinkedChildMaskSlotView {
    void *definitionOrAsset; 
    struct ModelRuntimeNode *modelNodeRuntime; 
    struct GameEntityRuntime *linkedEntityRuntime; 
    FactionRuntimeIndex factionIndex; 
    struct ArmyRuntimeMovementControlState movementControl; 
    ArmyMovementStateFlags movementStateFlags; 
    struct ArmyRuntimeSlot *commandTargetArmyRuntime; 
    Q12 commandCoordinate0Q12; 
    Q12 commandCoordinate1Q12; 
    Q12 commandCoordinate2Q12; 
    ArmyCommandModeFlags commandModeFlags; 
    ArmyCommandGeneration commandGeneration; 
    Q12 actionVector0Q12; 
    Q12 actionVector1Q12; 
    Q12 actionVector2Q12; 
    dword runtimeState40; 
    dword runtimeState44; 
    dword runtimeState48; 
    dword runtimeState4C; 
    byte reserved50_57[8]; 
    Q12 movementPosition0Q12; 
    Q12 movementPosition1Q12; 
    dword classState60; 
    dword ownerValue64; 
    dword ownerValue68; 
    struct ArmyRuntimeSlot *linkedArmyRuntimeOrSavedOffset; 
    Q12 fallbackWorldYQ12; 
    Q12 fallbackWorldXQ12; 
    Q12 movementTarget0Q12; 
    Q12 movementTarget1Q12; 
    dword definitionClassValue80; 
    dword definitionClassValue84; 
    dword definitionClassValue88; 
    dword runtimeState8C; 
    dword runtimeState90; 
    dword runtimeState94; 
    dword runtimeState98; 
    enum ModelRuntimeClassId depthBinClass; 
    enum PckArmyAssetIdCatalog armyAssetId; 
    dword runtimeStateA4; 
    dword runtimeStateA8; 
    struct ArmyRuntimeLinkedChildMaskArticulatedContactState14 articulatedContact; 
    struct ArmyRuntimeLinkedChildOverloadedState10 linkedChildOverloadedState; 
    struct ArmyRuntimeLinkedChildSpawnParameters0C linkedChildSpawnParameters; 
    struct ArmyRuntimeLinkedChildPendingCounts4 linkedChildPendingCounts; 
    byte reservedE0_EB[12]; 
    ArmyRuntimeFlags runtimeFlags; 
    struct ArmyRuntimeSlot *linkedArmyRuntime; 
    ArmyRuntimeTimer runtimeTimer; 
    byte reservedF8_FF[8]; 
    int stateOrTechnologyId; 
    dword runtimeState104; 
    ArmySelectionMetric selectionMetric0; 
    ArmySelectionMetric selectionMetric1; 
    ArmySelectionMetric selectionMetric2; 
    ArmySelectionMetric selectionMetric3; 
    ArmySelectionMetric selectionMetric4; 
    ArmySelectionMetric selectionMetric5; 
};

typedef struct ArmyRuntimeMovementCountdownSlotView ArmyRuntimeMovementCountdownSlotView, *PArmyRuntimeMovementCountdownSlotView;

typedef struct GameEntityMovementCountdownRuntime GameEntityMovementCountdownRuntime, *PGameEntityMovementCountdownRuntime;

typedef struct GameEntityMovementCountdownRuntimeCommon GameEntityMovementCountdownRuntimeCommon, *PGameEntityMovementCountdownRuntimeCommon;

typedef struct GameEntityMovementCountdownDamageState2C GameEntityMovementCountdownDamageState2C, *PGameEntityMovementCountdownDamageState2C;

struct ArmyRuntimeMovementCountdownSlotView {
    void *definitionOrAsset; 
    struct ModelRuntimeNode *modelNodeRuntime; 
    struct GameEntityMovementCountdownRuntime *linkedEntityRuntime; 
    FactionRuntimeIndex factionIndex; 
    struct ArmyRuntimeMovementControlState movementControl; 
    ArmyMovementStateFlags movementStateFlags; 
    struct ArmyRuntimeSlot *commandTargetArmyRuntime; 
    Q12 commandCoordinate0Q12; 
    Q12 commandCoordinate1Q12; 
    Q12 commandCoordinate2Q12; 
    ArmyCommandModeFlags commandModeFlags; 
    ArmyCommandGeneration commandGeneration; 
    Q12 actionVector0Q12; 
    Q12 actionVector1Q12; 
    Q12 actionVector2Q12; 
    dword runtimeState40; 
    dword runtimeState44; 
    dword runtimeState48; 
    dword runtimeState4C; 
    byte reserved50_57[8]; 
    Q12 movementPosition0Q12; 
    Q12 movementPosition1Q12; 
    dword classState60; 
    dword ownerValue64; 
    dword ownerValue68; 
    struct ArmyRuntimeSlot *linkedArmyRuntimeOrSavedOffset; 
    Q12 fallbackWorldYQ12; 
    Q12 fallbackWorldXQ12; 
    Q12 movementTarget0Q12; 
    Q12 movementTarget1Q12; 
    dword definitionClassValue80; 
    dword definitionClassValue84; 
    dword definitionClassValue88; 
    dword runtimeState8C; 
    dword runtimeState90; 
    dword runtimeState94; 
    dword runtimeState98; 
    enum ModelRuntimeClassId depthBinClass; 
    enum PckArmyAssetIdCatalog armyAssetId; 
    dword runtimeStateA4; 
    dword runtimeStateA8; 
    struct ArmyRuntimeArticulatedContactState14 articulatedContact; 
    struct ArmyRuntimeLinkedChildOverloadedState10 linkedChildOverloadedState; 
    struct ArmyRuntimeLinkedChildSpawnParameters0C linkedChildSpawnParameters; 
    struct ArmyRuntimeLinkedChildPendingCounts4 linkedChildPendingCounts; 
    byte reservedE0_EB[12]; 
    ArmyRuntimeFlags runtimeFlags; 
    struct ArmyRuntimeSlot *linkedArmyRuntime; 
    ArmyRuntimeTimer runtimeTimer; 
    byte reservedF8_FF[8]; 
    int stateOrTechnologyId; 
    dword runtimeState104; 
    ArmySelectionMetric selectionMetric0; 
    ArmySelectionMetric selectionMetric1; 
    ArmySelectionMetric selectionMetric2; 
    ArmySelectionMetric selectionMetric3; 
    ArmySelectionMetric selectionMetric4; 
    ArmySelectionMetric selectionMetric5; 
};

struct GameEntityMovementCountdownDamageState2C {
    dword reserved00; 
    int countdownOrState; 
    int remainingIntegrity; 
    byte reserved0C_2B[32]; 
};

struct GameEntityMovementCountdownRuntimeCommon {
    struct GameEntityOwnershipState10 ownership; 
    byte reserved10_17[8]; 
    GameEntityCommandFlags commandFlags; 
    struct GameEntityCommandTargetState commandTarget; 
    struct GameEntityMovementCountdownDamageState2C damageState; 
    Q12 selectionOffsetXQ12; 
    Q12 selectionOffsetYQ12; 
    byte reserved68_9F[56]; 
    RuntimeToken runtimeIdentityOrArmyAssetId; 
    byte reservedA4_EB[72]; 
    GameEntityRuntimeFlags runtimeFlags; 
    struct GameEntityPathingAndImpactState10 pathingAndImpactState; 
    GameEntityCommandState commandState; 
};

struct GameEntityMovementCountdownRuntime {
    struct GameEntityMovementCountdownRuntimeCommon common; 
    union GameEntityRuntimeClassPayload classPayload; 
};

typedef struct ArmyGraphicsBinding ArmyGraphicsBinding, *PArmyGraphicsBinding;

struct ArmyGraphicsBinding {
    void *textureSet; 
    struct GraphicsPaletteAsset *paletteAsset; 
};

typedef struct ArmyLinkedChildDispatchCallerFrame28 ArmyLinkedChildDispatchCallerFrame28, *PArmyLinkedChildDispatchCallerFrame28;

struct ArmyLinkedChildDispatchCallerFrame28 {
    byte reserved00_0B[12]; 
    dword preservedReturnLow0C; 
    byte reserved10_17[8]; 
    dword preservedReturnHigh18; 
    dword reserved1C; 
    struct WorldRuntimeContext *worldRuntime; 
    struct ArmyRuntimeSlot *armyRuntime; 
};

typedef struct ArmyRuntimeWaypointCoordinateSlotView ArmyRuntimeWaypointCoordinateSlotView, *PArmyRuntimeWaypointCoordinateSlotView;

typedef struct ArmyRuntimeWaypointCoordinateState10 ArmyRuntimeWaypointCoordinateState10, *PArmyRuntimeWaypointCoordinateState10;

struct ArmyRuntimeWaypointCoordinateState10 {
    Q12 primaryCoordinateOrTargetQ12; 
    Q12 secondaryCoordinateOrTargetQ12; 
    union ArmyRuntimeCoordinateCommandOrHistoryValue4 leftHeadingCommandOrSpawnValue; 
    union ArmyRuntimeCoordinateCommandOrHistoryValue4 rightHeadingCommandOrSpawnValue; 
};

struct ArmyRuntimeWaypointCoordinateSlotView {
    void *definitionOrAsset; 
    struct ModelRuntimeNode *modelNodeRuntime; 
    struct GameEntityRuntime *linkedEntityRuntime; 
    FactionRuntimeIndex factionIndex; 
    struct ArmyRuntimeMovementControlState movementControl; 
    ArmyMovementStateFlags movementStateFlags; 
    struct ArmyRuntimeSlot *commandTargetArmyRuntime; 
    Q12 commandCoordinate0Q12; 
    Q12 commandCoordinate1Q12; 
    Q12 commandCoordinate2Q12; 
    ArmyCommandModeFlags commandModeFlags; 
    ArmyCommandGeneration commandGeneration; 
    Q12 actionVector0Q12; 
    Q12 actionVector1Q12; 
    Q12 actionVector2Q12; 
    dword runtimeState40; 
    dword runtimeState44; 
    dword runtimeState48; 
    dword runtimeState4C; 
    byte reserved50_57[8]; 
    Q12 movementPosition0Q12; 
    Q12 movementPosition1Q12; 
    dword classState60; 
    dword ownerValue64; 
    dword ownerValue68; 
    struct ArmyRuntimeSlot *linkedArmyRuntimeOrSavedOffset; 
    Q12 fallbackWorldYQ12; 
    Q12 fallbackWorldXQ12; 
    Q12 movementTarget0Q12; 
    Q12 movementTarget1Q12; 
    dword definitionClassValue80; 
    dword definitionClassValue84; 
    dword definitionClassValue88; 
    dword runtimeState8C; 
    dword runtimeState90; 
    dword runtimeState94; 
    dword runtimeState98; 
    enum ModelRuntimeClassId depthBinClass; 
    enum PckArmyAssetIdCatalog armyAssetId; 
    dword runtimeStateA4; 
    dword runtimeStateA8; 
    struct ArmyRuntimeArticulatedContactState14 articulatedContact; 
    struct ArmyRuntimeWaypointCoordinateState10 linkedChildWaypointState; 
    struct ArmyRuntimeLinkedChildSpawnParameters0C linkedChildSpawnParameters; 
    struct ArmyRuntimeLinkedChildPendingCounts4 linkedChildPendingCounts; 
    byte reservedE0_EB[12]; 
    ArmyRuntimeFlags runtimeFlags; 
    struct ArmyRuntimeSlot *linkedArmyRuntime; 
    ArmyRuntimeTimer runtimeTimer; 
    byte reservedF8_FF[8]; 
    int stateOrTechnologyId; 
    dword runtimeState104; 
    ArmySelectionMetric selectionMetric0; 
    ArmySelectionMetric selectionMetric1; 
    ArmySelectionMetric selectionMetric2; 
    ArmySelectionMetric selectionMetric3; 
    ArmySelectionMetric selectionMetric4; 
    ArmySelectionMetric selectionMetric5; 
};

typedef struct ArmyTerrainContactDispatchTable2 ArmyTerrainContactDispatchTable2, *PArmyTerrainContactDispatchTable2;

struct ArmyTerrainContactDispatchTable2 {
    void *acquireOrInitializeContactSlotEntry; 
    void *advanceActiveContactAndReleaseEntry; 
};

typedef struct ArmyRuntimeLinkedChildDefinitionState10 ArmyRuntimeLinkedChildDefinitionState10, *PArmyRuntimeLinkedChildDefinitionState10;

struct ArmyRuntimeLinkedChildDefinitionState10 {
    int definitionValueC0; 
    Q12 secondaryPosition0Q12; 
    AngleTurn32 leftContactHeading; 
    AngleTurn32 rightContactHeading; 
};

typedef qword ArmyLinkedChildDispatchPreservedRegisterPair8;

typedef dword ArmyWaypointCount;

typedef struct ArmyMovementRuntime ArmyMovementRuntime, *PArmyMovementRuntime;

typedef dword ArmyMovementRetryCountdown;

typedef struct WorldPointXYQ12 WorldPointXYQ12, *PWorldPointXYQ12;

struct WorldPointXYQ12 {
    Q12 worldXQ12; 
    Q12 worldYQ12; 
};

struct ArmyMovementRuntime {
    struct GameEntityRuntime *entityRuntime; 
    struct ModelRuntimeNode *modelNodeRuntime; 
    struct GameEntityRuntime *linkedEntityRuntime; 
    FactionRuntimeIndex factionIndex; 
    struct ArmyRuntimeMovementControlState movementControl; 
    ArmyMovementStateFlags movementStateFlags; 
    struct ArmyRuntimeSlot *commandTargetArmyRuntime; 
    Q12 commandCoordinate0Q12; 
    Q12 commandCoordinate1Q12; 
    Q12 commandCoordinate2Q12; 
    ArmyCommandModeFlags commandModeFlags; 
    ArmyCommandGeneration commandGeneration; 
    Q12 actionVector0Q12; 
    Q12 actionVector1Q12; 
    Q12 actionVector2Q12; 
    dword runtimeState40; 
    dword runtimeState44; 
    dword runtimeState48; 
    dword runtimeState4C; 
    byte reserved50_57[8]; 
    Q12 movementWorldXQ12; 
    Q12 movementWorldYQ12; 
    dword classState60; 
    dword ownerValue64; 
    dword ownerValue68; 
    struct ArmyRuntimeSlot *linkedArmyRuntimeOrSavedOffset; 
    Q12 lastCheckedWorldXQ12; 
    Q12 lastCheckedWorldYQ12; 
    Q12 movementTargetWorldXQ12; 
    Q12 movementTargetWorldYQ12; 
    dword definitionClassValue80; 
    dword definitionClassValue84; 
    dword definitionClassValue88; 
    dword runtimeState8C; 
    dword runtimeState90; 
    dword runtimeState94; 
    dword runtimeState98; 
    enum ModelRuntimeClassId depthBinClass; 
    enum PckArmyAssetIdCatalog armyAssetId; 
    ArmyMovementRetryCountdown retryCountdown; 
    ArmyWaypointCount queuedWaypointCount; 
    byte unresolvedMovementPrefix[12]; 
    struct WorldPointXYQ12 fallbackPosition; 
    struct WorldPointXYQ12 queuedWaypoints[8]; 
    int stateOrTechnologyId; 
    dword runtimeState104; 
    ArmySelectionMetric selectionMetric0; 
    ArmySelectionMetric selectionMetric1; 
    ArmySelectionMetric selectionMetric2; 
    ArmySelectionMetric selectionMetric3; 
    ArmySelectionMetric selectionMetric4; 
    ArmySelectionMetric selectionMetric5; 
};

typedef struct ArmyPlacementAssetClassDispatchCallbackTable24 ArmyPlacementAssetClassDispatchCallbackTable24, *PArmyPlacementAssetClassDispatchCallbackTable24;

struct ArmyPlacementAssetClassDispatchCallbackTable24 {
    void (*callbacks[24])(dword, dword, dword, dword, Q12, Q12, struct ModelDefinitionRecordPrefix *, dword, struct WorldRuntimeContext *); 
};

typedef struct InGameFieldImageSaveContext58 InGameFieldImageSaveContext58, *PInGameFieldImageSaveContext58;

struct InGameFieldImageSaveContext58 {
    byte opaqueRuntimePrefix00_53[84]; 
    struct FieldGridAsset *fieldGridAsset; 
};

typedef struct RuntimeClassFamilyMap24 RuntimeClassFamilyMap24, *PRuntimeClassFamilyMap24;

typedef enum RuntimeClassFamilyProfile {
    RUNTIME_CLASS_PROFILE_CLASS00=0,
    RUNTIME_CLASS_PROFILE_CLASSES01_02_03_17_18_19=1,
    RUNTIME_CLASS_PROFILE_CLASSES05_06_07_08_09_21=2,
    RUNTIME_CLASS_PROFILE_CLASSES04_10_11_15_16_20_22=3,
    RUNTIME_CLASS_PROFILE_CLASS12=4,
    RUNTIME_CLASS_PROFILE_CLASS13=5,
    RUNTIME_CLASS_PROFILE_CLASS14=6,
    RUNTIME_CLASS_PROFILE_CLASS23=7
} RuntimeClassFamilyProfile;

struct RuntimeClassFamilyMap24 {
    enum RuntimeClassFamilyProfile class00; 
    enum RuntimeClassFamilyProfile class01; 
    enum RuntimeClassFamilyProfile class02; 
    enum RuntimeClassFamilyProfile class03; 
    enum RuntimeClassFamilyProfile class04; 
    enum RuntimeClassFamilyProfile class05; 
    enum RuntimeClassFamilyProfile class06; 
    enum RuntimeClassFamilyProfile class07; 
    enum RuntimeClassFamilyProfile class08; 
    enum RuntimeClassFamilyProfile class09; 
    enum RuntimeClassFamilyProfile class10; 
    enum RuntimeClassFamilyProfile class11; 
    enum RuntimeClassFamilyProfile class12; 
    enum RuntimeClassFamilyProfile class13; 
    enum RuntimeClassFamilyProfile class14; 
    enum RuntimeClassFamilyProfile class15; 
    enum RuntimeClassFamilyProfile class16; 
    enum RuntimeClassFamilyProfile class17; 
    enum RuntimeClassFamilyProfile class18; 
    enum RuntimeClassFamilyProfile class19; 
    enum RuntimeClassFamilyProfile class20; 
    enum RuntimeClassFamilyProfile class21; 
    enum RuntimeClassFamilyProfile class22; 
    enum RuntimeClassFamilyProfile class23; 
};

typedef struct ArmyRuntimeClassCallbackPartitions ArmyRuntimeClassCallbackPartitions, *PArmyRuntimeClassCallbackPartitions;

struct ArmyRuntimeClassCallbackPartitions {
    void (*runtimeUpdate[24])(struct WorldRuntimeContext *, struct ArmyRuntimeSlot *); 
    void (*classMethodD[24])(struct WorldRuntimeContext *, struct ArmyRuntimeSlot *); 
    void (*modelUnrebase[24])(struct ModelRuntimeSlot *); 
};

typedef struct RuntimeMaintenanceObjectCallbacks RuntimeMaintenanceObjectCallbacks, *PRuntimeMaintenanceObjectCallbacks;

typedef union RuntimeMaintenanceObjectReference4 RuntimeMaintenanceObjectReference4, *PRuntimeMaintenanceObjectReference4;

struct RuntimeMaintenanceObjectCallbacks {
    void (*army)(struct WorldRuntimeContext *, union RuntimeMaintenanceObjectReference4); 
    void (*shot)(struct WorldRuntimeContext *, union RuntimeMaintenanceObjectReference4); 
    void (*effect)(struct WorldRuntimeContext *, union RuntimeMaintenanceObjectReference4); 
};

union RuntimeMaintenanceObjectReference4 {
    struct ModelRuntimeNode *modelNodeRuntime; 
    struct WorldRuntimeNode *worldNode; 
    struct ArmyRuntimeSlot *armyRuntime; 
    struct ShotRuntimeSlot *shotRuntime; 
    struct EffectRuntimeSlot *effectRuntime; 
    void *opaqueRuntime; 
};

typedef struct RuntimeMaintenanceAudioRefreshCallbacks RuntimeMaintenanceAudioRefreshCallbacks, *PRuntimeMaintenanceAudioRefreshCallbacks;

struct RuntimeMaintenanceAudioRefreshCallbacks {
    void (*army)(struct WorldRuntimeContext *, struct ArmyRuntimeSlot *); 
    void (*shot)(struct WorldRuntimeContext *, struct ModelRuntimeNode *); 
    void (*effect)(struct WorldRuntimeContext *, void *); 
};

typedef struct RuntimeMaintenancePrimaryUpdateCallbacks RuntimeMaintenancePrimaryUpdateCallbacks, *PRuntimeMaintenancePrimaryUpdateCallbacks;

struct RuntimeMaintenancePrimaryUpdateCallbacks {
    void (*army)(struct WorldRuntimeContext *, struct WorldRuntimeNode *); 
    void (*shot)(struct WorldRuntimeContext *, struct ModelRuntimeNode *); 
    void (*effect)(struct WorldRuntimeContext *, struct ModelRuntimeNode *); 
};

typedef struct RuntimeMaintenanceTerrainStateRefreshCallbacks RuntimeMaintenanceTerrainStateRefreshCallbacks, *PRuntimeMaintenanceTerrainStateRefreshCallbacks;

struct RuntimeMaintenanceTerrainStateRefreshCallbacks {
    void (*army)(struct WorldRuntimeContext *, struct ModelRuntimeNode *); 
    void (*shot)(struct WorldRuntimeContext *, struct ModelRuntimeNode *); 
    void (*effect)(struct WorldRuntimeContext *, struct ModelRuntimeNode *); 
};

typedef struct RuntimeMaintenanceTargetTypeDescriptor8 RuntimeMaintenanceTargetTypeDescriptor8, *PRuntimeMaintenanceTargetTypeDescriptor8;

typedef enum RuntimeMaintenancePhaseIndex {
    PRIMARY_UPDATE=0,
    TERRAIN_STATE_REFRESH=1,
    OCCUPANCY_REBUILD=2,
    AUDIO_REFRESH=3
} RuntimeMaintenancePhaseIndex;

typedef enum RuntimeMaintenanceDomainIndex {
    ARMY=0,
    SHOT=1,
    EFFECT=2
} RuntimeMaintenanceDomainIndex;

typedef enum RuntimeMaintenanceSecondArgumentKind {
    WORLD_NODE=0,
    MODEL_NODE=1,
    ARMY_RUNTIME=2,
    OPAQUE=3
} RuntimeMaintenanceSecondArgumentKind;

struct RuntimeMaintenanceTargetTypeDescriptor8 {
    enum RuntimeMaintenancePhaseIndex phase; 
    enum RuntimeMaintenanceDomainIndex domain; 
    enum RuntimeMaintenanceSecondArgumentKind secondArgumentKind; 
    byte reserved03; 
    void *callbackTarget; 
};

typedef struct ArmyPlacementClassCallbackPartitions ArmyPlacementClassCallbackPartitions, *PArmyPlacementClassCallbackPartitions;

struct ArmyPlacementClassCallbackPartitions {
    void (*modelRelease[24])(struct ModelDefinitionRecordPrefix *, struct ModelRuntimeSlot *); 
    void (*placementValidation[24])(struct WorldRuntimeContext *, struct ArmyRuntimeSlot *); 
};

typedef struct ArmyRuntimeClassCommandTable ArmyRuntimeClassCommandTable, *PArmyRuntimeClassCommandTable;

struct ArmyRuntimeClassCommandTable {
    void (*callbacks[24])(struct WorldRuntimeContext *, struct ArmyRuntimeSlot *); 
};

typedef struct RuntimeMaintenanceCallbackPhasesTyped RuntimeMaintenanceCallbackPhasesTyped, *PRuntimeMaintenanceCallbackPhasesTyped;

typedef struct RuntimeMaintenanceOccupancyRebuildCallbacks RuntimeMaintenanceOccupancyRebuildCallbacks, *PRuntimeMaintenanceOccupancyRebuildCallbacks;

struct RuntimeMaintenanceOccupancyRebuildCallbacks {
    void (*army)(struct WorldRuntimeContext *, struct WorldRuntimeNode *); 
    void (*shot)(struct WorldRuntimeContext *, void *); 
    void (*effect)(struct WorldRuntimeContext *, void *); 
};

struct RuntimeMaintenanceCallbackPhasesTyped {
    struct RuntimeMaintenancePrimaryUpdateCallbacks primaryUpdate; 
    struct RuntimeMaintenanceTerrainStateRefreshCallbacks terrainStateRefresh; 
    struct RuntimeMaintenanceOccupancyRebuildCallbacks occupancyRebuild; 
    struct RuntimeMaintenanceAudioRefreshCallbacks audioRefresh; 
};

typedef struct RuntimeMaintenanceCallbackPhases RuntimeMaintenanceCallbackPhases, *PRuntimeMaintenanceCallbackPhases;

struct RuntimeMaintenanceCallbackPhases {
    struct RuntimeMaintenanceObjectCallbacks primaryUpdate; 
    struct RuntimeMaintenanceObjectCallbacks terrainStateRefresh; 
    struct RuntimeMaintenanceObjectCallbacks occupancyRebuild; 
    struct RuntimeMaintenanceObjectCallbacks audioRefresh; 
};

typedef struct RuntimeMaintenanceTargetTypeMatrix4x3 RuntimeMaintenanceTargetTypeMatrix4x3, *PRuntimeMaintenanceTargetTypeMatrix4x3;

struct RuntimeMaintenanceTargetTypeMatrix4x3 {
    struct RuntimeMaintenanceTargetTypeDescriptor8 slot0; 
    struct RuntimeMaintenanceTargetTypeDescriptor8 slot1; 
    struct RuntimeMaintenanceTargetTypeDescriptor8 slot2; 
    struct RuntimeMaintenanceTargetTypeDescriptor8 slot3; 
    struct RuntimeMaintenanceTargetTypeDescriptor8 slot4; 
    struct RuntimeMaintenanceTargetTypeDescriptor8 slot5; 
    struct RuntimeMaintenanceTargetTypeDescriptor8 slot6; 
    struct RuntimeMaintenanceTargetTypeDescriptor8 slot7; 
    struct RuntimeMaintenanceTargetTypeDescriptor8 slot8; 
    struct RuntimeMaintenanceTargetTypeDescriptor8 slot9; 
    struct RuntimeMaintenanceTargetTypeDescriptor8 slot10; 
    struct RuntimeMaintenanceTargetTypeDescriptor8 slot11; 
};

typedef struct ModelRuntimeSlotPointerRebaseCallbackTable24 ModelRuntimeSlotPointerRebaseCallbackTable24, *PModelRuntimeSlotPointerRebaseCallbackTable24;

struct ModelRuntimeSlotPointerRebaseCallbackTable24 {
    void (*callbacks[24])(struct ModelRuntimeSlot *); 
};

typedef struct ModelRuntimeSlotClassInitializeCallbackTable24 ModelRuntimeSlotClassInitializeCallbackTable24, *PModelRuntimeSlotClassInitializeCallbackTable24;

struct ModelRuntimeSlotClassInitializeCallbackTable24 {
    void (*callbacks[24])(struct ModelDefinitionRecordPrefix *, struct ModelRuntimeSlot *); 
};

typedef struct NetworkBackendInstanceDescriptorPrefix NetworkBackendInstanceDescriptorPrefix, *PNetworkBackendInstanceDescriptorPrefix;

struct NetworkBackendInstanceDescriptorPrefix {
    dword backendId00; 
    dword backendFlags04; 
    dword backendState08; 
    dword backendState0C; 
    word displayNameUtf16[20]; 
};

typedef struct PersistentSettingsRuntime PersistentSettingsRuntime, *PPersistentSettingsRuntime;

typedef struct PersistentSettingsImage PersistentSettingsImage, *PPersistentSettingsImage;

typedef enum PersistentSoundOptionFlags {
    SOUND_OPTIONS_EFFECTS_ENABLED=1,
    SOUND_OPTIONS_MUSIC_ENABLED=2,
    SOUND_OPTIONS_REVERSE_STEREO=4
} PersistentSoundOptionFlags;

typedef enum PersistentTextureQualityLevel {
    TEXTURE_QUALITY_HIGH=0,
    TEXTURE_QUALITY_MEDIUM=1,
    TEXTURE_QUALITY_LOW=2
} PersistentTextureQualityLevel;

struct PersistentSettingsRuntime {
    struct PersistentSettingsImage *image; 
    PersistentSettingsByteCount loadedByteCount; 
    PersistentSettingsMutationCount dirtyWriteCount; 
    word path[256]; 
};

struct PersistentSettingsImage {
    PersistentDisplayAdapterIndex graphicsAdapterIndex; 
    PersistentDisplayDimensionPixels displayWidth; 
    PersistentDisplayDimensionPixels displayHeight; 
    PersistentColorDepthBits displayBitsPerPixel; 
    PersistentShadingGridHalfSize shadingGridHalfSize; 
    PersistentShadingTextureDimension shadingTextureDimension; 
    PersistentShadingSubresourceCount shadingTextureSubresourceCount; 
    enum PersistentToggleState shadingEnabled; 
    enum PersistentSoundOptionFlags soundOptionFlags; 
    SpatialSoundGainQ15 soundEffectsGainQ15; 
    MovieAudioGainQ15 movieDefaultAudioGainQ15; 
    SpatialSoundGainQ15 musicGainQ15; 
    enum PersistentTextureQualityLevel textureQualityLevel; 
    PersistentModelLodDepthQ8 modelLodDepthThresholdQ8; 
    LocaleCountryCode localeCountryCodeOverride; 
    NetworkPlayerCount networkPlayerCount; 
    enum PersistentMapMouseOptionFlags mapMouseOptionFlags; 
    GameSpeedPercent gameSpeedPercent; 
    CameraScrollStepPixels cameraScrollStep; 
    MovieAudioGainQ15 movieAlternateAudioGainQ15; 
    byte reserved50_5B[12]; 
    enum PersistentMouseLinkPanelOptionFlags mouseLinkPanelOptionFlags; 
    word playerName[20]; 
    word gameName[20]; 
    byte reservedB0_C7[24]; 
};

typedef struct AiKnowledgeParameters AiKnowledgeParameters, *PAiKnowledgeParameters;

struct AiKnowledgeParameters {
    dword resource136DeficitScoreNumerator; 
    dword specialSite14aBaseWeight; 
    dword specialSite14cBaseWeight; 
    dword structure14bBaseWeight; 
    dword structure14dBaseWeight; 
    dword workspace08Id14aDerivedWeight; 
    dword workspace08OtherDerivedWeight; 
    dword strategicClass12dBaseWeight; 
    dword strategicClass12fTo132BaseWeight; 
    dword armyVariantABaseWeight; 
    dword armyVariantBUnexploredTerrainWeightCoefficient; 
    dword armyVariantCBaseWeight; 
    dword strategicClass141To143BaseWeight; 
    dword workspace12BestCandidateBaseWeight; 
    dword unknownParameterDwords14_15[2]; 
    dword strategic12dAnd141To143AdditionalPlanningCapacity; 
    dword strategic12fTo132AdditionalPlanningCapacity; 
    dword unknownParameterDword18; 
    dword resource136DeficitScoreDenominator; 
    dword structure14bPrerequisite14aCountLimit; 
    dword structure14dPrerequisite14cCountLimit; 
    dword structure14bCountGapLimit; 
    dword structure14dCountGapLimit; 
    dword unknownParameterDwords24_27[4]; 
    dword specialSiteSeparationQuantumQ12; 
    dword unknownParameterDwords29_31[3]; 
    dword generalSiteMinimumAxisSeparationQ12; 
    dword unknownParameterDword33; 
    dword generalSiteSecondaryDistanceCapQ12; 
    dword workspace05DistanceBiasQ12; 
    dword unknownParameterDwords36_37[2]; 
    dword generalSiteSecondaryDistanceCoefficient; 
    dword workspace05DistanceScaleQ12; 
    dword unknownParameterDwords40_47[8]; 
    dword flaggedSiteMinimumAxisSeparationQ12; 
    dword unknownParameterDwords49_50[2]; 
    dword factionAnchorDistanceBiasQ12; 
    dword unknownParameterDwords52_54[3]; 
    dword factionAnchorDistanceScaleQ12; 
    dword unknownParameterDwords56_66[11]; 
    dword secondaryWorkspaceDistanceBiasQ12; 
    dword unknownParameterDwords68_70[3]; 
    dword secondaryWorkspaceDistanceScaleQ12; 
    dword unknownParameterDwords72_79[8]; 
    dword terrainFeatureMinimumAxisSeparationQ12; 
    dword placementClearancePaddingQ12; 
    dword specialClass12SecondaryWorkspaceDistanceThresholdQ12; 
    dword specialClass12EntityDistanceBiasQ12; 
    dword specialClass12Workspace02NearDistanceThresholdQ12; 
    dword specialClass12Workspace08Field0cCoefficient; 
    dword specialClass12SecondaryWorkspaceShortfallCoefficient; 
    dword specialClass12EntityDistanceCoefficient; 
    dword specialClass12Workspace02NearDistanceCoefficient; 
    dword specialSiteMinimumWorkspaceDistanceQ12; 
    dword unknownParameterDwords90_119[30]; 
    dword strategicClass141Weight; 
    dword strategicClass142Weight; 
    dword strategicClass143Weight; 
    dword unknownParameterDwords123_127[5]; 
};

typedef struct AiKnowledgeDataImage AiKnowledgeDataImage, *PAiKnowledgeDataImage;

struct AiKnowledgeDataImage {
    struct AiKnowledgeParameters parameters; 
};

typedef struct AiScoredSiteWorkspaceEntry AiScoredSiteWorkspaceEntry, *PAiScoredSiteWorkspaceEntry;

struct AiScoredSiteWorkspaceEntry {
    Q12 cellWorldXQ12; 
    Q12 cellWorldYQ12; 
    int score; 
    struct FieldGridCell *cell; 
};

typedef struct AiTerrainFeatureWorkspaceEntry AiTerrainFeatureWorkspaceEntry, *PAiTerrainFeatureWorkspaceEntry;

struct AiTerrainFeatureWorkspaceEntry {
    struct FieldGridCell *cell; 
    dword unresolved04; 
    enum PckArmyAssetIdCatalog armyAssetId; 
    int priority; 
};

typedef struct AiRuntimeWorkspaceEntry AiRuntimeWorkspaceEntry, *PAiRuntimeWorkspaceEntry;

struct AiRuntimeWorkspaceEntry {
    struct ArmyRuntimeSlot *armyRuntime; 
    enum PckArmyAssetIdCatalog armyAssetId; 
};

typedef struct AiTargetWorkspaceEntry AiTargetWorkspaceEntry, *PAiTargetWorkspaceEntry;

struct AiTargetWorkspaceEntry {
    Q12 worldXQ12; 
    Q12 worldYQ12; 
    struct ArmyRuntimeSlot *armyRuntime; 
    struct ModelRuntimeNode *modelRuntime; 
};

typedef struct AiLinkedDefinitionListView AiLinkedDefinitionListView, *PAiLinkedDefinitionListView;

struct AiLinkedDefinitionListView {
    byte unresolved00_07[8]; 
    uint childListCount; 
    ModelLinkedDefinitionListAddress32 childList0Address; 
    ModelLinkedDefinitionListAddress32 childList1Address; 
    byte unresolved14_1F[12]; 
    enum PckModelDefinitionIdCatalog definitionIds[8]; 
};

typedef struct AiArmyScoreWeights AiArmyScoreWeights, *PAiArmyScoreWeights;

struct AiArmyScoreWeights {
    int pressureCategoryWeights[8]; 
    int definitionValue60Weight; 
    int definitionValue0CWeight; 
    int armyRecord74Weight; 
    int armyRecord78Weight; 
    int armyRecord70Weight; 
    int nonzeroDefinition18Bonus; 
    int baseScore; 
};

typedef int AiTechnologyCandidateScore;

typedef struct AiTechnologyPlanningCandidate AiTechnologyPlanningCandidate, *PAiTechnologyPlanningCandidate;

typedef enum AiTechnologyCandidateScoreKind {
    AI_TECHNOLOGY_SCORE_DEFAULT_ZERO=0,
    AI_TECHNOLOGY_SCORE_FACTION_SCALED=1,
    AI_TECHNOLOGY_SCORE_BASE_VALUE_KIND2=2,
    AI_TECHNOLOGY_SCORE_RUNTIME_CLASS_COMPATIBLE=3,
    AI_TECHNOLOGY_SCORE_BASE_VALUE_KIND4=4,
    AI_TECHNOLOGY_SCORE_CATEGORY_COMPATIBLE=5
} AiTechnologyCandidateScoreKind;

struct AiTechnologyPlanningCandidate {
    enum PckTechnologyIdCatalog technologyId00; 
    struct ArmyRuntimeSlot *sourceArmyRuntime04; 
    enum AiTechnologyCandidateScoreKind scoreKind08; 
    dword reserved0C; 
};

typedef dword AiTechnologyPlanningCandidateCount;

typedef dword AiTechnologyCategoryMask;

typedef struct SoundSampleAsset SoundSampleAsset, *PSoundSampleAsset;

typedef dword AssetDecodedBlockCount;

struct SoundSampleAsset {
    enum AssetMagic magic;
    byte reserved04_0B[8];
    AssetFormatVersion formatVersion;
    byte reserved10_AF[160];
    AssetDecodedBlockCount decodedBlockCount;
    byte reservedB4_1FF[332];
};

typedef struct SpatialSoundSlot SpatialSoundSlot, *PSpatialSoundSlot;

struct SpatialSoundSlot {
    struct DirectSoundVoiceSet *voiceSet; 
    struct IDirectSoundBuffer *activeVoice; 
    SpatialSoundGainQ15 desiredLeftGainQ15; 
    SpatialSoundGainQ15 desiredRightGainQ15; 
};

typedef struct GridInfluenceAddCallbackTable24 GridInfluenceAddCallbackTable24, *PGridInfluenceAddCallbackTable24;

struct GridInfluenceAddCallbackTable24 {
    void (*callbacks[24])(struct GameEntityRuntime *); 
};

typedef struct GridInfluenceRemoveCallbackTable24 GridInfluenceRemoveCallbackTable24, *PGridInfluenceRemoveCallbackTable24;

struct GridInfluenceRemoveCallbackTable24 {
    void (*callbacks[24])(struct GameEntityRuntime *); 
};

typedef struct SelectionPointerArray32 SelectionPointerArray32, *PSelectionPointerArray32;

struct SelectionPointerArray32 {
    struct GameEntityRuntime *entries[32]; 
};

typedef struct SelectionPlayerRuntimeBlock SelectionPlayerRuntimeBlock, *PSelectionPlayerRuntimeBlock;

struct SelectionPlayerRuntimeBlock {
    struct SelectionPointerArray32 selection; 
    struct SelectionPlayerPairRecord pairRecords80_807F[4096]; 
    dword primaryEntityOrFactionToken8080; 
    dword activePairCount8084; 
    int *terrainHeightScratchPlane8088; 
    dword *terrainMaterialEditPlane808C; 
    dword constructionLookupState8090; 
    dword primarySelectionEntityOffset8094; 
    dword pendingSelectionEntityOffset8098; 
    dword packedSelectionState809C; 
    dword assignmentToken80A0; 
    dword assignmentFlags80A4; 
    uint sessionFlags; 
    InGameSimulationStepBatchTicks simulationStepTicks; 
    byte reserved80B0_8117[104]; 
};

typedef struct PckArchiveHeader PckArchiveHeader, *PPckArchiveHeader;

typedef dword PckArchiveByteCount;

typedef dword PckArchiveVersion;

typedef dword PckArchiveFormat;

typedef dword PckPackedDate;

typedef dword PckPackedTime;

struct PckArchiveHeader {
    byte magic[4]; 
    PckArchiveByteCount archiveSize; 
    PckArchiveVersion version; 
    PckArchiveFormat format; 
    PckPackedDate dateValue0; 
    PckPackedTime timeValue0; 
    PckPackedDate dateValue1; 
    PckPackedTime timeValue1; 
    PckPackedDate dateValue2; 
    PckPackedTime timeValue2; 
    byte reserved28[8]; 
    word producerName[32]; 
    word sourceName[32]; 
    PckEntryCount entryCount; 
    byte reservedB4[332]; 
};

typedef struct PckEntryHeader PckEntryHeader, *PPckEntryHeader;

struct PckEntryHeader {
    word path[246]; 
    PckRuntimePayloadOffset runtimePayloadOffset; 
    PckDecodedByteCount unpackedSize; 
    enum PckAssetTypeTag typeTag; 
    PckStoredByteCount packedSize; 
    enum PckCompressionMethod compressionMethod; 
};

typedef struct PckMountSlot PckMountSlot, *PPckMountSlot;

struct PckMountSlot {
    EngineFileHandle fileHandle; 
    struct PckEntryHeader *entryHeaders; 
    PckEntryCount entryCount; 
};

typedef union ShotRayDistanceOrDefinitionReference4 ShotRayDistanceOrDefinitionReference4, *PShotRayDistanceOrDefinitionReference4;

union ShotRayDistanceOrDefinitionReference4 {
    Q12 rayDistanceQ12; 
    struct ShotDefinition *definition; 
    enum PckShotDefinitionIdCatalog savedId; 
    dword raw; 
};

typedef union GraphicsGeneratedTextureAssetOrEntryView200 GraphicsGeneratedTextureAssetOrEntryView200, *PGraphicsGeneratedTextureAssetOrEntryView200;

union GraphicsGeneratedTextureAssetOrEntryView200 {
    struct GraphicsTextureSourceAsset asset; 
    struct GraphicsTextureSourceEntry sourceEntry; 
};

typedef struct ModelRuntimeSlotUnrebaseSemanticView200 ModelRuntimeSlotUnrebaseSemanticView200, *PModelRuntimeSlotUnrebaseSemanticView200;

struct ModelRuntimeSlotUnrebaseSemanticView200 {
    union ModelDefinitionReferenceOrSavedId4 definitionReferenceOrSavedId; 
    dword rootModelNodeSavedOffset; 
    dword ownerArmyRuntimeSavedOffset; 
    dword attachmentCount0C; 
    byte reserved10_37[40]; 
    dword linkedModelRuntimeSavedOffset; 
    dword definitionValue60_3C; 
    dword definitionValue84_40; 
    dword definitionValue88_44; 
    dword definitionValue94_48; 
    dword definitionValue9C_4C; 
    dword definitionValueA4_50; 
    dword definitionValueAC_54; 
    dword definitionValueB4_58; 
    dword definitionValueBC_5C; 
    struct ModelRuntimeClassLinkState24 classLinkState; 
    struct ModelRuntimeSlotClassStateSerializedScalar7C classState; 
    byte reserved100_117[24]; 
    dword classState118; 
    dword classState11C; 
    byte reserved120_13F[32]; 
    struct ModelRuntimeAttachmentSerializedScalarDescriptor20 attachments140[6]; 
};

typedef union ArmyAssetReferenceOrSavedId4 ArmyAssetReferenceOrSavedId4, *PArmyAssetReferenceOrSavedId4;

typedef struct ArmyAssetRecordPrefix ArmyAssetRecordPrefix, *PArmyAssetRecordPrefix;

union ArmyAssetReferenceOrSavedId4 {
    struct ArmyAssetRecordPrefix *record; 
    enum PckArmyAssetIdCatalog savedId; 
    dword raw; 
};

struct ArmyAssetRecordPrefix {
    AssetRecordByteCount byteSize; 
    ArmySelectionDetailTemplateVariantIndex selectionDetailTemplateVariantIndex; 
    enum PckArmyAssetIdCatalog registryId; 
    dword rootNodeOffsetOrPointer; 
};

typedef struct ArmyAssetRuntimeSemanticView80 ArmyAssetRuntimeSemanticView80, *PArmyAssetRuntimeSemanticView80;

struct ArmyAssetRuntimeSemanticView80 {
    AssetRecordByteCount byteSize;
    ArmySelectionDetailTemplateVariantIndex selectionDetailTemplateVariantIndex;
    enum PckArmyAssetIdCatalog registryId;
    dword rootNodeOffsetOrPointer;
    void *linkedRuntimeOrRecord10;
    uint flags14; 
    byte reserved018_023[12]; 
    dword relocationValue24;
    dword relocationValue28;
    dword relocationPointerOrOffset2C;
    byte reserved030_06F[64]; 
    dword definitionClassValue70; 
    dword definitionClassValue74; 
    dword definitionClassValue78; 
    byte reserved07C_07F[4]; 
};

typedef struct ModelDefinitionRuntimeSemanticView280 ModelDefinitionRuntimeSemanticView280, *PModelDefinitionRuntimeSemanticView280;

struct ModelDefinitionRuntimeSemanticView280 {
    AssetRecordByteCount byteSize;
    dword flags;
    enum PckModelDefinitionIdCatalog definitionId;
    byte reserved00C_023[24]; 
    dword runtimeValue24;
    dword runtimeValue28;
    union ShotDefinitionReferenceOrSavedId4 shotDefinitionReference2C;
    byte reserved030_047[24]; 
    dword runtimeValue48;
    byte reserved04C_057[12]; 
    union EffectDefinitionReferenceOrSavedId4 effectDefinitionReference58;
    byte reserved05C_05F[4]; 
    dword runtimeValue60;
    dword serializedNodeOffsetOrPointer64;
    dword runtimeValue68;
    byte reserved06C_07F[20]; 
    union EffectDefinitionReferenceOrSavedId4 effectDefinitionReference80;
    dword runtimeValue84;
    union EffectDefinitionReferenceOrSavedId4 effectDefinitionReference88;
    dword runtimeValue8C;
    union EffectDefinitionReferenceOrSavedId4 effectDefinitionReference90;
    dword runtimeValue94;
    union EffectDefinitionReferenceOrSavedId4 effectDefinitionReference98;
    dword runtimeValue9C;
    union EffectDefinitionReferenceOrSavedId4 effectDefinitionReferenceA0;
    dword runtimeValueA4;
    union EffectDefinitionReferenceOrSavedId4 effectDefinitionReferenceA8;
    dword runtimeValueAC;
    union EffectDefinitionReferenceOrSavedId4 effectDefinitionReferenceB0;
    dword runtimeValueB4;
    union EffectDefinitionReferenceOrSavedId4 effectDefinitionReferenceB8;
    dword runtimeValueBC;
    byte reserved0C0_0DB[28]; 
    dword placementRadiusOrClearanceDC;
    byte reserved0E0_167[136]; 
    union ShotDefinitionReferenceOrSavedId4 shotDefinitionReference168;
    byte reserved16C_173[8]; 
    union EffectDefinitionReferenceOrSavedId4 effectDefinitionReference174;
    byte reserved178_187[16]; 
    void *buildMetricTuple188;
    dword runtimeValue18C;
    union EffectDefinitionReferenceOrSavedId4 effectDefinitionReference190;
    byte reserved194_197[4]; 
    dword runtimeValue198;
    byte reserved19C_19F[4]; 
    dword gridDerivedRuntimeValue1A0; 
    byte reserved1A4_1A7[4]; 
    dword placementFlags1A8;
    byte reserved1AC_1B7[12]; 
    ModelTextureSubresourceIndex primaryAnimatedSubresourceIndex1B8;
    ModelTextureSubresourceIndex secondaryAnimatedSubresourceIndex1BC;
    byte reserved1C0_253[148]; 
    union EffectDefinitionReferenceOrSavedId4 effectDefinitionReference254;
    byte reserved258_25F[8]; 
    dword gridClassification260;
    dword gridClassification264;
    dword runtimeValue268;
    byte reserved26C_277[12]; 
    dword placementCallbackSelector278;
    dword runtimeValue27C;
};

typedef struct ModelDefinitionResolvePhaseView280 ModelDefinitionResolvePhaseView280, *PModelDefinitionResolvePhaseView280;

struct ModelDefinitionResolvePhaseView280 {
    AssetRecordByteCount byteSize;
    dword flags;
    enum PckModelDefinitionIdCatalog definitionId;
    dword runtimeValue0C; 
    byte reserved010_017[8]; 
    dword runtimeValue18; 
    byte reserved01C_023[8]; 
    dword runtimeValue24;
    dword runtimeValue28;
    struct ShotDefinition *shotDefinitionReference2C; 
    dword runtimeValue30; 
    byte reserved034_047[20]; 
    dword runtimeValue48;
    byte reserved04C_057[12]; 
    struct EffectDefinition *effectDefinitionReference58; 
    uint categoryMaximumIndex5C; 
    dword runtimeValue60;
    dword serializedNodeOffsetOrPointer64;
    dword runtimeValue68;
    byte reserved06C_07F[20]; 
    struct EffectDefinition *effectDefinitionReference80; 
    dword runtimeValue84;
    struct EffectDefinition *effectDefinitionReference88; 
    dword runtimeValue8C;
    struct EffectDefinition *effectDefinitionReference90; 
    dword runtimeValue94;
    struct EffectDefinition *effectDefinitionReference98; 
    dword runtimeValue9C;
    struct EffectDefinition *effectDefinitionReferenceA0; 
    dword runtimeValueA4;
    struct EffectDefinition *effectDefinitionReferenceA8; 
    dword runtimeValueAC;
    struct EffectDefinition *effectDefinitionReferenceB0; 
    dword runtimeValueB4;
    struct EffectDefinition *effectDefinitionReferenceB8; 
    dword runtimeValueBC;
    byte reserved0C0_0DB[28]; 
    dword placementRadiusOrClearanceDC;
    byte reserved0E0_167[136]; 
    struct ShotDefinition *shotDefinitionReference168; 
    byte reserved16C_173[8]; 
    struct EffectDefinition *effectDefinitionReference174; 
    byte reserved178_187[16]; 
    void *buildMetricTuple188;
    dword runtimeValue18C;
    struct EffectDefinition *effectDefinitionReference190; 
    byte reserved194_197[4]; 
    dword runtimeValue198;
    byte reserved19C_19F[4]; 
    dword gridDerivedRuntimeValue1A0; 
    byte reserved1A4_1A7[4]; 
    dword placementFlags1A8;
    byte reserved1AC_1B7[12]; 
    ModelTextureSubresourceIndex primaryAnimatedSubresourceIndex1B8;
    ModelTextureSubresourceIndex secondaryAnimatedSubresourceIndex1BC;
    byte reserved1C0_253[148]; 
    struct EffectDefinition *effectDefinitionReference254; 
    byte reserved258_25F[8]; 
    dword gridClassification260;
    dword gridClassification264;
    dword runtimeValue268;
    byte reserved26C_277[12]; 
    dword placementCallbackSelector278;
    dword runtimeValue27C;
};

typedef struct ArmyRuntimeTerrainContactLinkedChildClassView120 ArmyRuntimeTerrainContactLinkedChildClassView120, *PArmyRuntimeTerrainContactLinkedChildClassView120;

typedef struct ArmyRuntimeTerrainContactLinkedChildState14 ArmyRuntimeTerrainContactLinkedChildState14, *PArmyRuntimeTerrainContactLinkedChildState14;

struct ArmyRuntimeTerrainContactLinkedChildState14 {
    enum ArmyTerrainContactDispatchMode terrainContactMode; 
    dword linkedChildDispatchState; 
    union ArmyRuntimeContactRadiusOrLinkedSlotMask4 contactRadiusOrLinkedSlotMask; 
    Q12 fallbackPosition0Q12; 
    Q12 fallbackPosition1Q12; 
};

struct ArmyRuntimeTerrainContactLinkedChildClassView120 {
    void *definitionOrAsset; 
    struct ModelRuntimeNode *modelNodeRuntime; 
    struct GameEntityRuntime *linkedEntityRuntime; 
    FactionRuntimeIndex factionIndex; 
    struct ArmyRuntimeMovementControlState movementControl; 
    ArmyMovementStateFlags movementStateFlags; 
    struct ArmyRuntimeSlot *commandTargetArmyRuntime; 
    Q12 commandCoordinate0Q12; 
    Q12 commandCoordinate1Q12; 
    Q12 commandCoordinate2Q12; 
    ArmyCommandModeFlags commandModeFlags; 
    ArmyCommandGeneration commandGeneration; 
    Q12 actionVector0Q12; 
    Q12 actionVector1Q12; 
    Q12 actionVector2Q12; 
    dword runtimeState40; 
    dword runtimeState44; 
    dword runtimeState48; 
    dword runtimeState4C; 
    byte reserved50_57[8]; 
    Q12 movementPosition0Q12; 
    Q12 movementPosition1Q12; 
    dword classState60; 
    dword ownerValue64; 
    dword ownerValue68; 
    dword linkedChildClassIndexOrSavedValue6C; 
    Q12 fallbackWorldYQ12; 
    Q12 fallbackWorldXQ12; 
    Q12 movementTarget0Q12; 
    Q12 movementTarget1Q12; 
    dword definitionClassValue80; 
    dword definitionClassValue84; 
    dword definitionClassValue88; 
    dword runtimeState8C; 
    dword runtimeState90; 
    dword runtimeState94; 
    dword runtimeState98; 
    enum ModelRuntimeClassId depthBinClass; 
    enum PckArmyAssetIdCatalog armyAssetId; 
    dword runtimeStateA4; 
    dword runtimeStateA8; 
    struct ArmyRuntimeTerrainContactLinkedChildState14 terrainContactLinkedChildState; 
    struct ArmyRuntimeLinkedChildOverloadedState10 linkedChildOverloadedState; 
    struct ArmyRuntimeLinkedChildSpawnParameters0C linkedChildSpawnParameters; 
    struct ArmyRuntimeLinkedChildPendingCounts4 linkedChildPendingCounts; 
    byte reservedE0_EB[12]; 
    ArmyRuntimeFlags runtimeFlags; 
    struct ArmyRuntimeSlot *linkedArmyRuntime; 
    ArmyRuntimeTimer runtimeTimer; 
    byte reservedF8_FF[8]; 
    int stateOrTechnologyId; 
    dword runtimeState104; 
    ArmySelectionMetric selectionMetric0; 
    ArmySelectionMetric selectionMetric1; 
    ArmySelectionMetric selectionMetric2; 
    ArmySelectionMetric selectionMetric3; 
    ArmySelectionMetric selectionMetric4; 
    ArmySelectionMetric selectionMetric5; 
};

typedef struct ModelRuntimeLinkedChildClassStateView7C ModelRuntimeLinkedChildClassStateView7C, *PModelRuntimeLinkedChildClassStateView7C;

struct ModelRuntimeLinkedChildClassStateView7C {
    byte reserved84_A7[36]; 
    dword classStateA8; 
    dword classStateAC; 
    sdword classStateB0; 
    sdword classStateB4; 
    dword classStateB8; 
    byte reservedBC_C3[8]; 
    FactionArmyAssetCount linkedChildSlotCountC4; 
    byte reservedC8_CF[8]; 
    sdword classStateD0; 
    byte reservedD4_DB[8]; 
    dword classStateDC; 
    dword classStateE0; 
    dword enabledStateE4; 
    dword enabledStateE8; 
    dword classStateEC; 
    union ArmyRuntimeReferenceOrSavedOffset4 linkedArmyRuntimeOrSavedOffset; 
    dword definitionDerivedValueF4; 
    dword classStateF8; 
    dword classStateFC; 
};

typedef struct EffectCompletionLinkedHandlerOwnerColumns104 EffectCompletionLinkedHandlerOwnerColumns104, *PEffectCompletionLinkedHandlerOwnerColumns104;

struct EffectCompletionLinkedHandlerOwnerColumns104 {
    ShotTerrainImpactEffectOwnerSlot ownerSlot0; 
    undefined field1_0x4;
    undefined field2_0x5;
    undefined field3_0x6;
    undefined field4_0x7;
    undefined field5_0x8;
    undefined field6_0x9;
    undefined field7_0xa;
    undefined field8_0xb;
    undefined field9_0xc;
    undefined field10_0xd;
    undefined field11_0xe;
    undefined field12_0xf;
    undefined field13_0x10;
    undefined field14_0x11;
    undefined field15_0x12;
    undefined field16_0x13;
    undefined field17_0x14;
    undefined field18_0x15;
    undefined field19_0x16;
    undefined field20_0x17;
    undefined field21_0x18;
    undefined field22_0x19;
    undefined field23_0x1a;
    undefined field24_0x1b;
    undefined field25_0x1c;
    undefined field26_0x1d;
    undefined field27_0x1e;
    undefined field28_0x1f;
    undefined field29_0x20;
    undefined field30_0x21;
    undefined field31_0x22;
    undefined field32_0x23;
    undefined field33_0x24;
    undefined field34_0x25;
    undefined field35_0x26;
    undefined field36_0x27;
    undefined field37_0x28;
    undefined field38_0x29;
    undefined field39_0x2a;
    undefined field40_0x2b;
    undefined field41_0x2c;
    undefined field42_0x2d;
    undefined field43_0x2e;
    undefined field44_0x2f;
    undefined field45_0x30;
    undefined field46_0x31;
    undefined field47_0x32;
    undefined field48_0x33;
    undefined field49_0x34;
    undefined field50_0x35;
    undefined field51_0x36;
    undefined field52_0x37;
    undefined field53_0x38;
    undefined field54_0x39;
    undefined field55_0x3a;
    undefined field56_0x3b;
    undefined field57_0x3c;
    undefined field58_0x3d;
    undefined field59_0x3e;
    undefined field60_0x3f;
    undefined field61_0x40;
    undefined field62_0x41;
    undefined field63_0x42;
    undefined field64_0x43;
    undefined field65_0x44;
    undefined field66_0x45;
    undefined field67_0x46;
    undefined field68_0x47;
    undefined field69_0x48;
    undefined field70_0x49;
    undefined field71_0x4a;
    undefined field72_0x4b;
    undefined field73_0x4c;
    undefined field74_0x4d;
    undefined field75_0x4e;
    undefined field76_0x4f;
    undefined field77_0x50;
    undefined field78_0x51;
    undefined field79_0x52;
    undefined field80_0x53;
    undefined field81_0x54;
    undefined field82_0x55;
    undefined field83_0x56;
    undefined field84_0x57;
    undefined field85_0x58;
    undefined field86_0x59;
    undefined field87_0x5a;
    undefined field88_0x5b;
    undefined field89_0x5c;
    undefined field90_0x5d;
    undefined field91_0x5e;
    undefined field92_0x5f;
    undefined field93_0x60;
    undefined field94_0x61;
    undefined field95_0x62;
    undefined field96_0x63;
    undefined field97_0x64;
    undefined field98_0x65;
    undefined field99_0x66;
    undefined field100_0x67;
    undefined field101_0x68;
    undefined field102_0x69;
    undefined field103_0x6a;
    undefined field104_0x6b;
    undefined field105_0x6c;
    undefined field106_0x6d;
    undefined field107_0x6e;
    undefined field108_0x6f;
    undefined field109_0x70;
    undefined field110_0x71;
    undefined field111_0x72;
    undefined field112_0x73;
    undefined field113_0x74;
    undefined field114_0x75;
    undefined field115_0x76;
    undefined field116_0x77;
    undefined field117_0x78;
    undefined field118_0x79;
    undefined field119_0x7a;
    undefined field120_0x7b;
    undefined field121_0x7c;
    undefined field122_0x7d;
    undefined field123_0x7e;
    undefined field124_0x7f;
    dword auxiliaryValue80; 
    undefined field126_0x84;
    undefined field127_0x85;
    undefined field128_0x86;
    undefined field129_0x87;
    undefined field130_0x88;
    undefined field131_0x89;
    undefined field132_0x8a;
    undefined field133_0x8b;
    undefined field134_0x8c;
    undefined field135_0x8d;
    undefined field136_0x8e;
    undefined field137_0x8f;
    undefined field138_0x90;
    undefined field139_0x91;
    undefined field140_0x92;
    undefined field141_0x93;
    undefined field142_0x94;
    undefined field143_0x95;
    undefined field144_0x96;
    undefined field145_0x97;
    undefined field146_0x98;
    undefined field147_0x99;
    undefined field148_0x9a;
    undefined field149_0x9b;
    undefined field150_0x9c;
    undefined field151_0x9d;
    undefined field152_0x9e;
    undefined field153_0x9f;
    undefined field154_0xa0;
    undefined field155_0xa1;
    undefined field156_0xa2;
    undefined field157_0xa3;
    undefined field158_0xa4;
    undefined field159_0xa5;
    undefined field160_0xa6;
    undefined field161_0xa7;
    undefined field162_0xa8;
    undefined field163_0xa9;
    undefined field164_0xaa;
    undefined field165_0xab;
    undefined field166_0xac;
    undefined field167_0xad;
    undefined field168_0xae;
    undefined field169_0xaf;
    undefined field170_0xb0;
    undefined field171_0xb1;
    undefined field172_0xb2;
    undefined field173_0xb3;
    undefined field174_0xb4;
    undefined field175_0xb5;
    undefined field176_0xb6;
    undefined field177_0xb7;
    undefined field178_0xb8;
    undefined field179_0xb9;
    undefined field180_0xba;
    undefined field181_0xbb;
    undefined field182_0xbc;
    undefined field183_0xbd;
    undefined field184_0xbe;
    undefined field185_0xbf;
    undefined field186_0xc0;
    undefined field187_0xc1;
    undefined field188_0xc2;
    undefined field189_0xc3;
    undefined field190_0xc4;
    undefined field191_0xc5;
    undefined field192_0xc6;
    undefined field193_0xc7;
    undefined field194_0xc8;
    undefined field195_0xc9;
    undefined field196_0xca;
    undefined field197_0xcb;
    undefined field198_0xcc;
    undefined field199_0xcd;
    undefined field200_0xce;
    undefined field201_0xcf;
    undefined field202_0xd0;
    undefined field203_0xd1;
    undefined field204_0xd2;
    undefined field205_0xd3;
    undefined field206_0xd4;
    undefined field207_0xd5;
    undefined field208_0xd6;
    undefined field209_0xd7;
    undefined field210_0xd8;
    undefined field211_0xd9;
    undefined field212_0xda;
    undefined field213_0xdb;
    undefined field214_0xdc;
    undefined field215_0xdd;
    undefined field216_0xde;
    undefined field217_0xdf;
    undefined field218_0xe0;
    undefined field219_0xe1;
    undefined field220_0xe2;
    undefined field221_0xe3;
    undefined field222_0xe4;
    undefined field223_0xe5;
    undefined field224_0xe6;
    undefined field225_0xe7;
    undefined field226_0xe8;
    undefined field227_0xe9;
    undefined field228_0xea;
    undefined field229_0xeb;
    undefined field230_0xec;
    undefined field231_0xed;
    undefined field232_0xee;
    undefined field233_0xef;
    undefined field234_0xf0;
    undefined field235_0xf1;
    undefined field236_0xf2;
    undefined field237_0xf3;
    undefined field238_0xf4;
    undefined field239_0xf5;
    undefined field240_0xf6;
    undefined field241_0xf7;
    undefined field242_0xf8;
    undefined field243_0xf9;
    undefined field244_0xfa;
    undefined field245_0xfb;
    undefined field246_0xfc;
    undefined field247_0xfd;
    undefined field248_0xfe;
    undefined field249_0xff;
    TerrainMaterialIndex terrainMaterialIndex100; 
};

typedef struct ArmyRuntimeTerrainContactLinkedChildModelView120 ArmyRuntimeTerrainContactLinkedChildModelView120, *PArmyRuntimeTerrainContactLinkedChildModelView120;

typedef struct ModelRuntimeLinkedChildClassView200 ModelRuntimeLinkedChildClassView200, *PModelRuntimeLinkedChildClassView200;

struct ModelRuntimeLinkedChildClassView200 {
    union ModelDefinitionReferenceOrSavedId4 definitionOrSavedId; 
    union ModelRuntimeNodeReferenceOrSavedOffset4 rootModelNodeOrSavedOffset; 
    union ArmyRuntimeReferenceOrSavedOffset4 ownerArmyRuntimeOrSavedOffset; 
    dword attachmentCount0C; 
    byte reserved10_37[40]; 
    union ModelRuntimeSlotReferenceOrSavedOffset4 linkedModelRuntimeOrSavedOffset; 
    dword definitionValue60_3C; 
    dword definitionValue84_40; 
    dword definitionValue88_44; 
    dword definitionValue94_48; 
    dword definitionValue9C_4C; 
    dword definitionValueA4_50; 
    dword definitionValueAC_54; 
    dword definitionValueB4_58; 
    dword definitionValueBC_5C; 
    struct ModelRuntimeClassLinkState24 classLinkState; 
    struct ModelRuntimeLinkedChildClassStateView7C linkedChildClassState; 
    byte reserved100_117[24]; 
    dword classState118; 
    dword classState11C; 
    byte reserved120_13F[32]; 
    struct ModelRuntimeAttachmentDescriptor attachments140[6]; 
};

struct ArmyRuntimeTerrainContactLinkedChildModelView120 {
    struct ModelRuntimeLinkedChildClassView200 *modelRuntime; 
    struct ModelRuntimeNode *modelNodeRuntime; 
    struct GameEntityRuntime *linkedEntityRuntime; 
    FactionRuntimeIndex factionIndex; 
    struct ArmyRuntimeMovementControlState movementControl; 
    ArmyMovementStateFlags movementStateFlags; 
    struct ArmyRuntimeSlot *commandTargetArmyRuntime; 
    Q12 commandCoordinate0Q12; 
    Q12 commandCoordinate1Q12; 
    Q12 commandCoordinate2Q12; 
    ArmyCommandModeFlags commandModeFlags; 
    ArmyCommandGeneration commandGeneration; 
    Q12 actionVector0Q12; 
    Q12 actionVector1Q12; 
    Q12 actionVector2Q12; 
    dword runtimeState40; 
    dword runtimeState44; 
    dword runtimeState48; 
    dword runtimeState4C; 
    byte reserved50_57[8]; 
    Q12 movementPosition0Q12; 
    Q12 movementPosition1Q12; 
    dword classState60; 
    dword ownerValue64; 
    dword ownerValue68; 
    dword linkedChildClassIndexOrSavedValue6C; 
    Q12 fallbackWorldYQ12; 
    Q12 fallbackWorldXQ12; 
    Q12 movementTarget0Q12; 
    Q12 movementTarget1Q12; 
    dword definitionClassValue80; 
    dword definitionClassValue84; 
    dword definitionClassValue88; 
    dword runtimeState8C; 
    dword runtimeState90; 
    dword runtimeState94; 
    dword runtimeState98; 
    enum ModelRuntimeClassId depthBinClass; 
    enum PckArmyAssetIdCatalog armyAssetId; 
    dword runtimeStateA4; 
    dword runtimeStateA8; 
    struct ArmyRuntimeTerrainContactLinkedChildState14 terrainContactLinkedChildState; 
    struct ArmyRuntimeLinkedChildOverloadedState10 linkedChildOverloadedState; 
    struct ArmyRuntimeLinkedChildSpawnParameters0C linkedChildSpawnParameters; 
    struct ArmyRuntimeLinkedChildPendingCounts4 linkedChildPendingCounts; 
    byte reservedE0_EB[12]; 
    ArmyRuntimeFlags runtimeFlags; 
    struct ArmyRuntimeSlot *linkedArmyRuntime; 
    ArmyRuntimeTimer runtimeTimer; 
    byte reservedF8_FF[8]; 
    int stateOrTechnologyId; 
    dword runtimeState104; 
    ArmySelectionMetric selectionMetric0; 
    ArmySelectionMetric selectionMetric1; 
    ArmySelectionMetric selectionMetric2; 
    ArmySelectionMetric selectionMetric3; 
    ArmySelectionMetric selectionMetric4; 
    ArmySelectionMetric selectionMetric5; 
};

typedef struct ArmyRuntimeTerrainContactLinkedChildSlotView120 ArmyRuntimeTerrainContactLinkedChildSlotView120, *PArmyRuntimeTerrainContactLinkedChildSlotView120;

struct ArmyRuntimeTerrainContactLinkedChildSlotView120 {
    struct ModelRuntimeLinkedChildClassView200 *modelRuntime; 
    struct ModelRuntimeNode *modelNodeRuntime; 
    struct GameEntityRuntime *linkedEntityRuntime; 
    FactionRuntimeIndex factionIndex; 
    struct ArmyRuntimeMovementControlState movementControl; 
    ArmyMovementStateFlags movementStateFlags; 
    struct ArmyRuntimeSlot *commandTargetArmyRuntime; 
    Q12 commandCoordinate0Q12; 
    Q12 commandCoordinate1Q12; 
    Q12 commandCoordinate2Q12; 
    ArmyCommandModeFlags commandModeFlags; 
    ArmyCommandGeneration commandGeneration; 
    Q12 actionVector0Q12; 
    Q12 actionVector1Q12; 
    Q12 actionVector2Q12; 
    dword runtimeState40; 
    dword runtimeState44; 
    dword runtimeState48; 
    dword runtimeState4C; 
    byte reserved50_57[8]; 
    Q12 movementPosition0Q12; 
    Q12 movementPosition1Q12; 
    dword classState60; 
    dword ownerValue64; 
    dword ownerValue68; 
    FactionArmyAssetCount linkedChildUsedSlotCount6C; 
    Q12 fallbackWorldYQ12; 
    Q12 fallbackWorldXQ12; 
    enum PckArmyAssetIdCatalog linkedChildAssetIdSlots13[13]; 
    struct ArmyRuntimeTerrainContactLinkedChildState14 terrainContactLinkedChildState; 
    struct ArmyRuntimeLinkedChildOverloadedState10 linkedChildOverloadedState; 
    struct ArmyRuntimeLinkedChildSpawnParameters0C linkedChildSpawnParameters; 
    struct ArmyRuntimeLinkedChildPendingCounts4 linkedChildPendingCounts; 
    byte reservedE0_EB[12]; 
    ArmyRuntimeFlags runtimeFlags; 
    struct ArmyRuntimeSlot *linkedArmyRuntime; 
    ArmyRuntimeTimer runtimeTimer; 
    byte reservedF8_FF[8]; 
    int stateOrTechnologyId; 
    dword runtimeState104; 
    ArmySelectionMetric selectionMetric0; 
    ArmySelectionMetric selectionMetric1; 
    ArmySelectionMetric selectionMetric2; 
    ArmySelectionMetric selectionMetric3; 
    ArmySelectionMetric selectionMetric4; 
    ArmySelectionMetric selectionMetric5; 
};

typedef struct EffectModelRuntimeNodeClassView100 EffectModelRuntimeNodeClassView100, *PEffectModelRuntimeNodeClassView100;

struct EffectModelRuntimeNodeClassView100 {
    struct WorldRuntimeNodeCommon common; 
    struct WorldRuntimeNodeModelPayload modelPayload; 
    struct EffectRuntimeSlot *effectRuntime; 
    ModelRuntimeFlags runtimeFlags; 
    ModelTextureSubresourceIndex textureSubresourceBaseIndex; 
    Q12 subtreeBoundingRadiusQ12; 
    PackedArgb32 tintArgb; 
    struct GraphicsShadingRuntimeRecord *shadingRecord; 
    void *modelRuntimeLinkOrSavedOffset; 
    ModelTextureSubresourceIndex primaryAnimatedSubresourceIndex; 
    ModelTextureOffsetTexel primaryTextureOffsetU; 
    ModelTextureOffsetTexel primaryTextureOffsetV; 
    struct GraphicsFixedMatrix3x4 worldTransform; 
    dword runtimeStateA0; 
    enum ModelRuntimeClassId ownerClassId; 
    ModelTextureSubresourceIndex secondaryAnimatedSubresourceIndex; 
    ModelTextureOffsetTexel secondaryTextureOffsetU; 
    ModelTextureOffsetTexel secondaryTextureOffsetV; 
    ModelDepthBinMask depthBinMaskNear; 
    ModelDepthBinMask depthBinMaskFar; 
    int renderDepthBiasOrState; 
    Q12 modelScaleQ12; 
    struct ModelRuntimeNode *parentNode; 
    dword childCount; 
    struct ModelRuntimeNode *childNodes[13]; 
};

typedef dword EnergyAmountQ4;

typedef dword ResourceExtractionRateQ4PerTick;

typedef struct ResourceRegistrationRuntimeImage ResourceRegistrationRuntimeImage, *PResourceRegistrationRuntimeImage;

struct ResourceRegistrationRuntimeImage {
    byte reserved0000_004F[80]; 
    dword levelRuntimeRecordIndex50; 
    byte reserved0054_0057[4]; 
    struct ResourceRegistrationRecord100 *records58; 
    byte reserved005C_00AB[80]; 
    dword recordCountAC; 
    byte reserved00B0_00D7[40]; 
    struct ResourceRegistrationRecord100 *tailRecordD8; 
};

typedef qword ResourceRegistrationImagePair;

typedef struct ResourceRegistrationImagePairComponents8 ResourceRegistrationImagePairComponents8, *PResourceRegistrationImagePairComponents8;

struct ResourceRegistrationImagePairComponents8 {
    void *runtimeImageBase; 
    dword byteLength; 
};

typedef struct ResourceRegistrationDomainPairDispatchTable3 ResourceRegistrationDomainPairDispatchTable3, *PResourceRegistrationDomainPairDispatchTable3;

struct ResourceRegistrationDomainPairDispatchTable3 {
    ResourceRegistrationImagePair (*callbacks[3])(void); 
};

typedef enum InGameScheduledConditionKind {
    INGAME_SCHEDULED_CONDITION_NONE_OR_UNUSED=0,
    INGAME_SCHEDULED_CONDITION_NO_ACTIVE_ENTITY_WITH_DEFINITION=2,
    INGAME_SCHEDULED_CONDITION_NO_ACTIVE_ENTITY_WITH_DEFINITION_AND_CLASS_COMMAND_GROUP_A=4,
    INGAME_SCHEDULED_CONDITION_NO_ACTIVE_ENTITY_WITH_DEFINITION_AND_RUNTIME_ID=6,
    INGAME_SCHEDULED_CONDITION_FACTION_INACTIVE_OR_RELATION_AT_LEAST_8=8,
    INGAME_SCHEDULED_CONDITION_PRIMARY_RESOURCE_CURRENT_AT_LEAST=10,
    INGAME_SCHEDULED_CONDITION_SECONDARY_RESOURCE_CURRENT_AT_LEAST=12,
    INGAME_SCHEDULED_CONDITION_ACTIVE_ARMY_SCALE_VALUE_AT_LEAST=14,
    INGAME_SCHEDULED_CONDITION_MATCHING_DEFINITION_AND_RUNTIME_ID_ACTIVE_ENTITY_COUNT_AT_LEAST=16,
    INGAME_SCHEDULED_CONDITION_FACTION_TERRAIN_OCCUPANCY_MASK_F9_PERCENT_AT_LEAST=18,
    INGAME_SCHEDULED_CONDITION_COUNTDOWN_ELAPSED=20,
    INGAME_SCHEDULED_CONDITION_PRIMARY_RESOURCE_LIMIT_AT_MOST_0FA0=22,
    INGAME_SCHEDULED_CONDITION_NO_ACTIVE_ENTITY_WITH_CLASS_ID_OUTSIDE_CLASS_COMMAND_GROUP_A=24,
    INGAME_SCHEDULED_CONDITION_BOOLEAN_POSTFIX_EXPRESSION=26
} InGameScheduledConditionKind;

typedef enum InGameEndConditionTriggerStateFlags {
    INGAME_END_CONDITION_TRIGGER_ACTIVE=1,
    INGAME_END_CONDITION_TRIGGER_PROCESSED=2
} InGameEndConditionTriggerStateFlags;

typedef enum InGameScheduledConditionStatusFlags {
    INGAME_SCHEDULED_CONDITION_SATISFIED=1,
    INGAME_SCHEDULED_CONDITION_KIND_MASK=254
} InGameScheduledConditionStatusFlags;

typedef enum FrontendPointerContextFlags {
    FRONTEND_POINTER_CONTEXT_SUPPRESS_BUILTIN_ACTION_RESOLUTION=16,
    FRONTEND_POINTER_CONTEXT_ROUTE_TO_SECONDARY_CALLBACK=32,
    FRONTEND_POINTER_CONTEXT_ROUTE_TO_BUILTIN_ACTION_RESOLUTION=64,
    FRONTEND_POINTER_CONTEXT_OBSERVED_ACTION_BRANCH_00000100=256,
    FRONTEND_POINTER_CONTEXT_OBSERVED_ACTION_BRANCH_00000200=512,
    FRONTEND_POINTER_CONTEXT_COMPARE_HITS_BY_METRIC_ONLY=4096,
    FRONTEND_POINTER_CONTEXT_OBSERVED_ACTION_BRANCH_00008000=32768,
    FRONTEND_POINTER_CONTEXT_ALLOW_CANDIDATE_WITHOUT_NODE_FLAG_20=4194304,
    FRONTEND_POINTER_CONTEXT_OBSERVED_BUTTON_BRANCH_04000000=67108864,
    FRONTEND_POINTER_CONTEXT_OBSERVED_CODE_OVERRIDE_40000000=1073741824,
    FRONTEND_POINTER_CONTEXT_OBSERVED_CODE_OVERRIDE_80000000=2147483648
} FrontendPointerContextFlags;

typedef dword ArenaAlignedPayloadByteCount;

typedef dword WorldMotionSplineChannelCount;

typedef dword SprMeshRecordFlags;

typedef dword MdlTimedEffectIntervalTicks;

typedef dword LevelAssetFormatVersion;

typedef dword FileSystemEnumerationRecordBytes;

typedef dword GraphicsPrimitiveQueueCount;

typedef dword FileSystemByteCount;

typedef dword MdlTimedEffectRandomSpanTicks;

typedef dword LocaleTimeFormat24HourFlag;

typedef dword PersistentOptionFlags5C;

typedef dword LevelAssetConverterVersion;

typedef dword GraphicsCursorEventRingIndex;

typedef dword LevelAssetByteSize;

typedef dword ArmyPlacementAuxiliaryValue;

typedef dword EngineDriveLetter;

typedef dword FileSystemEntryCount;

typedef dword ModelAttachmentPackedKindOrdinal;

typedef dword LocaleLanguageIdentifierDigits;

typedef dword MdlReloadTicks;

typedef int GraphicsResourceActiveToken;

typedef dword LocaleLongDateOrderCode;

typedef dword GraphicsBackendHandlerIndex;

typedef dword SprMeshGroupSelectorMask;

typedef dword LocaleNegativeNumberFormatCode;

typedef dword SprVertexCount;

typedef dword Dos83ComponentLength;

typedef dword PersistentOptionFlags40;

typedef dword RomRecordTableCount;

typedef dword SprTriangleCount;

typedef dword ArmyPlacementDispatchArg1;

typedef struct InGameEndConditionTriggerRecord8 InGameEndConditionTriggerRecord8, *PInGameEndConditionTriggerRecord8;

struct InGameEndConditionTriggerRecord8 {
    enum InGameEndConditionTriggerStateFlags stateFlags;
    byte movieVariantSelector;
    byte skipArmyDisableWhenOne;
    byte reserved03;
    byte factionRuntimeIndex;
    byte endMovieSelectionIndex;
    byte conditionIndex;
    byte reserved07;
};

typedef struct InGameConditionRuntimeStorageView800 InGameConditionRuntimeStorageView800, *PInGameConditionRuntimeStorageView800;

typedef struct InGameConditionScheduleImageView480 InGameConditionScheduleImageView480, *PInGameConditionScheduleImageView480;

typedef struct InGameScheduledConditionRecord10 InGameScheduledConditionRecord10, *PInGameScheduledConditionRecord10;

typedef union InGameScheduledConditionStatusAndKind4 InGameScheduledConditionStatusAndKind4, *PInGameScheduledConditionStatusAndKind4;

typedef union InGameScheduledConditionPayload0C InGameScheduledConditionPayload0C, *PInGameScheduledConditionPayload0C;

union InGameScheduledConditionStatusAndKind4 {
    enum InGameScheduledConditionKind kind; 
    enum InGameScheduledConditionStatusFlags statusFlags; 
    dword raw;
};

union InGameScheduledConditionPayload0C {
    dword operands[3];
    byte postfixExpression[12];
};

struct InGameScheduledConditionRecord10 {
    union InGameScheduledConditionStatusAndKind4 statusAndKind; 
    union InGameScheduledConditionPayload0C payload;
};

struct InGameConditionScheduleImageView480 {
    struct InGameScheduledConditionRecord10 conditions[64];
    struct InGameEndConditionTriggerRecord8 triggers[16];
};

struct InGameConditionRuntimeStorageView800 {
    byte runtimePrefix000_37F[896];
    struct InGameConditionScheduleImageView480 schedule; 
};

typedef struct SprMeshRecordHeader20 SprMeshRecordHeader20, *PSprMeshRecordHeader20;

struct SprMeshRecordHeader20 {
    dword recordByteSize;
    SprMeshGroupSelectorMask meshGroupSelectorMask; 
    SprVertexCount vertexCount; 
    SprTriangleCount triangleCount; 
    SprMeshRecordFlags meshRecordFlags; 
    byte reserved14_1F[12];
};

typedef struct RuntimeHexSegment RuntimeHexSegment, *PRuntimeHexSegment;

struct RuntimeHexSegment {
    byte opaque0000_00FF[256]; 
};

typedef struct InGameActiveNotificationPayload18 InGameActiveNotificationPayload18, *PInGameActiveNotificationPayload18;

struct InGameActiveNotificationPayload18 {
    dword payloadDword14; 
    dword payloadDword10;
    dword payloadDword0C;
    dword payloadDword08;
    dword payloadDword04;
    dword payloadDword00;
};

typedef struct FileSystemEntryNameUtf16_200 FileSystemEntryNameUtf16_200, *PFileSystemEntryNameUtf16_200;

struct FileSystemEntryNameUtf16_200 {
    word codeUnits[256]; 
};

typedef struct MdlDefinitionSemanticPrefix80 MdlDefinitionSemanticPrefix80, *PMdlDefinitionSemanticPrefix80;

struct MdlDefinitionSemanticPrefix80 {
    AssetRecordByteCount byteSize; 
    dword nameTextOffset; 
    enum PckModelDefinitionIdCatalog definitionId; 
    Q12 classSpeedQ12; 
    dword yawMaxVelocityTurn16; 
    dword pitchMaxVelocityTurn16; 
    dword unknown18; 
    dword yawAccelerationTurn16; 
    dword pitchAccelerationTurn16; 
    int pitchMinimumTurn16; 
    int pitchMaximumTurn16; 
    enum PckShotDefinitionIdCatalog shotDefinitionId; 
    MdlReloadTicks reloadTicks; 
    dword recoilRotationStepTurn16; 
    dword recoilDurationOrIntershotTicks; 
    Q12 recoilTranslationQ12; 
    dword actionVector0; 
    dword actionVector1; 
    dword unknown48; 
    enum ModelRuntimeClassId runtimeClassId; 
    dword unknown50; 
    Q12 placementRadiusQ12; 
    dword unknown58; 
    dword shotImpactClassIndex; 
    dword unknown60; 
    dword rootNodeOffset; 
    dword runtimeRenderFlags; 
    dword unknown6C; 
    dword unknown70; 
    dword unknown74; 
    dword positionedSoundGainQ15; 
    Q12 positionedSoundMaximumDistanceQ12; 
};

typedef struct PcxPreview64LegacyOpaqueShell1300 PcxPreview64LegacyOpaqueShell1300, *PPcxPreview64LegacyOpaqueShell1300;

struct PcxPreview64LegacyOpaqueShell1300 {
    byte opaquePreviewImageBytes[4864]; 
};

typedef struct RomRecordTableEntry200 RomRecordTableEntry200, *PRomRecordTableEntry200;

struct RomRecordTableEntry200 {
    byte opaqueEntry00_1B[28];
    dword recordId;
    byte opaqueEntry20_1FF[480];
};

typedef struct InGameRuntimeRootObservedView9E70 InGameRuntimeRootObservedView9E70, *PInGameRuntimeRootObservedView9E70;

struct InGameRuntimeRootObservedView9E70 {
    byte opaque0000_09B7[2488]; 
    struct RecentTextHistoryPointerList recentTextHistory09B8; 
    byte opaque09DC_0A2F[84]; 
    struct WorldRuntimeContext worldRuntime0A30; 
    byte opaque0B8C_0BA7[28]; 
    Q12 fieldRegionOriginWorldXQ12_0BA8; 
    Q12 fieldRegionOriginWorldYQ12_0BAC; 
    byte opaque0BB0_4937[15752]; 
    dword observedTerrainCompositeFlags4938; 
    byte opaque493C_49B3[120]; 
    int primaryResourceDisplayCurrent49B4; 
    int primaryResourceDisplayLimit49B8; 
    byte opaque49BC_4D53[920]; 
    dword observedRelationTransitionFlags4D54; 
    byte opaque4D58_9A6B[19732]; 
    FixedPlanarPointEdxEax8 fieldGridPosition9A6C; 
    byte opaque9A74_9B4B[216]; 
    dword observedSessionNotificationState9B4C; 
    dword observedSessionNotificationValue9B50; 
    byte opaque9B54_9E5F[780]; 
    dword observedPayloadScratch9E60[4]; 
};

typedef struct ShotDefinitionDisk2E0 ShotDefinitionDisk2E0, *PShotDefinitionDisk2E0;

struct ShotDefinitionDisk2E0 {
    dword trajectoryMode; 
    dword reservedZero04; 
    enum PckShotDefinitionIdCatalog definitionId; 
    Q12 launchSpeedQ12; 
    enum PckEffectDefinitionIdCatalog primaryEffectId; 
    enum PckEffectDefinitionIdCatalog terrainImpactEffectIds[31]; 
    enum PckEffectDefinitionIdCatalog targetClassImpactEffectIds[8]; 
    Q12 targetClassImpactDamageQ12[8]; 
    Q12 lifetimeOrRangeScaleD0Q12; 
    enum PckEffectDefinitionIdCatalog launchEffectId; 
    ShotModelSpinStepTurn16 modelSpinStepTurn16; 
    Q12 ballisticDivisorQ12; 
    byte reservedE0_15B[124]; 
    ShotFrameAdvanceThresholdQ4 animationFrameAdvanceThresholdQ4; 
    byte reserved160_1DB[124]; 
    ShotAnimationFrameCount animationFrameCount; 
    int terrainMaterialIndices31[31]; 
    PackedArgb32 shadingColorArgb; 
    GraphicsTransitionTickCount shadingTransitionDurationTicks; 
    dword unknown264; 
    enum PckEffectDefinitionIdCatalog secondaryEffectId; 
    ShotSecondaryEffectIntervalTicks secondaryEffectIntervalTicks; 
    Q12 rangeScale270Q12; 
    dword unknown274; 
    AngleTurn16Stored32 elevationOffsetAngle16; 
    Q12 mode2SelectionRangeQ12; 
    dword unknown280; 
    PackedArgb32 stateTintArgb; 
    dword unknown288; 
    dword unknown28C; 
    dword mode3LeadDisabled; 
    dword nestedResourceSaved; 
    dword ownedNestedResourcePresent; 
    word resourcePathUtf16[34]; 
};

typedef struct CubicSplineCoefficientTable80 CubicSplineCoefficientTable80, *PCubicSplineCoefficientTable80;

struct CubicSplineCoefficientTable80 {
    float coefficients[32]; 
};

typedef struct FrontendNetworkSettingsControl FrontendNetworkSettingsControl, *PFrontendNetworkSettingsControl;

struct FrontendNetworkSettingsControl {
    byte opaque0000_024F[592]; 
};

typedef struct GraphicsAdapterEnumerationContext GraphicsAdapterEnumerationContext, *PGraphicsAdapterEnumerationContext;

struct GraphicsAdapterEnumerationContext {
    byte opaque0000_003F[64]; 
};

typedef struct SprVertexRecord40_Pck SprVertexRecord40_Pck, *PSprVertexRecord40_Pck;

struct SprVertexRecord40_Pck {
    Q12 positionXQ12; 
    Q12 positionYQ12; 
    Q12 positionZQ12; 
    dword reserved0C; 
    Q12 normalXQ12; 
    Q12 normalYQ12; 
    Q12 normalZQ12; 
    dword reserved1C; 
    byte reserved20_3F[32]; 
};

typedef struct FrontendPointerContextRuntimeState118 FrontendPointerContextRuntimeState118, *PFrontendPointerContextRuntimeState118;

struct FrontendPointerContextRuntimeState118 {
    struct UiNodeBase base; 
    enum FrontendPointerContextFlags contextFlags; 
    byte reserved50_D7[136]; 
    struct UiNodeBase *candidateNodeListHead; 
    dword observedContextValueDC; 
    struct UiNodeBase *selectedHitNode; 
    int selectedHitMetric; 
    dword callbackArgumentE8; 
    dword callbackArgumentEC; 
    dword callbackArgumentF0; 
    byte reservedF4_103[16]; 
    dword (*resolvedActionCallback104)(dword, dword, dword, int, struct UiNodeBase *, struct FrontendPointerContextRuntimeState118 *); 
    dword (*resolvedActionCallback108)(dword, dword, dword, int, struct UiNodeBase *, struct FrontendPointerContextRuntimeState118 *); 
    dword (*resolvedActionCallback10C)(dword, dword, dword, int, struct UiNodeBase *, struct FrontendPointerContextRuntimeState118 *); 
    dword (*resolvedActionCallback110)(dword, dword, dword, int, struct UiNodeBase *, struct FrontendPointerContextRuntimeState118 *); 
    dword (*resolvedActionCallback114)(dword, dword, dword, int, struct UiNodeBase *, struct FrontendPointerContextRuntimeState118 *); 
};

typedef struct TechnologyMask256 TechnologyMask256, *PTechnologyMask256;

struct TechnologyMask256 {
    dword words[8]; 
};

typedef struct EffectDefinitionDiskC0 EffectDefinitionDiskC0, *PEffectDefinitionDiskC0;

struct EffectDefinitionDiskC0 {
    dword transitionKind; 
    dword reservedZero04; 
    enum PckEffectDefinitionIdCatalog definitionId; 
    dword runtimeValue0C; 
    dword linkedEffectPresent; 
    enum PckEffectDefinitionIdCatalog linkedEffectId; 
    dword linkedShotPresent; 
    enum PckShotDefinitionIdCatalog linkedShotId; 
    EffectMovementSpeedQ12 movementSpeedQ12; 
    dword runtimeValue24; 
    EffectFrameAdvanceThresholdQ4 frameAdvanceThresholdQ4; 
    TerrainGridMaskIndex terrainGridMaskIndex; 
    dword creationFlags; 
    PackedArgb32 shadingColorArgb; 
    GraphicsTransitionTickCount shadingTransitionDurationTicks; 
    GraphicsTransitionTickCount shadingReleaseTransitionDurationTicks; 
    EffectShadingCountdownTicks shadingStartCountdownTicks; 
    EffectShadingCountdownTicks shadingStopCountdownTicks; 
    enum PckEffectDefinitionIdCatalog periodicEffectId; 
    EffectPeriodicIntervalTicks periodicEffectIntervalTicks; 
    EffectAlphaFadeTicks alphaFadeInTicks; 
    EffectAlphaFadeTicks alphaFadeOutTicks; 
    dword unknown58; 
    dword unknown5C; 
    PackedArgb32 stateTintArgb; 
    Q12 modelScaleStartQ12; 
    Q12 modelScaleEndQ12; 
    dword positionedSoundArgument0; 
    dword positionedSoundArgument1; 
    dword nestedResourceSaved; 
    dword ownedNestedResourcePresent; 
    word resourcePathUtf16[34]; 
};

typedef struct RomRecordTableHeader200 RomRecordTableHeader200, *PRomRecordTableHeader200;

struct RomRecordTableHeader200 {
    byte opaqueHeader00_3B[60];
    RomRecordTableCount recordCount;
    byte opaqueHeader40_1FF[448];
};

typedef struct SprAttachmentRecord10 SprAttachmentRecord10, *PSprAttachmentRecord10;

struct SprAttachmentRecord10 {
    SprAttachmentPackedKey packedKindAndSelector; 
    Q12 localTranslationXQ12;
    Q12 localTranslationYQ12;
    Q12 localTranslationZQ12;
};

typedef struct CubicSplineMatrixWorkspace1000 CubicSplineMatrixWorkspace1000, *PCubicSplineMatrixWorkspace1000;

struct CubicSplineMatrixWorkspace1000 {
    float matrix[1024]; 
};

typedef struct LevelPlacementRecord LevelPlacementRecord, *PLevelPlacementRecord;

struct LevelPlacementRecord {
    enum PckArmyAssetIdCatalog armyAssetId; 
    FactionRuntimeIndex factionIndex; 
    GraphicsWorldCoordinateQ12 worldYQ12; 
    GraphicsWorldCoordinateQ12 worldXQ12; 
    AngleTurn16Stored32 orientationTurn16; 
    dword preservedTailDword0;
    dword preservedTailDword1;
    dword preservedTailDword2;
};

typedef struct SprLodGroupHeader20 SprLodGroupHeader20, *PSprLodGroupHeader20;

struct SprLodGroupHeader20 {
    dword groupByteSize;
    dword meshRecordCount;
    dword reserved08;
    dword groupFlags;
    byte reserved10_1F[16];
};

typedef struct FieldGridCompactCell FieldGridCompactCell, *PFieldGridCompactCell;

struct FieldGridCompactCell {
    FieldCellPersistedAux serializedAux54; 
    Q12 terrainHeightQ12; 
    Q12 waterSurfaceDeltaQ12; 
    enum FieldCellPackedFlagsAndMaterial flagsAndMaterial; 
};

typedef struct SprMeshRecordHeader20_Pck SprMeshRecordHeader20_Pck, *PSprMeshRecordHeader20_Pck;

struct SprMeshRecordHeader20_Pck {
    AssetRecordByteCount recordByteSize; 
    SprMeshGroupSelectorMask meshGroupSelectorMask; 
    SprVertexCount vertexCount; 
    SprTriangleCount triangleCount; 
    SprMeshRecordFlags meshRecordFlags; 
    byte reserved14_1F[12]; 
};

typedef struct FieldGridRuntimeCellView FieldGridRuntimeCellView, *PFieldGridRuntimeCellView;

typedef union FieldGridAux54LifetimeUnion FieldGridAux54LifetimeUnion, *PFieldGridAux54LifetimeUnion;

union FieldGridAux54LifetimeUnion {
    dword serializedAuxDword;
    dword runtimeTerrainDirectionRecordPointer;
};

struct FieldGridRuntimeCellView {
    dword animationPhaseSeed;
    dword visibilityOverlayArgb;
    dword triangle0NormalAngles;
    byte runtime0C_3F[52];
    dword worldXQ12;
    dword worldYQ12;
    dword terrainHeightQ12;
    dword waterSurfaceDeltaQ12;
    dword flagsAndMaterial;
    union FieldGridAux54LifetimeUnion aux54;
    byte runtime58_6F[24];
    qword occupancyMask;
    dword triangle1NormalAngles;
    dword runtime7C;
};

typedef struct SprTriangleRecord40_Pck SprTriangleRecord40_Pck, *PSprTriangleRecord40_Pck;

struct SprTriangleRecord40_Pck {
    dword vertex0SerializedOffset; 
    Q12 vertex0UQ12; 
    Q12 vertex0VQ12; 
    dword vertex1SerializedOffset; 
    Q12 vertex1UQ12; 
    Q12 vertex1VQ12; 
    dword vertex2SerializedOffset; 
    Q12 vertex2UQ12; 
    Q12 vertex2VQ12; 
    Q12 planeNormalXQ12; 
    Q12 planeNormalYQ12; 
    Q12 planeNormalZQ12; 
    dword textureSubresourceIndex; 
    dword renderFlags; 
    byte reserved38_3F[8]; 
};

typedef struct TechnologyRecordDisk40 TechnologyRecordDisk40, *PTechnologyRecordDisk40;

struct TechnologyRecordDisk40 {
    dword prerequisiteMasks[8]; 
    TechnologyXeniteCostQ4 xeniteCostQ4; 
    TechnologyEnergyCostQ4 energyCostQ4; 
    TechnologyResearchDurationQ5 researchDurationQ5; 
    enum PckTechnologyIdCatalog dependencyTechnologyIndex; 
    UiTextResourceId completionMessageResourceId; 
    dword category; 
    AiTechnologyCandidateScore baseCandidateScore; 
    dword reservedZero3C; 
};

typedef struct TextResourceAssetHeader TextResourceAssetHeader, *PTextResourceAssetHeader;

typedef struct TextResourceLocaleCountHeader TextResourceLocaleCountHeader, *PTextResourceLocaleCountHeader;

struct TextResourceLocaleCountHeader {
    struct GeneratedAssetCommonPrefix common;
    AssetRecordCount localeBlockCount;
};

struct TextResourceAssetHeader {
    struct TextResourceLocaleCountHeader localeCountHeader;
    byte reservedB4_1FF[332];
};

typedef struct TextResourcePageBinding TextResourcePageBinding, *PTextResourcePageBinding;

typedef struct TextResourceLocaleBlockPrefix TextResourceLocaleBlockPrefix, *PTextResourceLocaleBlockPrefix;

struct TextResourceLocaleBlockPrefix {
    TextResourceLocaleBlockByteSize blockSizeBytes; 
    TextResourceStringCount stringCount; 
    enum LocaleTelephoneCountryCode countryCode; 
    dword reserved0C; 
};

struct TextResourcePageBinding {
    struct TextResourceLocaleBlockPrefix *selectedLocaleBlock; 
    struct TextResourceAssetHeader *asset; 
};

typedef struct WideNumberFormatState WideNumberFormatState, *PWideNumberFormatState;

struct WideNumberFormatState {
    WideNumberFormatCodeUnitCount decimalSeparatorLength; 
    WideNumberFormatCodeUnitCount groupSeparatorLength; 
    WideNumberFormatCodeUnitCount positiveSignLength; 
    WideNumberFormatCodeUnitCount negativeSignLength; 
    WideNumberFormatCodeUnitCount hexPrefixLength; 
    WideNumberFormatCodeUnitCount hexSuffixLength; 
    word decimalSeparator[16]; 
    word groupSeparator[16]; 
    word positiveSign[16]; 
    word negativeSign[16]; 
    word hexPrefix[16]; 
    word hexSuffix[16]; 
    word spacePadding[16]; 
    word zeroPadding[16]; 
    word reservedZero[16]; 
    word digitAlphabet[16]; 
};

typedef struct TextResourceOverrideTable TextResourceOverrideTable, *PTextResourceOverrideTable;

struct TextResourceOverrideTable {
    dword resourceIds[4096]; 
    word *textPointers[4096]; 
};

typedef enum WideNumberFormatFlags {
    WIDE_FORMAT_SIGNED_VALUE=1,
    WIDE_FORMAT_HEXADECIMAL=2,
    WIDE_FORMAT_FIXED_FRACTION_WIDTH=4,
    WIDE_FORMAT_PAD_WITH_ZERO=8,
    WIDE_FORMAT_PAD_WITH_SPACE=16,
    WIDE_FORMAT_SHOW_PLUS_SIGN=32,
    WIDE_FORMAT_WRITE_TERMINATOR=64,
    WIDE_FORMAT_GROUP_THOUSANDS=128
} WideNumberFormatFlags;

typedef struct RichTextExtentRegs RichTextExtentRegs, *PRichTextExtentRegs;

struct RichTextExtentRegs {
    dword widthPixels; 
    dword heightPixels; 
};

typedef struct GraphicsDisplayMode GraphicsDisplayMode, *PGraphicsDisplayMode;

struct GraphicsDisplayMode {
    FrontendDisplayDimensionPixels width; 
    FrontendDisplayDimensionPixels height; 
    FrontendColorDepthBits bitsPerPixel; 
    FrontendDisplayAdapterIndex adapterIndex; 
};

typedef struct GraphicsTexturePaletteEntry GraphicsTexturePaletteEntry, *PGraphicsTexturePaletteEntry;

struct GraphicsTexturePaletteEntry {
    PackedArgb32 argb8888; 
    PackedFramebufferPixel framebufferPixel; 
};

typedef struct GraphicsProjectedVertexSource GraphicsProjectedVertexSource, *PGraphicsProjectedVertexSource;

struct GraphicsProjectedVertexSource {
    byte reserved00_0B[12]; 
    dword texturedPacketAttributes[5]; 
    GraphicsPrimitiveBackendCoordinate backendCoord0; 
    GraphicsPrimitiveBackendCoordinate backendCoord1; 
    GraphicsPrimitiveDepthFixed depth; 
    dword reserved2C; 
    GraphicsPrimitiveScreenCoordinate screenX; 
    GraphicsPrimitiveScreenCoordinate screenY; 
};

typedef struct GraphicsPaletteTextureSourceAsset GraphicsPaletteTextureSourceAsset, *PGraphicsPaletteTextureSourceAsset;

struct GraphicsPaletteTextureSourceAsset {
    enum GraphicsPaletteTextureAssetMagic magic; 
    GraphicsAssetAllocationByteSize allocationSizeBytes; 
    enum GraphicsPaletteTextureFormatVersion formatVersion; 
    dword reserved0C; 
    byte reserved10_AF[160]; 
    GraphicsAssetSubresourceCount subresourceCount; 
    GraphicsPaletteBankCount paletteBankCount; 
    GraphicsAssetRelativeByteOffset subresourceTableOffset; 
    byte reservedBC_1FF[324]; 
    struct GraphicsTexturePaletteEntry paletteEntries[1]; 
};

typedef struct SoftwareBgraWordLanes SoftwareBgraWordLanes, *PSoftwareBgraWordLanes;

struct SoftwareBgraWordLanes {
    SoftwareColorLaneUnsigned16 blue; 
    SoftwareColorLaneUnsigned16 green; 
    SoftwareColorLaneUnsigned16 red; 
    SoftwareColorLaneUnsigned16 alpha; 
};

typedef struct SoftwarePixelFormatConfig SoftwarePixelFormatConfig, *PSoftwarePixelFormatConfig;

struct SoftwarePixelFormatConfig {
    GraphicsPixelChannelBitCount redBitCount; 
    GraphicsPixelChannelBitCount greenBitCount; 
    GraphicsPixelChannelBitCount blueBitCount; 
    GraphicsPixelChannelBitShift redShift; 
    GraphicsPixelChannelBitShift greenShift; 
    GraphicsPixelChannelBitShift blueShift; 
    GraphicsPackedPixelMask redMask; 
    GraphicsPackedPixelMask greenMask; 
    GraphicsPackedPixelMask blueMask; 
};

typedef struct GraphicsCapturedTextureSourceAsset GraphicsCapturedTextureSourceAsset, *PGraphicsCapturedTextureSourceAsset;

struct GraphicsCapturedTextureSourceAsset {
    struct GeneratedAssetCommonPrefix common; 
    struct GraphicsTextureSourceTableDescriptor tableDescriptor; 
    byte opaqueTablePayloadBC_1FF[324]; 
    struct GraphicsTextureSourceEntry sourceEntry; 
    dword argb8888Pixels[1]; 
};

typedef struct SoftwareRasterScalarMmxLane SoftwareRasterScalarMmxLane, *PSoftwareRasterScalarMmxLane;

struct SoftwareRasterScalarMmxLane {
    sdword value; 
    sdword unusedHighLane; 
};

typedef struct GraphicsPrimitivePacket GraphicsPrimitivePacket, *PGraphicsPrimitivePacket;

typedef struct GraphicsPrimitiveVertexRaw GraphicsPrimitiveVertexRaw, *PGraphicsPrimitiveVertexRaw;

struct GraphicsPrimitiveVertexRaw {
    GraphicsPrimitiveScreenCoordinate screenX; 
    GraphicsPrimitiveScreenCoordinate screenY; 
    GraphicsPrimitiveBackendCoordinate backendCoord0; 
    GraphicsPrimitiveBackendCoordinate backendCoord1; 
    GraphicsPrimitiveDepthFixed depth; 
    GraphicsPrimitiveTextureCoordinateFixed textureU; 
    GraphicsPrimitiveTextureCoordinateFixed textureV; 
    PackedArgb32 diffuseColor; 
};

struct GraphicsPrimitivePacket {
    struct GraphicsPrimitiveVertexRaw vertices[3]; 
    PackedArgb32 modulationColor; 
    struct GraphicsTextureSetEntry *textureEntry; 
    GraphicsPrimitiveDispatchFlags renderFlags; 
    byte reserved6C_7F[20]; 
};

typedef union GraphicsPrimitiveRadixBucket GraphicsPrimitiveRadixBucket, *PGraphicsPrimitiveRadixBucket;

typedef struct GraphicsPrimitiveQueueNode GraphicsPrimitiveQueueNode, *PGraphicsPrimitiveQueueNode;

union GraphicsPrimitiveRadixBucket {
    dword count; 
    struct GraphicsPrimitiveQueueNode *writeCursor; 
};

struct GraphicsPrimitiveQueueNode {
    dword sortKey; 
    struct GraphicsPrimitivePacket *packet; 
    struct GraphicsPrimitiveQueueNode *next; 
    struct GraphicsPrimitiveQueueNode *previous; 
};

typedef struct GraphicsTriangleInput GraphicsTriangleInput, *PGraphicsTriangleInput;

struct GraphicsTriangleInput {
    struct GraphicsProjectedVertexSource *vertex0; 
    GraphicsPrimitiveTextureCoordinateFixed textureU0; 
    GraphicsPrimitiveTextureCoordinateFixed textureV0; 
    struct GraphicsProjectedVertexSource *vertex1; 
    GraphicsPrimitiveTextureCoordinateFixed textureU1; 
    GraphicsPrimitiveTextureCoordinateFixed textureV1; 
    struct GraphicsProjectedVertexSource *vertex2; 
    GraphicsPrimitiveTextureCoordinateFixed textureU2; 
    GraphicsPrimitiveTextureCoordinateFixed textureV2; 
    GraphicsPlaneNormalFixed planeNormalXQ12; 
    GraphicsPlaneNormalFixed planeNormalYQ12; 
    GraphicsPlaneNormalFixed planeNormalZQ12; 
    GraphicsSubresourceIndex subresourceIndex; 
    GraphicsPrimitiveDispatchFlags renderFlags; 
};

typedef struct GlideTextureInfo GlideTextureInfo, *PGlideTextureInfo;

struct GlideTextureInfo {
    dword smallLod; 
    dword largeLod; 
    sdword aspectRatio; 
    dword format; 
    void *data; 
};

typedef struct GraphicsDispatchTable GraphicsDispatchTable, *PGraphicsDispatchTable;

struct GraphicsDispatchTable {
    void (*colorUpload[3])(struct GraphicsTextureResource *); 
    void (*alphaUpload[3])(struct GraphicsTextureResource *); 
    void (*primitive[64])(struct GraphicsPrimitivePacket *); 
};

typedef struct SoftwareRgbWordLanes SoftwareRgbWordLanes, *PSoftwareRgbWordLanes;

struct SoftwareRgbWordLanes {
    SoftwareColorLaneFixed16 blue; 
    SoftwareColorLaneFixed16 green; 
    SoftwareColorLaneFixed16 red; 
    short zero; 
};

typedef struct SoftwareFramebufferAccess SoftwareFramebufferAccess, *PSoftwareFramebufferAccess;

struct SoftwareFramebufferAccess {
    GraphicsPixelDimension width; 
    GraphicsPixelDimension height; 
    enum SoftwareFramebufferPixelSize bytesPerPixel; 
    byte *pixels; 
};

typedef struct ComReleaseObject_Vtbl ComReleaseObject_Vtbl, *PComReleaseObject_Vtbl;

typedef struct ComReleaseObject ComReleaseObject, *PComReleaseObject;

struct ComReleaseObject_Vtbl {
    sdword (*QueryInterface)(struct ComReleaseObject *, void *, void **);
    dword (*AddRef)(struct ComReleaseObject *);
    dword (*Release)(struct ComReleaseObject *);
};

struct ComReleaseObject {
    struct ComReleaseObject_Vtbl *lpVtbl; 
};

typedef struct GraphicsCursorInputEvent GraphicsCursorInputEvent, *PGraphicsCursorInputEvent;

struct GraphicsCursorInputEvent {
    enum GraphicsCursorEventType eventType; 
    dword buttonMask; 
    UiPixelCoordinate x; 
    UiPixelCoordinate y; 
    UiPointerWheelDelta wheelDelta; 
    GraphicsCursorClockValue clockValue; 
};

typedef struct GraphicsPrimitiveRenderStateCache GraphicsPrimitiveRenderStateCache, *PGraphicsPrimitiveRenderStateCache;

typedef enum D3DBLEND_DX6 {
    D3DBLEND_ZERO=1,
    D3DBLEND_ONE=2,
    D3DBLEND_SRCCOLOR=3,
    D3DBLEND_INVSRCCOLOR=4,
    D3DBLEND_SRCALPHA=5,
    D3DBLEND_INVSRCALPHA=6,
    D3DBLEND_DESTALPHA=7,
    D3DBLEND_INVDESTALPHA=8,
    D3DBLEND_DESTCOLOR=9,
    D3DBLEND_INVDESTCOLOR=10,
    D3DBLEND_SRCALPHASAT=11,
    D3DBLEND_BOTHSRCALPHA=12,
    D3DBLEND_BOTHINVSRCALPHA=13
} D3DBLEND_DX6;

struct GraphicsPrimitiveRenderStateCache {
    enum GraphicsBooleanState zWriteEnable; 
    enum GraphicsBooleanState alphaBlendEnable; 
    enum D3DBLEND_DX6 sourceBlend; 
    enum D3DBLEND_DX6 destinationBlend; 
};

typedef struct SoftwareMaskRuntimeView SoftwareMaskRuntimeView, *PSoftwareMaskRuntimeView;

struct SoftwareMaskRuntimeView {
    byte unresolved00_4F[80]; 
    struct GraphicsTextureSourceAsset *textureSource; 
    uint patternState54; 
    uint patternState58; 
    dword unresolved5C; 
    byte *maskPixels; 
    dword unresolved64; 
    int tickCounter; 
};

typedef struct SoftwarePixelPackTables SoftwarePixelPackTables, *PSoftwarePixelPackTables;

struct SoftwarePixelPackTables {
    dword blue[256]; 
    dword green[256]; 
    dword red[256]; 
};

typedef struct GraphicsResourceRecord GraphicsResourceRecord, *PGraphicsResourceRecord;

struct GraphicsResourceRecord {
    struct ComReleaseObject *object0; 
    struct ComReleaseObject *object1; 
    struct ComReleaseObject *object2; 
    struct IDirectDrawSurface *deviceSurfaceBase; 
    struct IDirectDrawSurface3 *deviceSurface3; 
    struct IDirect3DTexture2 *deviceTexture2; 
    GraphicsResourceActiveToken activeToken; 
    GraphicsResourceUsageSerial lastUsedCounter; 
    GraphicsPixelDimension width; 
    GraphicsPixelDimension height; 
    void *sourceData; 
    GraphicsBackendHandlerIndex backendHandlerIndex; 
};

typedef struct SoftwareRasterTextureAddressState SoftwareRasterTextureAddressState, *PSoftwareRasterTextureAddressState;

struct SoftwareRasterTextureAddressState {
    dword uMaskQ12; 
    dword zeroAfterUMask; 
    dword zeroBeforeVMask; 
    dword vMaskQ12; 
    dword vRowAddressShift; 
    dword zeroShiftHigh; 
};

typedef struct GraphicsCursorFrameRecord GraphicsCursorFrameRecord, *PGraphicsCursorFrameRecord;

struct GraphicsCursorFrameRecord {
    UiPixelOffset hotspotX; 
    UiPixelOffset hotspotY; 
    GraphicsSubresourceIndex idleAnimationFirstSubresourceIndex; 
    GraphicsSubresourceIndex idleAnimationLastSubresourceIndex; 
    GraphicsSubresourceIndex activeAnimationFirstSubresourceIndex; 
    GraphicsSubresourceIndex activeAnimationLastSubresourceIndex; 
    GraphicsSubresourceIndex idleSubresourceIndex; 
    GraphicsSubresourceIndex activeSubresourceIndex; 
};

typedef struct GraphicsWideFixed GraphicsWideFixed, *PGraphicsWideFixed;

struct GraphicsWideFixed {
    dword low; 
    sdword high; 
};

typedef struct GraphicsFixedRect GraphicsFixedRect, *PGraphicsFixedRect;

struct GraphicsFixedRect {
    GraphicsSceneExtentFixed minX; 
    GraphicsSceneExtentFixed minY; 
    GraphicsSceneExtentFixed maxX; 
    GraphicsSceneExtentFixed maxY; 
};

typedef struct GraphicsPrimitiveRenderStatePreset GraphicsPrimitiveRenderStatePreset, *PGraphicsPrimitiveRenderStatePreset;

struct GraphicsPrimitiveRenderStatePreset {
    enum D3DBLEND_DX6 sourceBlend; 
    enum D3DBLEND_DX6 destinationBlend; 
    enum GraphicsBooleanState alphaBlendEnable; 
    enum GraphicsBooleanState zWriteEnable; 
};

typedef struct SoftwarePixelMmxConstants SoftwarePixelMmxConstants, *PSoftwarePixelMmxConstants;

struct SoftwarePixelMmxConstants {
    struct SoftwareRgbWordLanes packWeights; 
    struct SoftwareRgbWordLanes quantizeMasksQ12; 
    struct SoftwareRgbWordLanes unpackScales; 
    struct SoftwareRgbWordLanes packedPixelMasks; 
};

typedef struct GraphicsPrimitiveQueue GraphicsPrimitiveQueue, *PGraphicsPrimitiveQueue;

struct GraphicsPrimitiveQueue {
    dword capacity; 
    dword count; 
    struct GraphicsPrimitivePacket *packetPool; 
    struct GraphicsPrimitiveQueueNode *radixScratchPool; 
    struct GraphicsPrimitiveQueueNode *traversalCursor; 
    dword reserved14; 
    dword reserved18; 
    dword reserved1C; 
    struct GraphicsPrimitiveQueueNode primaryNodes[1]; 
};

typedef struct GraphicsFixedVec2 GraphicsFixedVec2, *PGraphicsFixedVec2;

struct GraphicsFixedVec2 {
    sdword component0; 
    sdword component1; 
};

typedef struct SoftwareRasterTexCoordFixed2 SoftwareRasterTexCoordFixed2, *PSoftwareRasterTexCoordFixed2;

struct SoftwareRasterTexCoordFixed2 {
    GraphicsPrimitiveTextureCoordinateFixed u; 
    GraphicsPrimitiveTextureCoordinateFixed v; 
};

typedef struct GraphicsSceneBounds8 GraphicsSceneBounds8, *PGraphicsSceneBounds8;

struct GraphicsSceneBounds8 {
    sdword bound0; 
    sdword bound1; 
    sdword bound2; 
    sdword bound3; 
    sdword bound4; 
    sdword bound5; 
    sdword bound6; 
    sdword bound7; 
};

typedef struct SoftwareRasterScanState SoftwareRasterScanState, *PSoftwareRasterScanState;

typedef struct SoftwareRasterColorFixed4 SoftwareRasterColorFixed4, *PSoftwareRasterColorFixed4;

struct SoftwareRasterColorFixed4 {
    SoftwareColorLaneFixed16 blue; 
    SoftwareColorLaneFixed16 green; 
    SoftwareColorLaneFixed16 red; 
    SoftwareColorLaneFixed16 alpha; 
};

struct SoftwareRasterScanState {
    struct SoftwareRasterColorFixed4 longEdgeColor; 
    struct SoftwareRasterColorFixed4 longEdgeColorStepY; 
    struct SoftwareRasterColorFixed4 colorStepX; 
    struct SoftwareRasterTexCoordFixed2 longEdgeTexCoord; 
    struct SoftwareRasterTexCoordFixed2 longEdgeTexCoordStepY; 
    struct SoftwareRasterTexCoordFixed2 texCoordStepX; 
    struct SoftwareRasterScalarMmxLane longEdgeDepth; 
    struct SoftwareRasterScalarMmxLane longEdgeDepthStepY; 
    struct SoftwareRasterScalarMmxLane depthStepX; 
    sdword longEdgeXQ12; 
    sdword shortEdgeXQ12; 
    sdword longEdgeXStepYQ12; 
    sdword shortEdgeXStepYQ12; 
    struct SoftwareRasterTextureAddressState textureAddress; 
    sdword scanlineY; 
};

typedef struct TerrainCompositeTextureRuntime TerrainCompositeTextureRuntime, *PTerrainCompositeTextureRuntime;

struct TerrainCompositeTextureRuntime {
    struct GraphicsTextureSourceAsset textureSource; 
    struct GraphicsTextureSourceEntry sourceEntries[3]; 
    dword argbPixels[1]; 
};

typedef struct DirectDrawPaletteEntry DirectDrawPaletteEntry, *PDirectDrawPaletteEntry;

struct DirectDrawPaletteEntry {
    ColorChannelByte red; 
    ColorChannelByte green; 
    ColorChannelByte blue; 
    PaletteEntryFlagsByte flags; 
};

typedef dword GridPathCost;

typedef struct GridScratchCell GridScratchCell, *PGridScratchCell;

typedef enum GridScratchStateMask { 
    GRID_SCRATCH_TRAVERSAL_VISITED=1,
    GRID_SCRATCH_TERRAIN_CLASS_BIT24=16777216,
    GRID_SCRATCH_TERRAIN_CLASS_BIT25=33554432,
    GRID_SCRATCH_TERRAIN_CLASS_BIT26=67108864,
    GRID_SCRATCH_TERRAIN_CLASS_BIT27=134217728,
    GRID_SCRATCH_TERRAIN_CLASS_BIT28=268435456,
    GRID_SCRATCH_TERRAIN_CLASS_BIT29=536870912,
    GRID_SCRATCH_TERRAIN_CLASS_BIT30=1073741824
} GridScratchStateMask;

struct GridScratchCell {
    enum GridScratchStateMask stateMask; 
    GridPathCost pathCost; 
};

typedef union TerrainScanSelectorUnion TerrainScanSelectorUnion, *PTerrainScanSelectorUnion;

typedef int FieldGridOccupancyByteIndex;

union TerrainScanSelectorUnion {
    FieldGridOccupancyByteIndex occupancyMaskByteIndex; 
    FieldCellFlagMask fieldCellFlagMask; 
    dword raw; 
};

typedef struct FncHostApiTable FncHostApiTable, *PFncHostApiTable;

struct FncHostApiTable {
    void *alloc; 
    void *free; 
    void *allocLargestFreeBlock; 
    void *shrinkInPlace; 
    void *getPackedCurrentTime; 
    void *getPackedCurrentDate; 
    void *copyDefaultComputerLabelUtf16; 
};

typedef struct FncModuleHeader FncModuleHeader, *PFncModuleHeader;

typedef struct FncModuleExportBindingDescriptor FncModuleExportBindingDescriptor, *PFncModuleExportBindingDescriptor;

typedef dword AssetExportCount;

typedef dword FncBindingMode;

struct FncModuleExportBindingDescriptor {
    AssetExportCount exportCount;
    FncBindingMode bindingMode;
    AssetRelativeOffset exportTableOffset;
    AssetRelativeOffset hostApiTableOffset;
};

struct FncModuleHeader {
    enum AssetMagic magic;
    AssetAllocationSizeBytes allocationSizeBytes;
    AssetFormatVersion formatVersion;
    dword reserved0C;
    byte reserved10_AF[160];
    struct FncModuleExportBindingDescriptor exportBinding;
    byte reservedC0_1FF[320];
};

typedef qword PreservedEaxEdxRegisterPair64;

typedef struct SoundCoefficientBlock256 SoundCoefficientBlock256, *PSoundCoefficientBlock256;

struct SoundCoefficientBlock256 {
    short coefficients[256]; 
};

typedef qword MmxPackedValue64;

typedef struct GameFactionRuntimeRecord GameFactionRuntimeRecord, *PGameFactionRuntimeRecord;

typedef struct AiFactionCandidateCacheState AiFactionCandidateCacheState, *PAiFactionCandidateCacheState;

struct AiFactionCandidateCacheState {
    struct AiCandidateWorkspaceEntry savedEntries[3]; 
    dword savedEntryCount; 
    dword cacheReuseState; 
};

struct GameFactionRuntimeRecord {
    XeniteAmountQ4 xeniteCurrentQ4; 
    XeniteAmountQ4 xeniteStorageLimitQ4; 
    ResourceExtractionRateQ4PerTick xeniteExtractionRateQ4PerTick; 
    XeniteAmountQ4 xeniteExtractedTotalQ4; 
    TritiumAmountQ4 tritiumCurrentQ4; 
    TritiumAmountQ4 tritiumStorageLimitQ4; 
    ResourceExtractionRateQ4PerTick tritiumExtractionRateQ4PerTick; 
    TritiumAmountQ4 tritiumExtractedTotalQ4; 
    EnergyAmountQ4 baselineEnergySupplyQ4; 
    EnergyAmountQ4 energyGenerationCapacityQ4; 
    EnergyDemandQ4 suppliedEnergyDemandQ4; 
    EnergyDemandQ4 unpoweredEnergyDemandQ4; 
    FactionArmyAssetCount primaryArmyAssetCount; 
    FactionArmyAssetCount secondaryArmyAssetCount; 
    dword factionClassOrMode; 
    FactionCapabilityFlags capabilityFlags; 
    FactionPackedRelationStates packedRelationStates; 
    FactionRelationCapabilityState relationCapabilityState; 
    FactionContributionScaleQ8 terrainContributionScaleQ8; 
    FactionRelationTick relationTransitionTick; 
    GraphicsWorldCoordinateQ12 primaryAnchorYQ12; 
    GraphicsWorldCoordinateQ12 primaryAnchorXQ12; 
    FactionAnchorCooldownTicks primaryAnchorCooldown; 
    GraphicsWorldCoordinateQ12 secondaryAnchorYQ12; 
    GraphicsWorldCoordinateQ12 secondaryAnchorXQ12; 
    FactionAnchorCooldownTicks anchorCooldown0; 
    FactionAnchorCooldownTicks anchorCooldown1; 
    FactionAnchorCooldownTicks anchorCooldown2; 
    FactionRuntimeFlags runtimeFlags; 
    FactionAiPressureScore maximumAiPressure; 
    byte reserved78_87[16]; 
    FactionProgressScore combinedProgressScore; 
    FactionProgressScore activeArmyContribution; 
    FactionProgressScore economyProgressScore; 
    FactionProgressScore relationScore; 
    TerrainExploredPercent exploredTerrainPercent; 
    FactionTechnologyCount unlockedTechnologyCountBeyondBaseline; 
    FactionResourceScoreComponent primaryResourceComponent; 
    FactionResourceScoreComponent secondaryResourceComponent; 
    FactionRelationCounter relationCounterA; 
    FactionRelationCounter relationCounterB; 
    FactionRelationCounter relationCounterC; 
    FactionRelationCounter relationCounterD; 
    FactionRelationCounter relationCounterE; 
    FactionRelationCounter relationCounterF; 
    struct AiFactionCandidateCacheState candidateCache; 
    dword secondaryArmyAssetPointersOrIds[64]; 
    dword primaryArmyAssetPointersOrIds[64]; 
    struct ArmyRuntimeSlot *runtimeGroupMembers8x32[256]; 
    dword technologyMasks256Bits[8]; 
    dword relationStateTicks[8]; 
    sdword aiPressureValues[8]; 
};

typedef struct GameFactionRuntimeImage GameFactionRuntimeImage, *PGameFactionRuntimeImage;

typedef struct GameFactionRuntimeImageTail GameFactionRuntimeImageTail, *PGameFactionRuntimeImageTail;

typedef dword InGameSimulationTick;

typedef dword InGamePresentationTick;

typedef dword InGamePeriodicClockTick;

typedef dword GameSpeedQ8;

typedef dword GameRelationUiFlags;

struct GameFactionRuntimeImageTail {
    byte reserved00_03[4]; 
    enum FactionRuntimeLifecycleObservedState factionLifecycleStates[8]; 
    InGameSimulationTick simulationTick; 
    InGamePresentationTick presentationTick; 
    InGamePeriodicClockTick periodicClockTick; 
    GameSpeedQ8 gameSpeedQ8; 
    GameRelationUiFlags relationUiFlags; 
};

struct GameFactionRuntimeImage {
    struct GameFactionRuntimeRecord records[8]; 
    struct GameFactionRuntimeImageTail tail; 
};

typedef struct GameDataAuxState GameDataAuxState, *PGameDataAuxState;

struct GameDataAuxState {
    dword pairPressureMatrix8x8[64]; 
};

typedef struct KeyboardAsciiCaseTransformCallbackTable3 KeyboardAsciiCaseTransformCallbackTable3, *PKeyboardAsciiCaseTransformCallbackTable3;

struct KeyboardAsciiCaseTransformCallbackTable3 {
    void (*compareCaseInsensitiveFlags)(dword, dword); 
    dword (*toUpper)(dword); 
    dword (*toLower)(dword); 
};

typedef struct RandomGeneratorState RandomGeneratorState, *PRandomGeneratorState;

struct RandomGeneratorState {
    dword (*next)(void); 
    RandomSeed primarySeed; 
    RandomSeed secondarySeed; 
};

typedef struct UiTextButtonControl UiTextButtonControl, *PUiTextButtonControl;

struct UiTextButtonControl {
    struct UiSelectableControl selectable; 
    UiTextResourceId textResourceId; 
    UiPackedTextStyle packedTextStyle; 
    dword activationSoundId; 
};

typedef struct UiCommandRuntimeRecordPrefix UiCommandRuntimeRecordPrefix, *PUiCommandRuntimeRecordPrefix;

struct UiCommandRuntimeRecordPrefix {
    byte reserved00_07[8]; 
    enum PckArmyAssetIdCatalog armyAssetId; 
    byte reserved0C_1B[16]; 
    struct GraphicsTextureSourceAsset *textureSource; 
    dword reserved20; 
    ArmyBuildDurationQ5 buildDurationQ5; 
    ArmyBuildXeniteCostQ4 buildXeniteCostQ4; 
};

typedef struct UiCatalogEntryControl UiCatalogEntryControl, *PUiCatalogEntryControl;

typedef struct UiCommandSpriteButtonControl UiCommandSpriteButtonControl, *PUiCommandSpriteButtonControl;

typedef struct UiSpriteButtonControl UiSpriteButtonControl, *PUiSpriteButtonControl;

typedef struct UiSpriteButtonDrawOffsets UiSpriteButtonDrawOffsets, *PUiSpriteButtonDrawOffsets;

struct UiSpriteButtonDrawOffsets {
    sbyte normalX; 
    sbyte normalY; 
    sbyte selectedX; 
    sbyte selectedY; 
};

struct UiSpriteButtonControl {
    struct UiSelectableControl selectable; 
    struct GraphicsTextureSourceAsset *primaryTextureSource; 
    dword normalSubresourceStartOrDescriptor; 
    struct UiSpriteButtonDrawOffsets drawOffsets; 
    GraphicsSubresourceIndex selectedSubresourceStart; 
    GraphicsSubresourceEndIndex normalSubresourceEndExclusive; 
    GraphicsSubresourceEndIndex selectedSubresourceEndExclusive; 
    GraphicsSubresourceOffset animationFrameOffset; 
    dword activationSoundId; 
    struct GraphicsTextureSourceAsset *alternateTextureSource; 
};

struct UiCommandSpriteButtonControl {
    struct UiSpriteButtonControl sprite; 
    enum UiCommandActivationStateFlags activationInputState; 
};

struct UiCatalogEntryControl {
    struct UiCommandSpriteButtonControl command; 
    dword runtimeDisplayValueQ4; 
};

typedef struct UiDirtyRectEntry UiDirtyRectEntry, *PUiDirtyRectEntry;

struct UiDirtyRectEntry {
    struct UiRootNode *rootNode; 
    struct UiRootNode *rootNodeCopy; 
    GraphicsScreenCoordinate left; 
    GraphicsScreenCoordinate top; 
    GraphicsScreenCoordinate right; 
    GraphicsScreenCoordinate bottom; 
};

typedef struct UiImageControl UiImageControl, *PUiImageControl;

struct UiImageControl {
    struct UiSelectableControl selectable; 
    struct GraphicsTextureSourceAsset *textureSource; 
    GraphicsSubresourceIndex normalSubresource; 
    dword keyboardActivationSoundId; 
    GraphicsSubresourceIndex alternateSubresource; 
    struct UiNodeBase *activeChild; 
    dword pointerActivationSoundId; 
};

typedef struct UiFramedTextButtonControl UiFramedTextButtonControl, *PUiFramedTextButtonControl;

struct UiFramedTextButtonControl {
    struct UiSelectableControl selectable; 
    UiTextResourceId textResourceId; 
    UiPackedTextStyle packedTextStyle; 
    dword activationSoundId; 
};

typedef struct UiActionHandlerPage UiActionHandlerPage, *PUiActionHandlerPage;

struct UiActionHandlerPage {
    void *handlers[256]; 
};

typedef struct UiActionQueueEntry UiActionQueueEntry, *PUiActionQueueEntry;

struct UiActionQueueEntry {
    UiActionId actionId; 
    void *source; 
};

typedef struct UiTooltipState UiTooltipState, *PUiTooltipState;

struct UiTooltipState {
    UiFrameCount countdownFrames; 
    struct UiNodeBase *targetNode; 
    UiPixelCoordinate pointerX; 
    UiPixelCoordinate pointerY; 
};

typedef enum UiPointerCaptureButton {
    UI_POINTER_CAPTURE_LEFT=0,
    UI_POINTER_CAPTURE_MIDDLE=1,
    UI_POINTER_CAPTURE_RIGHT=2,
    UI_POINTER_CAPTURE_NONE=255
} UiPointerCaptureButton;

typedef struct UiRuntimeRecord UiRuntimeRecord, *PUiRuntimeRecord;

struct UiRuntimeRecord {
    struct UiTransferPacketHeader packetHeader; 
    byte payload10_FF[240]; 
};

typedef struct UiTransferMailboxState UiTransferMailboxState, *PUiTransferMailboxState;

struct UiTransferMailboxState {
    void *outgoingAllocation; 
    UiTransferPayloadByteCount outgoingByteCount; 
    void *receivedAllocation; 
    UiTransferPayloadByteCount receivedByteCount; 
    UiTransferRemainingByteCount receivedRemainingBytes; 
    UiTransferRetryTickCount receiveRetryTicks; 
};

typedef struct FrontendDisplaySettingsPageOptionState1010 FrontendDisplaySettingsPageOptionState1010, *PFrontendDisplaySettingsPageOptionState1010;

typedef struct FrontendDisplayAdapterRows208 FrontendDisplayAdapterRows208, *PFrontendDisplayAdapterRows208;

typedef struct FrontendDisplayResolutionRows410 FrontendDisplayResolutionRows410, *PFrontendDisplayResolutionRows410;

typedef struct FrontendDisplayColorDepthRows1A0 FrontendDisplayColorDepthRows1A0, *PFrontendDisplayColorDepthRows1A0;

typedef struct FrontendDisplayAdapterOptionRow68 FrontendDisplayAdapterOptionRow68, *PFrontendDisplayAdapterOptionRow68;

typedef struct FrontendDisplayResolutionOptionRow68 FrontendDisplayResolutionOptionRow68, *PFrontendDisplayResolutionOptionRow68;

typedef struct FrontendDisplayColorDepthOptionRow68 FrontendDisplayColorDepthOptionRow68, *PFrontendDisplayColorDepthOptionRow68;

struct FrontendDisplayResolutionOptionRow68 {
    FrontendDisplayDimensionPixels width; 
    FrontendDisplayDimensionPixels height; 
    byte reserved0008_0067[96]; 
};

struct FrontendDisplayResolutionRows410 {
    struct FrontendDisplayResolutionOptionRow68 rows[10]; 
};

struct FrontendDisplayAdapterOptionRow68 {
    word *adapterDescriptionUtf16; 
    word *deviceNameUtf16; 
    byte reserved0008_0067[96]; 
};

struct FrontendDisplayAdapterRows208 {
    struct FrontendDisplayAdapterOptionRow68 rows[5]; 
};

struct FrontendDisplayColorDepthOptionRow68 {
    dword bitsPerPixel; 
    byte reserved0004_0067[100]; 
};

struct FrontendDisplayColorDepthRows1A0 {
    struct FrontendDisplayColorDepthOptionRow68 rows[4]; 
};

struct FrontendDisplaySettingsPageOptionState1010 {
    byte reserved0000_07AF[1968]; 
    struct FrontendDisplayAdapterRows208 adapterRows; 
    byte reserved09B8_0A0B[84]; 
    struct FrontendDisplayResolutionRows410 resolutionRows; 
    byte reserved0E1C_0E6F[84]; 
    struct FrontendDisplayColorDepthRows1A0 colorDepthRows; 
};

typedef union FrontendUiDisplayModeAndTaskAssignmentScratch280 FrontendUiDisplayModeAndTaskAssignmentScratch280, *PFrontendUiDisplayModeAndTaskAssignmentScratch280;

union FrontendUiDisplayModeAndTaskAssignmentScratch280 {
    byte raw[640]; 
    struct FrontendTaskAssignmentGeneratedFactionTextState280 taskAssignmentText; 
    union FrontendDisplayModeScratch28 displayModeScratch; 
    struct FrontendDisplayModeEnumerationState280 displayEnumeration; 
};

typedef struct UiSelectableOptionRow60 UiSelectableOptionRow60, *PUiSelectableOptionRow60;

struct UiSelectableOptionRow60 {
    struct UiSelectableControl control; 
    byte reserved0054_005F[12]; 
};

typedef struct FrontendGraphicsSecondaryRows120 FrontendGraphicsSecondaryRows120, *PFrontendGraphicsSecondaryRows120;

struct FrontendGraphicsSecondaryRows120 {
    struct UiSelectableOptionRow60 rows[3]; 
};

typedef struct UiSelectableOptionRow68 UiSelectableOptionRow68, *PUiSelectableOptionRow68;

struct UiSelectableOptionRow68 {
    struct UiSelectableControl control; 
    byte reserved0054_0067[20]; 
};

typedef struct InGameGraphicsRuntimeSettingsPageState12D0 InGameGraphicsRuntimeSettingsPageState12D0, *PInGameGraphicsRuntimeSettingsPageState12D0;

typedef struct FrontendGraphicsQualityRows270 FrontendGraphicsQualityRows270, *PFrontendGraphicsQualityRows270;

struct FrontendGraphicsQualityRows270 {
    struct UiSelectableOptionRow68 rows[6]; 
};

struct InGameGraphicsRuntimeSettingsPageState12D0 {
    struct UiNodeBase base; 
    byte reserved4C_CBB[3184]; 
    struct UiSelectableControl shadingEnabledControl; 
    byte reserved0D10_0D6F[96]; 
    struct FrontendGraphicsQualityRows270 shadingResolutionRows; 
    byte reserved0FE0_114B[364]; 
    dword polygonResolutionLodThresholdQ8; 
    byte reserved1150_11AF[96]; 
    struct FrontendGraphicsSecondaryRows120 textureResolutionRows; 
};

typedef struct FrontendGraphicsRuntimeSettingsPageState167C FrontendGraphicsRuntimeSettingsPageState167C, *PFrontendGraphicsRuntimeSettingsPageState167C;

struct FrontendGraphicsRuntimeSettingsPageState167C {
    struct UiNodeBase base; 
    byte reserved4C_1067[4124]; 
    struct UiSelectableControl shadingEnabledControl; 
    byte reserved10BC_111B[96]; 
    struct FrontendGraphicsQualityRows270 shadingResolutionRows; 
    byte reserved138C_14F7[364]; 
    dword polygonResolutionLodThresholdQ8; 
    byte reserved14FC_155B[96]; 
    struct FrontendGraphicsSecondaryRows120 textureResolutionRows; 
};

typedef struct InGamePlayerStatusTextSlot InGamePlayerStatusTextSlot, *PInGamePlayerStatusTextSlot;

struct InGamePlayerStatusTextSlot {
    word text[64]; 
};

typedef struct UiRequiredTextEditControl UiRequiredTextEditControl, *PUiRequiredTextEditControl;

struct UiRequiredTextEditControl {
    struct UiNodeBase base; 
    enum UiRequiredTextEditStateFlags editStateFlags; 
    UiActionId actionId; 
    UiPixelOffset horizontalScrollPixels; 
    UiTextCodeUnitCount bufferCapacityCodeUnits; 
    UiTextCodeUnitIndex cursorIndex; 
    UiTextCodeUnitIndex selectionStart; 
    UiTextCodeUnitIndex selectionEnd; 
    struct DirectSoundVoiceSet *activationSound; 
    word textPrefix6C[10]; 
};

typedef struct UiPathTextEditControl UiPathTextEditControl, *PUiPathTextEditControl;

struct UiPathTextEditControl {
    struct UiNodeBase base; 
    enum UiTextEditStateFlags editStateFlags; 
    UiActionId actionId; 
    UiPixelOffset horizontalScrollPixels; 
    dword reservedOrCapacity58; 
    UiTextCodeUnitIndex cursorIndex; 
    UiTextCodeUnitIndex selectionStart; 
    UiTextCodeUnitIndex selectionEnd; 
    struct DirectSoundVoiceSet *activationSound; 
    word pathBuffer[256]; 
};

typedef struct UiDisplayModeSelectionActionHandlerTable20 UiDisplayModeSelectionActionHandlerTable20, *PUiDisplayModeSelectionActionHandlerTable20;

struct UiDisplayModeSelectionActionHandlerTable20 {
    void (*handlers[20])(struct UiNodeBase *); 
};

typedef struct FrontendUiActionHandlerPage20Prefix86 FrontendUiActionHandlerPage20Prefix86, *PFrontendUiActionHandlerPage20Prefix86;

struct FrontendUiActionHandlerPage20Prefix86 {
    void (*handlers00_54[85])(void *); 
    undefined8 (*handler55)(dword, dword, dword, dword); 
};

typedef struct InGameUiActionHandlerPage12Prefix28 InGameUiActionHandlerPage12Prefix28, *PInGameUiActionHandlerPage12Prefix28;

struct InGameUiActionHandlerPage12Prefix28 {
    void (*handlers[28])(void *); 
};

typedef struct InGameUiActionHandlerPage10Prefix40 InGameUiActionHandlerPage10Prefix40, *PInGameUiActionHandlerPage10Prefix40;

struct InGameUiActionHandlerPage10Prefix40 {
    void (*handlers[40])(void *); 
};

typedef struct InGameUiCommandModeActionHandlerPage11Prefix3 InGameUiCommandModeActionHandlerPage11Prefix3, *PInGameUiCommandModeActionHandlerPage11Prefix3;

struct InGameUiCommandModeActionHandlerPage11Prefix3 {
    void (*handlers[3])(void *); 
};

typedef struct FrontendTaskAssignmentControlOffsetRow1C FrontendTaskAssignmentControlOffsetRow1C, *PFrontendTaskAssignmentControlOffsetRow1C;

struct FrontendTaskAssignmentControlOffsetRow1C {
    dword offsets[7]; 
};

typedef struct FrontendTaskAssignmentControlOffsetTablesA8 FrontendTaskAssignmentControlOffsetTablesA8, *PFrontendTaskAssignmentControlOffsetTablesA8;

struct FrontendTaskAssignmentControlOffsetTablesA8 {
    struct FrontendTaskAssignmentControlOffsetRow1C assignmentControls; 
    struct FrontendTaskAssignmentControlOffsetRow1C playerControls; 
    struct FrontendTaskAssignmentControlOffsetRow1C factionControls; 
    struct FrontendTaskAssignmentControlOffsetRow1C selectionRows; 
    struct FrontendTaskAssignmentControlOffsetRow1C statusRows; 
    struct FrontendTaskAssignmentControlOffsetRow1C primaryAndPadding; 
};

typedef struct UiCommandDispatchRecord UiCommandDispatchRecord, *PUiCommandDispatchRecord;

struct UiCommandDispatchRecord {
    dword commandCode; 
    dword modifierClassFlags; 
    ContinuationEntryAddress32 continuationEntryAddress; 
};

typedef struct DynamicModuleEntry DynamicModuleEntry, *PDynamicModuleEntry;

struct DynamicModuleEntry {
    HINSTANCE module; 
    char *name; 
};

typedef struct DynamicApiBinding DynamicApiBinding, *PDynamicApiBinding;

struct DynamicApiBinding {
    void **destination; 
    char *moduleName; 
};

typedef struct ArenaBlockHeader ArenaBlockHeader, *PArenaBlockHeader;

struct ArenaBlockHeader {
    dword payloadSize; 
    enum ArenaBlockStateMagic stateMagic; 
    struct ArenaBlockHeader *next; 
    struct ArenaBlockHeader *previous; 
    byte alignmentPadding10_1F[16]; 
};

typedef struct TimerCallbackTable TimerCallbackTable, *PTimerCallbackTable;

struct TimerCallbackTable {
    void (*callbacks[32])(void); 
};

typedef struct CommandLineState CommandLineState, *PCommandLineState;

struct CommandLineState {
    char executablePath[256];
    char argument1[256];
    char argument2[256];
    char argument3[256];
    char optionBuffer[256]; 
};

typedef struct ArenaState ArenaState, *PArenaState;

struct ArenaState {
    void *processHeap; 
    void *rawAllocation; 
    struct ArenaBlockHeader *firstBlock; 
    byte *linearCursor; 
    byte *linearLimit; 
};

typedef struct TH_WNDCLASSA TH_WNDCLASSA, *PTH_WNDCLASSA;

struct TH_WNDCLASSA {
    dword style;
    LRESULT (*lpfnWndProc)(HWND, dword, WPARAM, LPARAM); 
    sdword cbClsExtra;
    sdword cbWndExtra;
    HINSTANCE hInstance;
    void *hIcon;
    void *hCursor;
    void *hbrBackground;
    char *lpszMenuName;
    char *lpszClassName;
};

typedef struct MemoryApiTable MemoryApiTable, *PMemoryApiTable;

struct MemoryApiTable {
    void * (*alloc)(dword); 
    void (*free)(void *); 
    void * (*allocLargestFreeBlock)(void); 
    void (*shrinkInPlace)(dword, void *); 
    dword (*queryFreeBytes)(void); 
    void * (*reserveLinear)(dword); 
};

typedef struct ArmyPlacementContactCallbackTable5 ArmyPlacementContactCallbackTable5, *PArmyPlacementContactCallbackTable5;

struct ArmyPlacementContactCallbackTable5 {
    void (*callbacks[5])(Q12, Q12, Q12, struct ModelRuntimeNode *, struct WorldRuntimeContext *); 
};

typedef struct DIPROPHEADER DIPROPHEADER, *PDIPROPHEADER;

struct DIPROPHEADER {
    TH_LEGACY_DWORD dwSize;
    TH_LEGACY_DWORD dwHeaderSize;
    TH_LEGACY_DWORD dwObj;
    TH_LEGACY_DWORD dwHow;
};

typedef struct PALETTEENTRY_DX6 PALETTEENTRY_DX6, *PPALETTEENTRY_DX6;

struct PALETTEENTRY_DX6 {
    ColorChannelByte peRed; 
    ColorChannelByte peGreen; 
    ColorChannelByte peBlue; 
    PaletteEntryFlagsByte peFlags; 
};

typedef struct DIDATAFORMAT DIDATAFORMAT, *PDIDATAFORMAT;

typedef struct DIOBJECTDATAFORMAT DIOBJECTDATAFORMAT, *PDIOBJECTDATAFORMAT;

struct DIOBJECTDATAFORMAT {
    struct TH_LEGACY_GUID *pguid;
    TH_LEGACY_DWORD dwOfs;
    TH_LEGACY_DWORD dwType;
    TH_LEGACY_DWORD dwFlags;
};

struct DIDATAFORMAT {
    TH_LEGACY_DWORD dwSize;
    TH_LEGACY_DWORD dwObjSize;
    TH_LEGACY_DWORD dwFlags;
    TH_LEGACY_DWORD dwDataSize;
    TH_LEGACY_DWORD dwNumObjs;
    struct DIOBJECTDATAFORMAT *rgodf;
};

typedef struct DIMOUSESTATE_4BUTTON DIMOUSESTATE_4BUTTON, *PDIMOUSESTATE_4BUTTON;

struct DIMOUSESTATE_4BUTTON {
    TH_LEGACY_LONG lX;
    TH_LEGACY_LONG lY;
    TH_LEGACY_LONG lZ;
    TH_LEGACY_BYTE rgbButtons[4];
};

typedef struct D3DRECT_DX6 D3DRECT_DX6, *PD3DRECT_DX6;

struct D3DRECT_DX6 {
    GraphicsScreenCoordinate x1; 
    GraphicsScreenCoordinate y1; 
    GraphicsScreenCoordinate x2; 
    GraphicsScreenCoordinate y2; 
};

typedef struct DIDEVICEOBJECTDATA_DX6 DIDEVICEOBJECTDATA_DX6, *PDIDEVICEOBJECTDATA_DX6;

struct DIDEVICEOBJECTDATA_DX6 {
    TH_LEGACY_DWORD dwOfs;
    TH_LEGACY_DWORD dwData;
    TH_LEGACY_DWORD dwTimeStamp;
    TH_LEGACY_DWORD dwSequence;
    TH_LEGACY_ULONG uAppData; 
};

typedef struct DIDEVICEOBJECTDATA_DX3 DIDEVICEOBJECTDATA_DX3, *PDIDEVICEOBJECTDATA_DX3;

struct DIDEVICEOBJECTDATA_DX3 {
    TH_LEGACY_DWORD dwOfs;
    TH_LEGACY_DWORD dwData;
    TH_LEGACY_DWORD dwTimeStamp;
    TH_LEGACY_DWORD dwSequence;
};

typedef struct DIPROPDWORD DIPROPDWORD, *PDIPROPDWORD;

struct DIPROPDWORD {
    struct DIPROPHEADER diph;
    TH_LEGACY_DWORD dwData;
};

typedef struct IDirectDraw2 IDirectDraw2, *PIDirectDraw2;

typedef struct IDirectDraw2_Vtbl IDirectDraw2_Vtbl, *PIDirectDraw2_Vtbl;

struct IDirectDraw2_Vtbl {
    TH_LEGACY_HRESULT (*QueryInterface)(struct IDirectDraw2 *, struct TH_LEGACY_GUID *, TH_LEGACY_LPVOID *);
    TH_LEGACY_ULONG (*AddRef)(struct IDirectDraw2 *);
    TH_LEGACY_ULONG (*Release)(struct IDirectDraw2 *);
    TH_LEGACY_HRESULT (*Compact)(struct IDirectDraw2 *);
    TH_LEGACY_HRESULT (*CreateClipper)(struct IDirectDraw2 *, TH_LEGACY_DWORD, TH_LEGACY_LPVOID *, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*CreatePalette)(struct IDirectDraw2 *, TH_LEGACY_DWORD, TH_LEGACY_LPVOID, TH_LEGACY_LPVOID *, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*CreateSurface)(struct IDirectDraw2 *, struct DDSURFACEDESC_DX6 *, struct IDirectDrawSurface **, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*DuplicateSurface)(struct IDirectDraw2 *, struct IDirectDrawSurface *, struct IDirectDrawSurface **);
    sdword (*EnumDisplayModes)(struct IDirectDraw2 *, dword, struct DDSURFACEDESC_DX6 *, dword, sdword (*)(struct DDSURFACEDESC_DX6 *, dword)); 
    TH_LEGACY_HRESULT (*EnumSurfaces)(struct IDirectDraw2 *, TH_LEGACY_DWORD, struct DDSURFACEDESC_DX6 *, TH_LEGACY_LPVOID, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*FlipToGDISurface)(struct IDirectDraw2 *);
    TH_LEGACY_HRESULT (*GetCaps)(struct IDirectDraw2 *, TH_LEGACY_LPVOID, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*GetDisplayMode)(struct IDirectDraw2 *, struct DDSURFACEDESC_DX6 *);
    TH_LEGACY_HRESULT (*GetFourCCCodes)(struct IDirectDraw2 *, TH_LEGACY_DWORD *, TH_LEGACY_DWORD *);
    TH_LEGACY_HRESULT (*GetGDISurface)(struct IDirectDraw2 *, struct IDirectDrawSurface **);
    TH_LEGACY_HRESULT (*GetMonitorFrequency)(struct IDirectDraw2 *, TH_LEGACY_DWORD *);
    TH_LEGACY_HRESULT (*GetScanLine)(struct IDirectDraw2 *, TH_LEGACY_DWORD *);
    TH_LEGACY_HRESULT (*GetVerticalBlankStatus)(struct IDirectDraw2 *, TH_LEGACY_BOOL *);
    TH_LEGACY_HRESULT (*Initialize)(struct IDirectDraw2 *, struct TH_LEGACY_GUID *);
    TH_LEGACY_HRESULT (*RestoreDisplayMode)(struct IDirectDraw2 *);
    TH_LEGACY_HRESULT (*SetCooperativeLevel)(struct IDirectDraw2 *, TH_LEGACY_HWND, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*SetDisplayMode)(struct IDirectDraw2 *, TH_LEGACY_DWORD, TH_LEGACY_DWORD, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*WaitForVerticalBlank)(struct IDirectDraw2 *, TH_LEGACY_DWORD, TH_LEGACY_HANDLE);
    TH_LEGACY_HRESULT (*GetAvailableVidMem)(struct IDirectDraw2 *, struct DDSCAPS *, TH_LEGACY_DWORD *, TH_LEGACY_DWORD *);
};

struct IDirectDraw2 {
    struct IDirectDraw2_Vtbl *lpVtbl;
};

typedef struct IDirectInputDeviceA IDirectInputDeviceA, *PIDirectInputDeviceA;

typedef struct IDirectInputDeviceA_Vtbl IDirectInputDeviceA_Vtbl, *PIDirectInputDeviceA_Vtbl;

typedef void *TH_LEGACY_HINSTANCE;

struct IDirectInputDeviceA {
    struct IDirectInputDeviceA_Vtbl *lpVtbl;
};

struct IDirectInputDeviceA_Vtbl {
    TH_LEGACY_HRESULT (*QueryInterface)(struct IDirectInputDeviceA *, struct TH_LEGACY_GUID *, TH_LEGACY_LPVOID *);
    TH_LEGACY_ULONG (*AddRef)(struct IDirectInputDeviceA *);
    TH_LEGACY_ULONG (*Release)(struct IDirectInputDeviceA *);
    TH_LEGACY_HRESULT (*GetCapabilities)(struct IDirectInputDeviceA *, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*EnumObjects)(struct IDirectInputDeviceA *, TH_LEGACY_LPVOID, TH_LEGACY_LPVOID, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*GetProperty)(struct IDirectInputDeviceA *, struct TH_LEGACY_GUID *, struct DIPROPHEADER *);
    TH_LEGACY_HRESULT (*SetProperty)(struct IDirectInputDeviceA *, struct TH_LEGACY_GUID *, struct DIPROPHEADER *);
    TH_LEGACY_HRESULT (*Acquire)(struct IDirectInputDeviceA *);
    TH_LEGACY_HRESULT (*Unacquire)(struct IDirectInputDeviceA *);
    TH_LEGACY_HRESULT (*GetDeviceState)(struct IDirectInputDeviceA *, TH_LEGACY_DWORD, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*GetDeviceData)(struct IDirectInputDeviceA *, TH_LEGACY_DWORD, struct DIDEVICEOBJECTDATA_DX3 *, TH_LEGACY_DWORD *, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*SetDataFormat)(struct IDirectInputDeviceA *, struct DIDATAFORMAT *);
    TH_LEGACY_HRESULT (*SetEventNotification)(struct IDirectInputDeviceA *, TH_LEGACY_HANDLE);
    TH_LEGACY_HRESULT (*SetCooperativeLevel)(struct IDirectInputDeviceA *, TH_LEGACY_HWND, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*GetObjectInfo)(struct IDirectInputDeviceA *, TH_LEGACY_LPVOID, TH_LEGACY_DWORD, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*GetDeviceInfo)(struct IDirectInputDeviceA *, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*RunControlPanel)(struct IDirectInputDeviceA *, TH_LEGACY_HWND, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*Initialize)(struct IDirectInputDeviceA *, TH_LEGACY_HINSTANCE, TH_LEGACY_DWORD, struct TH_LEGACY_GUID *);
};

typedef struct IDirectInputA_Vtbl IDirectInputA_Vtbl, *PIDirectInputA_Vtbl;

typedef struct IDirectInputA IDirectInputA, *PIDirectInputA;

struct IDirectInputA_Vtbl {
    TH_LEGACY_HRESULT (*QueryInterface)(struct IDirectInputA *, struct TH_LEGACY_GUID *, TH_LEGACY_LPVOID *);
    TH_LEGACY_ULONG (*AddRef)(struct IDirectInputA *);
    TH_LEGACY_ULONG (*Release)(struct IDirectInputA *);
    TH_LEGACY_HRESULT (*CreateDevice)(struct IDirectInputA *, struct TH_LEGACY_GUID *, struct IDirectInputDeviceA **, TH_LEGACY_LPVOID);
    TH_LEGACY_HRESULT (*EnumDevices)(struct IDirectInputA *, TH_LEGACY_DWORD, TH_LEGACY_LPVOID, TH_LEGACY_LPVOID, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*GetDeviceStatus)(struct IDirectInputA *, struct TH_LEGACY_GUID *);
    TH_LEGACY_HRESULT (*RunControlPanel)(struct IDirectInputA *, TH_LEGACY_HWND, TH_LEGACY_DWORD);
    TH_LEGACY_HRESULT (*Initialize)(struct IDirectInputA *, TH_LEGACY_HINSTANCE, TH_LEGACY_DWORD);
};

struct IDirectInputA {
    struct IDirectInputA_Vtbl *lpVtbl;
};

typedef qword WorldPositionXYRegisterPairQ12;

typedef byte FactionRuntimeLifecycleState;

typedef qword ModelRuntimeScaleRatioRegisterPairQ12;

typedef qword ModelRuntimeAttachmentCollectionRegisterPair;

typedef qword ModelRuntimeActiveTotalMetricRegisterPair;

typedef qword ShotAimXZRegisterPairQ12;

typedef uint DisplayModeScratchWord;

typedef int FieldGridRadiusUnits;

typedef dword ArmyTerrainContactMode;

typedef dword UQ8;

typedef dword ArmyAssetId;

typedef int FieldGridAccumulatorValue;

typedef dword SelectionPlayerPairKey;

typedef dword FieldGridTransitionValue;

typedef int FieldGridRowStrideBytes;

typedef dword FieldGridCommandReservedValue;

typedef dword TechnologyId;

typedef int FieldGridMaterialBitIndex;

typedef int FieldGridByteOffset;

typedef int FieldGridHeightDeltaUnits;

typedef dword SelectionPlayerPairValue;

typedef dword PackedFieldGridDeltaXY16;

typedef struct Win32SystemTime16 Win32SystemTime16, *PWin32SystemTime16;

struct Win32SystemTime16 {
    Win32CalendarYear16 year; 
    Win32CalendarMonth16 month; 
    Win32DayOfWeek16 dayOfWeek; 
    Win32CalendarDay16 day; 
    Win32Hour16 hour; 
    Win32Minute16 minute; 
    Win32Second16 second; 
    Win32Millisecond16 milliseconds; 
};

typedef struct LocaleSystemState LocaleSystemState, *PLocaleSystemState;

struct LocaleSystemState {
    struct Win32SystemTime16 localTime; 
    dword languageIdentifierDigits; 
    word decimalSeparator[16]; 
    word thousandsSeparator[16]; 
    word numberGrouping[16]; 
    dword negativeNumberFormat; 
    word dateSeparator[16]; 
    word timeSeparator[16]; 
    dword longDateOrder; 
    dword timeFormat24Hour; 
    word amDesignator[16]; 
    word pmDesignator[16]; 
};

typedef enum RuntimeSpinLockValue {
    SPIN_LOCK_UNLOCKED=0,
    SPIN_LOCK_LOCKED=4294967295
} RuntimeSpinLockValue;

typedef struct TimerSystemState TimerSystemState, *PTimerSystemState;

struct TimerSystemState {
    void (*callbacks[32])(void); 
    dword winmmTimerIds[32]; 
};

typedef struct Win32Message32 Win32Message32, *PWin32Message32;

struct Win32Message32 {
    void *hwnd; 
    Win32MessageId32 message; 
    Win32WParam32 wParam; 
    Win32LParam32 lParam; 
    Win32MessageTimestamp32 time; 
    Win32CursorCoordinate32 pointX; 
    Win32CursorCoordinate32 pointY; 
};

typedef struct PcxPreview64 PcxPreview64, *PPcxPreview64;

typedef struct PcxRgb24 PcxRgb24, *PPcxRgb24;

struct PcxRgb24 {
    ColorChannelByte red; 
    ColorChannelByte green; 
    ColorChannelByte blue; 
};

struct PcxPreview64 {
    struct PcxRgb24 paletteRgbTriplets256[256]; 
    byte indexedPixels64x64[4096]; 
};

typedef struct TerrainClassPlacementAndOverlayCallbackTable10 TerrainClassPlacementAndOverlayCallbackTable10, *PTerrainClassPlacementAndOverlayCallbackTable10;

struct TerrainClassPlacementAndOverlayCallbackTable10 {
    int (*placementTests[5])(dword, Q12, Q12, Q12, struct FieldGridAsset *); 
    void (*overlayCallbacks[5])(dword, int, dword, Q12, Q12, struct FieldGridAsset *); 
};

typedef struct FieldGridInterpolationCallbackTable5 FieldGridInterpolationCallbackTable5, *PFieldGridInterpolationCallbackTable5;

struct FieldGridInterpolationCallbackTable5 {
    Q12 (*callbacks[5])(Q12, Q12, struct FieldGridAsset *); 
};

typedef struct TechnologyCategoryMasks TechnologyCategoryMasks, *PTechnologyCategoryMasks;

struct TechnologyCategoryMasks {
    dword category2[8]; 
    dword category3[8]; 
};

typedef struct TechnologyRecord TechnologyRecord, *PTechnologyRecord;

struct TechnologyRecord {
    dword prerequisiteMasks[8]; 
    TechnologyXeniteCostQ4 xeniteCostQ4; 
    TechnologyEnergyCostQ4 energyCostQ4; 
    TechnologyResearchDurationQ5 researchDurationQ5; 
    enum PckTechnologyIdCatalog dependencyTechnologyIndex; 
    UiTextResourceId completionMessageResourceId; 
    enum TechnologyCategory category; 
    AiTechnologyCandidateScore baseCandidateScore; 
    dword reserved3C; 
};

typedef struct TechnologyAssetHeader TechnologyAssetHeader, *PTechnologyAssetHeader;

struct TechnologyAssetHeader {
    struct GeneratedAssetCommonPrefix common; 
    byte reservedB0_1FF[336]; 
};

typedef struct TechnologyAsset TechnologyAsset, *PTechnologyAsset;

struct TechnologyAsset {
    struct TechnologyAssetHeader header; 
    struct TechnologyRecord records[1]; 
};

typedef struct ArmyAssetHeader ArmyAssetHeader, *PArmyAssetHeader;

typedef struct GeneratedAssetRecordCountHeader GeneratedAssetRecordCountHeader, *PGeneratedAssetRecordCountHeader;

struct GeneratedAssetRecordCountHeader {
    struct GeneratedAssetCommonPrefix common;
    AssetRecordCount recordCount;
};

struct ArmyAssetHeader {
    struct GeneratedAssetRecordCountHeader recordCountHeader;
    byte reservedB4_1FF[332];
};

typedef struct GeneratedAssetEntryCountHeader GeneratedAssetEntryCountHeader, *PGeneratedAssetEntryCountHeader;

struct GeneratedAssetEntryCountHeader {
    struct GeneratedAssetCommonPrefix common;
    AssetRecordCount entryCount;
};

typedef struct GraphicsTextureSourceLifecycleCallbackTable3 GraphicsTextureSourceLifecycleCallbackTable3, *PGraphicsTextureSourceLifecycleCallbackTable3;

struct GraphicsTextureSourceLifecycleCallbackTable3 {
    void (*releasePackage)(struct GraphicsTextureSourceAsset *); 
    GraphicsTextureSourceAsset * (*clone)(struct GraphicsTextureSourceAsset *); 
    void (*releaseClone)(struct GraphicsTextureSourceAsset *); 
};

typedef struct GraphicsPaletteAssetLifecycleCallbackTable3 GraphicsPaletteAssetLifecycleCallbackTable3, *PGraphicsPaletteAssetLifecycleCallbackTable3;

struct GraphicsPaletteAssetLifecycleCallbackTable3 {
    void (*releasePackage)(struct GraphicsPaletteAsset *); 
    GraphicsPaletteAsset * (*clone)(struct GraphicsPaletteAsset *); 
    void (*releaseClone)(struct GraphicsPaletteAsset *); 
};

typedef struct RomAssetHeader RomAssetHeader, *PRomAssetHeader;

struct RomAssetHeader {
    struct GeneratedAssetRecordCountHeader recordCountHeader;
    byte reservedB4_1FF[332];
};

typedef struct RomRegistrySlot RomRegistrySlot, *PRomRegistrySlot;

typedef struct RomAssetRecordPrefix RomAssetRecordPrefix, *PRomAssetRecordPrefix;

struct RomRegistrySlot {
    struct RomAssetRecordPrefix *record; 
    struct WorldRuntimeNode *runtimeRootNode; 
};

struct RomAssetRecordPrefix {
    RomRecordByteSize byteSize; 
    dword rootNodeOffsetOrPointer; 
    RomRecordId recordId; 
};

typedef struct ModelPackedPointRecord ModelPackedPointRecord, *PModelPackedPointRecord;

struct ModelPackedPointRecord {
    uint packedLookupKey; 
    struct GraphicsFixedVec3 localPosition; 
};

typedef struct ModelAssetHeader ModelAssetHeader, *PModelAssetHeader;

struct ModelAssetHeader {
    struct GeneratedAssetRecordCountHeader recordCountHeader;
    byte reservedB4_1FF[332];
};

typedef struct ModelRuntimeSlotAssetView200 ModelRuntimeSlotAssetView200, *PModelRuntimeSlotAssetView200;

struct ModelRuntimeSlotAssetView200 {
    byte opaque[512]; 
};

typedef struct EffectAssetHeader EffectAssetHeader, *PEffectAssetHeader;

struct EffectAssetHeader {
    struct GeneratedAssetEntryCountHeader entryCountHeader;
    byte reservedB4_1FF[332];
};

typedef struct EffectLifecycleTransitionDispatchTable5 EffectLifecycleTransitionDispatchTable5, *PEffectLifecycleTransitionDispatchTable5;

struct EffectLifecycleTransitionDispatchTable5 {
    void *spawnLinkedEffectAfterCountdownEntry; 
    void *advancePeriodicEmissionAndCompletionActionEntry; 
    void *integrateLinearMotionAndShadingPositionEntry; 
    void *noAdditionalActionEntry; 
    void *advanceTerrainRelativeMotionAndTerminateOnContactEntry; 
};

typedef struct ShotAssetHeader ShotAssetHeader, *PShotAssetHeader;

struct ShotAssetHeader {
    struct GeneratedAssetEntryCountHeader entryCountHeader;
    byte reservedB4_1FF[332];
};

typedef char *TH_LEGACY_LPCSTR;

typedef char *TH_LEGACY_LPSTR;

typedef dword TH_LEGACY_UINT;

typedef void *TH_LEGACY_LPCVOID;

typedef struct CommandLineArgumentMirrorState500 CommandLineArgumentMirrorState500, *PCommandLineArgumentMirrorState500;

struct CommandLineArgumentMirrorState500 {
    char executablePath[256]; 
    char argument1[256]; 
    char argument2[256]; 
    char argument3[256]; 
    char optionBuffer[256]; 
};

typedef struct CommandLineWideArguments CommandLineWideArguments, *PCommandLineWideArguments;

struct CommandLineWideArguments {
    word argument1[256]; 
    word argument2[256]; 
    word argument3[256]; 
};

typedef struct FrontendPlayerFactionAssignmentState10 FrontendPlayerFactionAssignmentState10, *PFrontendPlayerFactionAssignmentState10;

struct FrontendPlayerFactionAssignmentState10 {
    FrontendReadyOrWaitState readyOrWaitState; 
    FrontendFactionAssignmentIndex factionAssignmentIndex; 
    FrontendConsensusValue consensusValue; 
    FrontendRoleStateFlags roleStateFlags; 
};

typedef struct FrontendPlayerNameUtf16_28 FrontendPlayerNameUtf16_28, *PFrontendPlayerNameUtf16_28;

struct FrontendPlayerNameUtf16_28 {
    word textUtf16[20]; 
};

typedef struct FrontendNetworkBackendCommonPrefix44 FrontendNetworkBackendCommonPrefix44, *PFrontendNetworkBackendCommonPrefix44;

typedef struct FrontendNetworkSettingsPageCommonPrefix44 FrontendNetworkSettingsPageCommonPrefix44, *PFrontendNetworkSettingsPageCommonPrefix44;

struct FrontendNetworkSettingsPageCommonPrefix44 {
    struct UiNodeBase *nextSibling; 
    struct UiNodeBase *firstChild; 
    struct UiNodeBase *parent; 
    struct UiNodeVtable *vtable; 
    sdword left; 
    sdword top; 
    sdword right; 
    sdword bottom; 
    sdword leftOffset; 
    sdword topOffset; 
    sdword rightOffset; 
    sdword bottomOffset; 
    UiAnchorFractionQ31 leftAnchorQ31; 
    UiAnchorFractionQ31 topAnchorQ31; 
    UiAnchorFractionQ31 rightAnchorQ31; 
    UiAnchorFractionQ31 bottomAnchorQ31; 
    sdword layoutWidth; 
};

struct FrontendNetworkBackendCommonPrefix44 {
    struct FrontendNetworkSettingsPageCommonPrefix44 commonPrefix; 
};

typedef union FrontendNetworkSettingsControlView250 FrontendNetworkSettingsControlView250, *PFrontendNetworkSettingsControlView250;

typedef struct FrontendNetworkSettingsPageCommonState250 FrontendNetworkSettingsPageCommonState250, *PFrontendNetworkSettingsPageCommonState250;

typedef struct FrontendNetworkSettingsUiNodeView250 FrontendNetworkSettingsUiNodeView250, *PFrontendNetworkSettingsUiNodeView250;

typedef struct FrontendNetworkSettingsTextEditView250 FrontendNetworkSettingsTextEditView250, *PFrontendNetworkSettingsTextEditView250;

typedef struct FrontendNetworkSettingsPointerListView250 FrontendNetworkSettingsPointerListView250, *PFrontendNetworkSettingsPointerListView250;

typedef struct FrontendNetworkSettingsPrimaryPageStackView250 FrontendNetworkSettingsPrimaryPageStackView250, *PFrontendNetworkSettingsPrimaryPageStackView250;

typedef struct FrontendNetworkSettingsSecondaryPageStackView250 FrontendNetworkSettingsSecondaryPageStackView250, *PFrontendNetworkSettingsSecondaryPageStackView250;

typedef struct FrontendNetworkSettingsGeneratedNameBufferView250 FrontendNetworkSettingsGeneratedNameBufferView250, *PFrontendNetworkSettingsGeneratedNameBufferView250;

typedef struct FrontendNetworkBackendModePageState250 FrontendNetworkBackendModePageState250, *PFrontendNetworkBackendModePageState250;

typedef union FrontendNetworkBackendModeOverlap44 FrontendNetworkBackendModeOverlap44, *PFrontendNetworkBackendModeOverlap44;

typedef struct FrontendNetworkGeneratedNamePrefix2E FrontendNetworkGeneratedNamePrefix2E, *PFrontendNetworkGeneratedNamePrefix2E;

struct FrontendNetworkSettingsPrimaryPageStackView250 {
    byte reserved0000_0067[104]; 
    struct UiPageStackControl pageStack; 
    byte reserved00BC_024F[404]; 
};

struct FrontendNetworkSettingsGeneratedNameBufferView250 {
    byte reserved0000_0177[376]; 
    word generatedNameUtf16[40]; 
    byte reserved01C8_024F[136]; 
};

struct FrontendNetworkSettingsUiNodeView250 {
    struct UiNodeBase base; 
    byte reserved004C_024F[516]; 
};

struct FrontendNetworkGeneratedNamePrefix2E {
    byte backendModeMarker; 
    byte reserved01_05[5]; 
    char generatedName[40]; 
};

union FrontendNetworkBackendModeOverlap44 {
    struct FrontendNetworkBackendCommonPrefix44 commonPrefix; 
    struct FrontendNetworkGeneratedNamePrefix2E generatedNamePrefix; 
    byte raw[68]; 
};

struct FrontendNetworkBackendModePageState250 {
    union FrontendNetworkBackendModeOverlap44 backendModeOverlap; 
    byte reserved44_24F[524]; 
};

struct FrontendNetworkSettingsTextEditView250 {
    byte reserved0000_0043[68]; 
    struct UiTextEditControl textEdit; 
    byte reserved00C4_024F[396]; 
};

struct FrontendNetworkSettingsPointerListView250 {
    byte reserved0000_005B[92]; 
    struct UiPointerListControl pointerList; 
    byte reserved00C0_024F[400]; 
};

struct FrontendNetworkSettingsPageCommonState250 {
    struct FrontendNetworkSettingsPageCommonPrefix44 commonPrefix; 
    byte reserved0044_024F[524]; 
};

struct FrontendNetworkSettingsSecondaryPageStackView250 {
    byte reserved0000_0147[328]; 
    struct UiPageStackControl pageStack; 
    byte reserved019C_024F[180]; 
};

union FrontendNetworkSettingsControlView250 {
    byte raw[592]; 
    struct FrontendNetworkSettingsPageCommonState250 commonState; 
    struct FrontendNetworkSettingsUiNodeView250 nodeView; 
    struct FrontendNetworkSettingsTextEditView250 textEditView; 
    struct FrontendNetworkSettingsPointerListView250 pointerListView; 
    struct FrontendNetworkSettingsPrimaryPageStackView250 primaryPageStackView; 
    struct FrontendNetworkSettingsSecondaryPageStackView250 secondaryPageStackView; 
    struct FrontendNetworkSettingsGeneratedNameBufferView250 generatedNameBufferView; 
    struct FrontendNetworkBackendModePageState250 backendModeState; 
};

typedef struct FrontendPlayerRuntimeRecord FrontendPlayerRuntimeRecord, *PFrontendPlayerRuntimeRecord;

struct FrontendPlayerRuntimeRecord {
    dword runtimeState00; 
    UiTransferSequenceToken peerSequenceToken; 
    dword runtimeState08;
    dword runtimeState0C;
    FrontendHeartbeatTickCount heartbeatExpiryTicks; 
    FrontendPlayerRuntimeId playerRuntimeId; 
    struct FrontendPlayerNameUtf16_28 playerName; 
    struct UiTransferEndpointDescriptor endpoint; 
    enum FrontendCommandSyncPendingState commandSyncPending; 
    struct FrontendPlayerFactionAssignmentState10 factionAssignment; 
    dword runtimeState64;
    enum FrontendSnapshotTransferFlags snapshotTransferFlags; 
    FrontendSnapshotChunkByteOffset snapshotChunkOffset; 
    dword runtimeState70;
    FrontendCapabilityFlags capabilityFlags; 
    byte reserved78_7F[8];
    FrontendCapturedStateFlag capturedFlag80; 
    dword callbackValue84;
    dword callbackValue88;
    dword callbackValue8C;
    byte reserved90_AF[32];
    byte snapshotPayloadB0_13AF[4864]; 
};

typedef struct FrontendCommandPacketRecord FrontendCommandPacketRecord, *PFrontendCommandPacketRecord;

struct FrontendCommandPacketRecord {
    struct UiTransferPacketHeader header; 
    struct UiCommandQueueRecord command; 
};

typedef struct FrontendPlayerRemovalPacket10007 FrontendPlayerRemovalPacket10007, *PFrontendPlayerRemovalPacket10007;

struct FrontendPlayerRemovalPacket10007 {
    struct UiTransferPacketHeader header; 
    FrontendPlayerRuntimeId removedPlayerToken; 
    byte reservedPayload14_1F[12]; 
};

typedef union FrontendTransferPacketUnion FrontendTransferPacketUnion, *PFrontendTransferPacketUnion;

typedef struct FrontendPacket10000Handshake FrontendPacket10000Handshake, *PFrontendPacket10000Handshake;

typedef struct FrontendPacket20002PlayerDescriptor FrontendPacket20002PlayerDescriptor, *PFrontendPacket20002PlayerDescriptor;

typedef struct FrontendPacket10003JoinAck FrontendPacket10003JoinAck, *PFrontendPacket10003JoinAck;

typedef struct FrontendPacket10004PlayerSnapshotRequest FrontendPacket10004PlayerSnapshotRequest, *PFrontendPacket10004PlayerSnapshotRequest;

typedef struct FrontendPacket30005PlayerSnapshot FrontendPacket30005PlayerSnapshot, *PFrontendPacket30005PlayerSnapshot;

typedef struct FrontendPacket10006CapabilityHeartbeat FrontendPacket10006CapabilityHeartbeat, *PFrontendPacket10006CapabilityHeartbeat;

typedef struct FrontendPacket40008LobbyRosterSnapshot FrontendPacket40008LobbyRosterSnapshot, *PFrontendPacket40008LobbyRosterSnapshot;

typedef struct FrontendPacket10009SnapshotChunkRequest FrontendPacket10009SnapshotChunkRequest, *PFrontendPacket10009SnapshotChunkRequest;

typedef struct FrontendPacket8000ASnapshotChunk FrontendPacket8000ASnapshotChunk, *PFrontendPacket8000ASnapshotChunk;

typedef struct FrontendPacket10012SyncPending FrontendPacket10012SyncPending, *PFrontendPacket10012SyncPending;

typedef struct FrontendPacket10013HeartbeatAck FrontendPacket10013HeartbeatAck, *PFrontendPacket10013HeartbeatAck;

typedef struct FrontendPacket10032HostValue FrontendPacket10032HostValue, *PFrontendPacket10032HostValue;

typedef struct FrontendPacket10022StatePending FrontendPacket10022StatePending, *PFrontendPacket10022StatePending;

typedef struct FrontendPacket10023StateAck FrontendPacket10023StateAck, *PFrontendPacket10023StateAck;

struct FrontendPacket20002PlayerDescriptor {
    struct UiTransferPacketHeader header; 
    UiTransferPayloadByteCount payloadByteCount; 
    dword reserved14; 
    dword playerDescriptorPayload[10]; 
};

struct FrontendPacket8000ASnapshotChunk {
    struct UiTransferPacketHeader header; 
    dword reserved10; 
    FrontendSnapshotChunkByteOffset snapshotChunkOffset; 
    byte g_FrontendPacket10009Buffer[232]; 
};

struct FrontendPacket10003JoinAck {
    struct UiTransferPacketHeader header; 
    FrontendPlayerRuntimeId assignedPlayerRuntimeId; 
    FrontendNetworkTickInterval networkTickInterval; 
    byte reserved18_1F[8]; 
};

struct FrontendPacket40008LobbyRosterSnapshot {
    struct UiTransferPacketHeader header; 
    FrontendPlayerCount pendingSessionPlayerCount; 
    FrontendStatusCode selectedStatusCode0; 
    FrontendStatusCode selectedStatusCode1; 
    dword reserved1C; 
    FrontendPlayerIndex selectedPlayerIndex; 
    FrontendPlayerCount playerCount; 
    FrontendPlayerRuntimeId selectedPlayerRuntimeId; 
    byte reserved2C_37[12]; 
    dword playerDescriptorPayload[10]; 
    word selectedPlayerStatusTextUtf16[16]; 
};

struct FrontendPacket10032HostValue {
    struct UiTransferPacketHeader header; 
    FrontendBackendSessionValue backendSessionValue; 
    byte reserved14_1F[12]; 
};

struct FrontendPacket10000Handshake {
    struct UiTransferPacketHeader header; 
    FrontendProtocolMagic protocolMagic2931; 
    byte reserved14_1F[12]; 
};

struct FrontendPacket30005PlayerSnapshot {
    struct UiTransferPacketHeader header; 
    FrontendPlayerIndex playerIndex; 
    FrontendPlayerRuntimeId playerRuntimeId; 
    dword playerDescriptorPayload[10]; 
    struct UiTransferEndpointDescriptor endpoint; 
    FrontendRandomSeed secondaryRandomSeed; 
    FrontendReadyOrWaitState readyOrWaitState; 
    FrontendFactionAssignmentIndex factionAssignmentIndex; 
    FrontendConsensusValue consensusValue; 
};

struct FrontendPacket10012SyncPending {
    struct UiTransferPacketHeader header; 
    byte reserved10_1F[16]; 
};

struct FrontendPacket10013HeartbeatAck {
    struct UiTransferPacketHeader header; 
    byte reserved10_1F[16]; 
};

struct FrontendPacket10004PlayerSnapshotRequest {
    struct UiTransferPacketHeader header; 
    FrontendPlayerIndex requestedPlayerIndex; 
    byte reserved14_1F[12]; 
};

struct FrontendPacket10023StateAck {
    struct UiTransferPacketHeader header; 
    byte reserved10_1F[16]; 
};

struct FrontendPacket10022StatePending {
    struct UiTransferPacketHeader header; 
    byte reserved10_1F[16]; 
};

struct FrontendPacket10009SnapshotChunkRequest {
    struct UiTransferPacketHeader header; 
    dword reserved10; 
    FrontendSnapshotChunkByteOffset snapshotChunkOffset; 
    byte reserved18_1F[8]; 
};

struct FrontendPacket10006CapabilityHeartbeat {
    struct UiTransferPacketHeader header; 
    FrontendCapabilityFlags capabilityFlags; 
    FrontendHeartbeatTickCount heartbeatExpiryTicks; 
    byte reserved18_1F[8]; 
};

union FrontendTransferPacketUnion {
    struct FrontendPacket10000Handshake packet10000Handshake; 
    struct FrontendPacket50001SessionAdvertisement packet50001SessionAdvertisement; 
    struct FrontendPacket20002PlayerDescriptor packet20002PlayerDescriptor; 
    struct FrontendPacket10003JoinAck packet10003JoinAck; 
    struct FrontendPacket10004PlayerSnapshotRequest packet10004PlayerSnapshotRequest; 
    struct FrontendPacket30005PlayerSnapshot packet30005PlayerSnapshot; 
    struct FrontendPacket10006CapabilityHeartbeat packet10006CapabilityHeartbeat; 
    struct FrontendPacket40008LobbyRosterSnapshot packet40008LobbyRosterSnapshot; 
    struct FrontendPacket10009SnapshotChunkRequest packet10009SnapshotChunkRequest; 
    struct FrontendPacket8000ASnapshotChunk packet8000ASnapshotChunk; 
    struct FrontendPacket10012SyncPending packet10012SyncPending; 
    struct FrontendPacket10013HeartbeatAck packet10013HeartbeatAck; 
    struct FrontendPacket10032HostValue packet10032HostValue; 
    struct FrontendPacket10022StatePending packet10022StatePending; 
    struct FrontendPacket10023StateAck packet10023StateAck; 
    struct FrontendPlayerRemovalPacket10007 playerRemoval10007; 
    struct FrontendCommandPacketRecord command10011Or10021; 
    struct UiTransferPacket genericTransferPacket; 
};

typedef struct ScenarioCatalogHeader ScenarioCatalogHeader, *PScenarioCatalogHeader;

struct ScenarioCatalogHeader {
    ScenarioCatalogByteOffset levelRecordsOffset; 
    ScenarioCatalogByteOffset campaignRecordsOffset; 
    ScenarioCatalogByteOffset saveRecordsOffset; 
    ScenarioCatalogRecordCount levelRecordCount; 
    ScenarioCatalogRecordCount campaignRecordCount; 
    ScenarioCatalogRecordCount saveRecordCount; 
};

typedef struct ScenarioCatalogRecord ScenarioCatalogRecord, *PScenarioCatalogRecord;

struct ScenarioCatalogRecord {
    word identifier[32]; 
    byte metadata40_BF[128]; 
    word timestampText[32]; 
};

typedef ULONG_PTR SIZE_T;

typedef DWORD *LPDWORD;

typedef HINSTANCE HMODULE;

typedef HANDLE HLOCAL;

typedef long *LPLONG;

typedef struct HMENU__ HMENU__, *PHMENU__;

typedef struct HMENU__ *HMENU;

struct HMENU__ {
    int unused;
};

typedef struct _FILETIME *LPFILETIME;

typedef int (*FARPROC)(void);

typedef WORD *LPWORD;

typedef WORD ATOM;

typedef void *LPCVOID;

typedef struct IMAGE_OPTIONAL_HEADER32 IMAGE_OPTIONAL_HEADER32, *PIMAGE_OPTIONAL_HEADER32;

typedef struct IMAGE_DATA_DIRECTORY IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

struct IMAGE_DATA_DIRECTORY {
    ImageBaseOffset32 VirtualAddress;
    dword Size;
};

struct IMAGE_OPTIONAL_HEADER32 {
    word Magic;
    byte MajorLinkerVersion;
    byte MinorLinkerVersion;
    dword SizeOfCode;
    dword SizeOfInitializedData;
    dword SizeOfUninitializedData;
    ImageBaseOffset32 AddressOfEntryPoint;
    ImageBaseOffset32 BaseOfCode;
    ImageBaseOffset32 BaseOfData;
    pointer32 ImageBase;
    dword SectionAlignment;
    dword FileAlignment;
    word MajorOperatingSystemVersion;
    word MinorOperatingSystemVersion;
    word MajorImageVersion;
    word MinorImageVersion;
    word MajorSubsystemVersion;
    word MinorSubsystemVersion;
    dword Win32VersionValue;
    dword SizeOfImage;
    dword SizeOfHeaders;
    dword CheckSum;
    word Subsystem;
    word DllCharacteristics;
    dword SizeOfStackReserve;
    dword SizeOfStackCommit;
    dword SizeOfHeapReserve;
    dword SizeOfHeapCommit;
    dword LoaderFlags;
    dword NumberOfRvaAndSizes;
    struct IMAGE_DATA_DIRECTORY DataDirectory[16];
};

typedef struct IMAGE_RESOURCE_DIRECTORY_ENTRY_NameStruct IMAGE_RESOURCE_DIRECTORY_ENTRY_NameStruct, *PIMAGE_RESOURCE_DIRECTORY_ENTRY_NameStruct;

struct IMAGE_RESOURCE_DIRECTORY_ENTRY_NameStruct {
    dword NameOffset:31;
    dword NameIsString:1;
};

typedef struct IMAGE_FILE_HEADER IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

struct IMAGE_FILE_HEADER {
    word Machine; 
    word NumberOfSections;
    dword TimeDateStamp;
    dword PointerToSymbolTable;
    dword NumberOfSymbols;
    word SizeOfOptionalHeader;
    word Characteristics;
};

typedef struct IMAGE_NT_HEADERS32 IMAGE_NT_HEADERS32, *PIMAGE_NT_HEADERS32;

struct IMAGE_NT_HEADERS32 {
    char Signature[4];
    struct IMAGE_FILE_HEADER FileHeader;
    struct IMAGE_OPTIONAL_HEADER32 OptionalHeader;
};

typedef struct IMAGE_RESOURCE_DIRECTORY_ENTRY IMAGE_RESOURCE_DIRECTORY_ENTRY, *PIMAGE_RESOURCE_DIRECTORY_ENTRY;

typedef union IMAGE_RESOURCE_DIRECTORY_ENTRY_NameUnion IMAGE_RESOURCE_DIRECTORY_ENTRY_NameUnion, *PIMAGE_RESOURCE_DIRECTORY_ENTRY_NameUnion;

union IMAGE_RESOURCE_DIRECTORY_ENTRY_NameUnion {
    struct IMAGE_RESOURCE_DIRECTORY_ENTRY_NameStruct IMAGE_RESOURCE_DIRECTORY_ENTRY_NameStruct;
    dword Name;
    word Id;
};

struct IMAGE_RESOURCE_DIRECTORY_ENTRY {
    union IMAGE_RESOURCE_DIRECTORY_ENTRY_NameUnion NameUnion;
    union IMAGE_RESOURCE_DIRECTORY_ENTRY_DirectoryUnion DirectoryUnion;
};

typedef struct IMAGE_SECTION_HEADER IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;

typedef union Misc Misc, *PMisc;

typedef enum SectionFlags {
    IMAGE_SCN_TYPE_NO_PAD=8,
    IMAGE_SCN_RESERVED_0001=16,
    IMAGE_SCN_CNT_CODE=32,
    IMAGE_SCN_CNT_INITIALIZED_DATA=64,
    IMAGE_SCN_CNT_UNINITIALIZED_DATA=128,
    IMAGE_SCN_LNK_OTHER=256,
    IMAGE_SCN_LNK_INFO=512,
    IMAGE_SCN_RESERVED_0040=1024,
    IMAGE_SCN_LNK_REMOVE=2048,
    IMAGE_SCN_LNK_COMDAT=4096,
    IMAGE_SCN_GPREL=32768,
    IMAGE_SCN_MEM_16BIT=131072,
    IMAGE_SCN_MEM_PURGEABLE=131072,
    IMAGE_SCN_MEM_LOCKED=262144,
    IMAGE_SCN_MEM_PRELOAD=524288,
    IMAGE_SCN_ALIGN_1BYTES=1048576,
    IMAGE_SCN_ALIGN_2BYTES=2097152,
    IMAGE_SCN_ALIGN_4BYTES=3145728,
    IMAGE_SCN_ALIGN_8BYTES=4194304,
    IMAGE_SCN_ALIGN_16BYTES=5242880,
    IMAGE_SCN_ALIGN_32BYTES=6291456,
    IMAGE_SCN_ALIGN_64BYTES=7340032,
    IMAGE_SCN_ALIGN_128BYTES=8388608,
    IMAGE_SCN_ALIGN_256BYTES=9437184,
    IMAGE_SCN_ALIGN_512BYTES=10485760,
    IMAGE_SCN_ALIGN_1024BYTES=11534336,
    IMAGE_SCN_ALIGN_2048BYTES=12582912,
    IMAGE_SCN_ALIGN_4096BYTES=13631488,
    IMAGE_SCN_ALIGN_8192BYTES=14680064,
    IMAGE_SCN_LNK_NRELOC_OVFL=16777216,
    IMAGE_SCN_MEM_DISCARDABLE=33554432,
    IMAGE_SCN_MEM_NOT_CACHED=67108864,
    IMAGE_SCN_MEM_NOT_PAGED=134217728,
    IMAGE_SCN_MEM_SHARED=268435456,
    IMAGE_SCN_MEM_EXECUTE=536870912,
    IMAGE_SCN_MEM_READ=1073741824,
    IMAGE_SCN_MEM_WRITE=2147483648
} SectionFlags;

union Misc {
    dword PhysicalAddress;
    dword VirtualSize;
};

struct IMAGE_SECTION_HEADER {
    char Name[8];
    union Misc Misc;
    ImageBaseOffset32 VirtualAddress;
    dword SizeOfRawData;
    dword PointerToRawData;
    dword PointerToRelocations;
    dword PointerToLinenumbers;
    word NumberOfRelocations;
    word NumberOfLinenumbers;
    enum SectionFlags Characteristics;
};

typedef struct IMAGE_RESOURCE_DATA_ENTRY IMAGE_RESOURCE_DATA_ENTRY, *PIMAGE_RESOURCE_DATA_ENTRY;

struct IMAGE_RESOURCE_DATA_ENTRY {
    dword OffsetToData;
    dword Size;
    dword CodePage;
    dword Reserved;
};

typedef struct IMAGE_RESOURCE_DIRECTORY IMAGE_RESOURCE_DIRECTORY, *PIMAGE_RESOURCE_DIRECTORY;

struct IMAGE_RESOURCE_DIRECTORY {
    dword Characteristics;
    dword TimeDateStamp;
    word MajorVersion;
    word MinorVersion;
    word NumberOfNamedEntries;
    word NumberOfIdEntries;
};

#endif /* THANDOR_GENERATED_V523_TYPES_H */

/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/contracts.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_CONTRACTS_H
#define THANDOR_CORE_CONTRACTS_H

/*
Core contracts shared by the split submodules.
Q12: 0x1000 == 1.0. Q4 resource values use 16 units per displayed unit. Q5 research time uses 32 units per displayed unit.
FieldGridCell is 0x80 bytes. FLD flags/material at +0x50 and runtime occupancy at +0x70 are separate namespaces.
LEV file offsets and the loaded LevelAsset overlay are separate representations.
Faction runtime index, frontend player index, player-runtime ID, ARM ID, MDL ID and TEC ID are separate identity domains.
*/

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;
typedef unsigned long long qword;
typedef int Q12;
typedef unsigned int UQ12;


#endif /* THANDOR_CORE_CONTRACTS_H */

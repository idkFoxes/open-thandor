/* Integrated map editor entry point. The implementation lives under mapeditor/ and is intentionally
   invoked only for /mapeditor, -mapeditor or --mapeditor. */
#ifndef THANDOR_MAPEDITOR_ENTRY_H
#define THANDOR_MAPEDITOR_ENTRY_H

/* Large enough for a full editor terrain/object frame.  The bootstrap initializes the shared
   open-thandor primitive queue to this capacity before entering the editor. */
#define THANDOR_MAPEDITOR_PRIMITIVE_QUEUE_PACKET_COUNT 0x40000U

int ThandorMapEditor_Run();

#endif

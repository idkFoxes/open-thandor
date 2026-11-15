Map editor project format readers (.thmap), used by the custom maps of the game
(src/assets/scenario/custom_project.cpp, docs/custom_maps.md).

Origin: the portable project-format readers of the Thandor Map Editor 0.9.781 (github.com/idkFoxes/thandor-map-editor),
as copied into the open-thandor fork "OpenThandor_ZipMaps" (based on v1.0.7, mapeditor/project_format/). Only the
reader is taken; no editor renderer or simulation code is linked into the game. ZIP projects are read with miniz
(third_party/miniz, MIT licence, see third_party/miniz/LICENSE).

Changes for open-thandor (marked "open-thandor" in the code):
  - map_project_archive: MapProjectArchive::parse (the bytes of a file, so the self-test can fuzz in memory);
    limits 32 MiB per project file, 16 MiB per entry, 64 MiB decoded in total (the editor: 64 / 32 / 128 MiB);
  - text_resource: TextResourceAsset::string_units (the raw UTF-16 units of a string slot; the game filters the
    rich-text commands itself).
Everything else is unchanged; the writers (stored PCK, ZIP, TGA) are not used by the game.

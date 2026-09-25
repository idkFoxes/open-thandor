// Writes the disassembly of the named functions to <outdir>/<name>.asm.
// Used to recover values the decompiler lost (stale register arguments, preserved registers).
//
// Headless:
//   analyzeHeadless <tmpdir> thandor -import ghidra/thandor.exeV537.gzf -noanalysis ^
//       -scriptPath tools/ghidra -postScript DumpDisassembly.java <outdir> <FunctionName>... -deleteProject
//@category open-thandor

import java.io.File;
import java.io.PrintWriter;

import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;
import ghidra.program.model.symbol.Symbol;

public class DumpDisassembly extends GhidraScript {

    @Override
    protected void run() throws Exception {
        String[] args = getScriptArgs();
        File out = new File(args[0]);
        out.mkdirs();
        java.util.List<String> names = new java.util.ArrayList<>();
        for (int i = 1; i < args.length; i++) {
            if (args[i].equals("*")) { // every non-external function
                for (Function g : currentProgram.getFunctionManager().getFunctions(true)) {
                    names.add(g.getName());
                }
            } else {
                names.add(args[i]);
            }
        }
        for (String name : names) {
            Function f = getGlobalFunctions(name).stream().findFirst().orElse(null);
            if (f == null) {
                println("not found: " + name);
                continue;
            }
            try (PrintWriter w = new PrintWriter(new File(out, name + ".asm"), "UTF-8")) {
                InstructionIterator it = currentProgram.getListing().getInstructions(f.getBody(), true);
                while (it.hasNext()) {
                    Instruction ins = it.next();
                    Symbol label = getSymbolAt(ins.getAddress());
                    if (label != null && !label.getName().equals(f.getName())) {
                        w.println(label.getName() + ":");
                    }
                    StringBuilder refs = new StringBuilder();
                    for (var ref : ins.getReferencesFrom()) {
                        Symbol s = getSymbolAt(ref.getToAddress());
                        if (s != null) {
                            refs.append(refs.length() == 0 ? "  ; " : ", ").append(s.getName());
                        }
                    }
                    w.println("  " + ins.getAddress() + "  " + ins + refs);
                }
            }
            println("dumped " + name);
        }
    }
}

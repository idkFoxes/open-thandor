// Exports what Ghidra's "Export C" leaves out, as JSON lines, for tools/gen_globals.py:
//   function_definitions.jsonl  every FunctionDefinition data type (the *Proc / *Callback types)
//   strings.jsonl               every defined string with its label, address and value
//   symbols.jsonl               every labeled defined data item with address, type and length
//   imports.jsonl               every external (DLL) function with library and signature
//   functions.jsonl             every function with entry address and prototype
//
// Headless:
//   analyzeHeadless <tmpdir> thandor -import ghidra/thandor.exeV537.gzf -noanalysis ^
//       -scriptPath tools/ghidra -postScript ExportBuildData.java <outdir> -deleteProject
//@category open-thandor

import java.io.File;
import java.io.PrintWriter;
import java.util.Iterator;

import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.DataType;
import ghidra.program.model.data.FunctionDefinition;
import ghidra.program.model.data.ParameterDefinition;
import ghidra.program.model.listing.Data;
import ghidra.program.model.listing.DataIterator;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Parameter;
import ghidra.program.model.symbol.ExternalLocation;
import ghidra.program.model.symbol.Symbol;

public class ExportBuildData extends GhidraScript {

    @Override
    protected void run() throws Exception {
        String[] args = getScriptArgs();
        File out = new File(args.length > 0 ? args[0] : ".");
        out.mkdirs();

        int defs = 0;
        try (PrintWriter w = new PrintWriter(new File(out, "function_definitions.jsonl"), "UTF-8")) {
            Iterator<DataType> it = currentProgram.getDataTypeManager().getAllDataTypes();
            while (it.hasNext()) {
                DataType dt = it.next();
                if (!(dt instanceof FunctionDefinition fd)) {
                    continue;
                }
                StringBuilder params = new StringBuilder("[");
                ParameterDefinition[] ps = fd.getArguments();
                for (int i = 0; i < ps.length; i++) {
                    if (i > 0) {
                        params.append(',');
                    }
                    params.append("{\"type\":").append(q(ps[i].getDataType().getDisplayName()))
                          .append(",\"name\":").append(q(ps[i].getName())).append('}');
                }
                params.append(']');
                w.println("{\"name\":" + q(fd.getName())
                        + ",\"category\":" + q(fd.getCategoryPath().getPath())
                        + ",\"return\":" + q(fd.getReturnType().getDisplayName())
                        + ",\"convention\":" + q(fd.getCallingConventionName())
                        + ",\"varargs\":" + fd.hasVarArgs()
                        + ",\"noreturn\":" + fd.hasNoReturn()
                        + ",\"params\":" + params
                        + ",\"prototype\":" + q(fd.getPrototypeString(true)) + "}");
                defs++;
            }
        }

        int strings = 0;
        int symbols = 0;
        try (PrintWriter ws = new PrintWriter(new File(out, "strings.jsonl"), "UTF-8");
             PrintWriter wd = new PrintWriter(new File(out, "symbols.jsonl"), "UTF-8")) {
            DataIterator di = currentProgram.getListing().getDefinedData(true);
            while (di.hasNext() && !monitor.isCancelled()) {
                Data d = di.next();
                Symbol s = d.getPrimarySymbol();
                String label = s == null ? null : s.getName();
                String addr = d.getAddress().toString();
                if (d.hasStringValue()) {
                    Object v = d.getValue();
                    ws.println("{\"label\":" + q(label) + ",\"address\":" + q(addr)
                            + ",\"type\":" + q(d.getDataType().getDisplayName())
                            + ",\"value\":" + q(v == null ? null : v.toString()) + "}");
                    strings++;
                }
                if (label != null) {
                    wd.println("{\"name\":" + q(label) + ",\"address\":" + q(addr)
                            + ",\"type\":" + q(d.getDataType().getDisplayName())
                            + ",\"length\":" + d.getLength() + "}");
                    symbols++;
                }
            }
        }
        int imports = 0;
        int functions = 0;
        try (PrintWriter wi = new PrintWriter(new File(out, "imports.jsonl"), "UTF-8");
             PrintWriter wf = new PrintWriter(new File(out, "functions.jsonl"), "UTF-8")) {
            for (Function f : currentProgram.getFunctionManager().getExternalFunctions()) {
                ExternalLocation loc = f.getExternalLocation();
                wi.println("{\"name\":" + q(f.getName())
                        + ",\"library\":" + q(loc == null ? null : loc.getLibraryName())
                        + ",\"convention\":" + q(f.getCallingConventionName())
                        + ",\"return\":" + q(f.getReturnType().getDisplayName())
                        + ",\"params\":" + params(f)
                        + ",\"varargs\":" + f.hasVarArgs()
                        + ",\"prototype\":" + q(f.getPrototypeString(true, false)) + "}");
                imports++;
            }
            for (Function f : currentProgram.getFunctionManager().getFunctions(true)) {
                wf.println("{\"name\":" + q(f.getName())
                        + ",\"address\":" + q(f.getEntryPoint().toString())
                        + ",\"thunk\":" + f.isThunk()
                        + ",\"convention\":" + q(f.getCallingConventionName())
                        + ",\"prototype\":" + q(f.getPrototypeString(true, false)) + "}");
                functions++;
            }
        }
        println("open-thandor export: " + defs + " function definitions, " + strings + " strings, "
                + symbols + " symbols, " + imports + " imports, " + functions + " functions -> "
                + out.getAbsolutePath());
    }

    private static String params(Function f) {
        StringBuilder b = new StringBuilder("[");
        Parameter[] ps = f.getParameters();
        for (int i = 0; i < ps.length; i++) {
            if (i > 0) {
                b.append(',');
            }
            b.append("{\"type\":").append(q(ps[i].getDataType().getDisplayName()))
             .append(",\"name\":").append(q(ps[i].getName())).append('}');
        }
        return b.append(']').toString();
    }

    private static String q(String s) {
        if (s == null) {
            return "null";
        }
        StringBuilder b = new StringBuilder("\"");
        for (char c : s.toCharArray()) {
            switch (c) {
                case '"' -> b.append("\\\"");
                case '\\' -> b.append("\\\\");
                case '\n' -> b.append("\\n");
                case '\r' -> b.append("\\r");
                case '\t' -> b.append("\\t");
                default -> {
                    if (c < 0x20) {
                        b.append(String.format("\\u%04x", (int) c));
                    } else {
                        b.append(c);
                    }
                }
            }
        }
        return b.append('"').toString();
    }
}

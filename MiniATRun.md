MiniAT Assembly Guide
=======================
This guide details how to use llvm to generate MiniAT assembly.

    clang -S -emit-llvm {source file}.c

This will generate {source file}.ll. In the generate file, there should be lines of the following form:
    
    !llvm.ident = !{!0}
    !0 = metadata !{meta...
    
either delete them, or comment them out like this:
    
    ;!llvm.ident = !{!0}
    ;!0 = metadata !{meta...
    
Next, we generate the MiniAT assembly:
    
    {Path to bin}/llc -O0 -march=miniat -mcpu=miniat -filetype=asm {source file}.ll -o {source file}.asm
    
Finally, since there are still bugs in the assembler output, the MiniATFixUp script needs to be run:
    
    {Path to bin}/MiniATFixUp {source file}.asm > {source file}Final.asm
    
The resulting file can then be passed through Mash.

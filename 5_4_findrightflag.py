import os
import re
from subprocess import run
import time
o0flags = """
-fPIC -fPIE -faggressive-loop-optimizations
 -fasynchronous-unwind-tables -fauto-inc-dec -fchkp-check-incomplete-type
 -fchkp-check-read -fchkp-check-write -fchkp-instrument-calls
 -fchkp-narrow-bounds -fchkp-optimize -fchkp-store-bounds
 -fchkp-use-static-bounds -fchkp-use-static-const-bounds
 -fchkp-use-wrappers -fcommon -fdelete-null-pointer-checks
 -fdwarf2-cfi-asm -fearly-inlining -feliminate-unused-debug-types
 -fexceptions -ffp-int-builtin-inexact -ffunction-cse -fgcse-lm
 -fgnu-runtime -fgnu-unique -fident -finline-atomics -fira-hoist-pressure
 -fira-share-save-slots -fira-share-spill-slots -fivopts
 -fkeep-static-consts -fleading-underscore -flifetime-dse
 -flto-odr-type-merging -fmath-errno -fmerge-debug-strings -fpeephole
 -fplt -fprefetch-loop-arrays -freg-struct-return
 -fsched-critical-path-heuristic -fsched-dep-count-heuristic
 -fsched-group-heuristic -fsched-interblock -fsched-last-insn-heuristic
 -fsched-rank-heuristic -fsched-spec -fsched-spec-insn-heuristic
 -fsched-stalled-insns-dep -fschedule-fusion -fsemantic-interposition
 -fshow-column -fshrink-wrap-separate -fsigned-zeros
 -fsplit-ivs-in-unroller -fssa-backprop -fstdarg-opt
 -fstrict-volatile-bitfields -fsync-libcalls -ftrapping-math -ftree-cselim
 -ftree-forwprop -ftree-loop-if-convert -ftree-loop-im -ftree-loop-ivcanon
 -ftree-loop-optimize -ftree-phiprop
 -ftree-reassoc -ftree-scev-cprop -funit-at-a-time -funwind-tables
 -fverbose-asm -fzero-initialized-in-bss -m128bit-long-double -m64 -m80387
 -malign-stringops -mavx256-split-unaligned-load
 -mavx256-split-unaligned-store -mfancy-math-387 -mfp-ret-in-387 -mfxsr -mglibc -mieee-fp -mlong-double-80 -mmmx -mno-sse4 -mpush-args -mred-zone
 -msse -msse2 -mstv -mtls-direct-seg-refs -mvzeroupper""".split()

o0flags_as_needed = " ".join(o0flags)

o1flags = """
-fPIC -fPIE -faggressive-loop-optimizations
 -fasynchronous-unwind-tables -fauto-inc-dec -fbranch-count-reg
 -fchkp-check-incomplete-type -fchkp-check-read -fchkp-check-write
 -fchkp-instrument-calls -fchkp-narrow-bounds -fchkp-optimize
 -fchkp-store-bounds -fchkp-use-static-bounds
 -fchkp-use-static-const-bounds -fchkp-use-wrappers
 -fcombine-stack-adjustments -fcommon -fcompare-elim -fcprop-registers
 -fdefer-pop -fdelete-null-pointer-checks -fdwarf2-cfi-asm
 -fearly-inlining -feliminate-unused-debug-types -fexceptions
 -fforward-propagate -ffp-int-builtin-inexact -ffunction-cse -fgcse-lm
 -fgnu-runtime -fgnu-unique -fguess-branch-probability -fident
 -fif-conversion -fif-conversion2 -finline -finline-atomics
 -finline-functions-called-once -fipa-profile -fipa-pure-const
 -fipa-reference -fira-hoist-pressure -fira-share-save-slots
 -fira-share-spill-slots -fivopts -fkeep-static-consts
 -fleading-underscore -flifetime-dse -flto-odr-type-merging -fmath-errno
 -fmerge-constants -fmerge-debug-strings -fmove-loop-invariants
 -fomit-frame-pointer -fpeephole -fplt -fprefetch-loop-arrays
 -freg-struct-return -freorder-blocks -fsched-critical-path-heuristic
 -fsched-dep-count-heuristic -fsched-group-heuristic -fsched-interblock
 -fsched-last-insn-heuristic -fsched-rank-heuristic -fsched-spec
 -fsched-spec-insn-heuristic -fsched-stalled-insns-dep -fschedule-fusion
 -fsemantic-interposition -fshow-column -fshrink-wrap
 -fshrink-wrap-separate -fsigned-zeros -fsplit-ivs-in-unroller
 -fsplit-wide-types -fssa-backprop -fssa-phiopt -fstdarg-opt
 -fstrict-volatile-bitfields -fsync-libcalls -ftoplevel-reorder
 -ftrapping-math -ftree-bit-ccp -ftree-builtin-call-dce -ftree-ccp
 -ftree-ch -ftree-coalesce-vars -ftree-copy-prop -ftree-cselim -ftree-dce
 -ftree-dominator-opts -ftree-dse -ftree-forwprop -ftree-fre
 -ftree-loop-if-convert -ftree-loop-im -ftree-loop-ivcanon
 -ftree-loop-optimize -ftree-phiprop -ftree-pta
 -ftree-reassoc -ftree-scev-cprop -ftree-sink -ftree-slsr -ftree-sra
 -ftree-ter -funit-at-a-time -funwind-tables -fverbose-asm
 -fzero-initialized-in-bss -m128bit-long-double -m64 -m80387
 -malign-stringops -mavx256-split-unaligned-load
 -mavx256-split-unaligned-store -mfancy-math-387 -mfp-ret-in-387 -mfxsr
 -mglibc -mieee-fp -mlong-double-80 -mmmx -mno-sse4 -mpush-args -mred-zone
 -msse -msse2 -mstv -mtls-direct-seg-refs -mvzeroupper""".split()

o1minuso0 = [entry for entry in o1flags if (entry not in o0flags)]
print(o1minuso0)

def compile_with_each_flag_and_save_file(to_this_path):
  run("mkdir "+to_this_path,shell=True)
  for flag in o1minuso0:
    print(flag)
    to_run = f"g++ -O0 {flag} -std=c++17 4_3_betterbenchmark.cpp -o 43 && ./43 > {to_this_path}/{flag}.txt"
    print(to_run)
    run(to_run,shell=True)
    time.sleep(3)


def makereports(on_this_path,to_this_file):
  li = [on_this_path+'/'+entry for entry in os.listdir(on_this_path)]
  crit = re.compile("[0-9]+ us")
  ret = []
  for filename in li:
    with open(filename,"r") as file:
      ret.append( [filename,crit.findall(file.read())] )
  ret = sorted(ret,key=lambda x:int(x[1][0][:-2]))
  with open(to_this_file,'w') as file:
   for entry in ret:
    file.write(str(entry)+'\n')

if __name__=="__main__":
 compile_with_each_flag_and_save_file('./5_5_opts')
 makereports('./5_5_opts','5_6_optreport.txt')

# maybe I should learn to shell script

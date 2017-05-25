#*******************************************************************************
#
#     SCons main construct file
#
#     Processor: Cortex-A 
#
#     Toolkit:   arm-none-eabi
#
#     Copyright (c) 2006-2016, Harry E. Zhurov
#
#-------------------------------------------------------------------------------

import os
import sys
import glob
import subprocess
import re
import struct
import binascii
import fnmatch
from intelhex import IntelHex
from datetime import datetime

sys.path.append(os.getcwd() + '/cfg/script')

import utils
import scmIC

#===============================================================================
#
#     User definable area
#
#-------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------
#
#     Project settings
#
ProjectName  = 'fsbl'

RootDir      = '.'                          
SourceDirs   = ['src', 'src/xil', '/opt/cad/xilinx/zynq7000/ps7mmrs/const']

SourceFiles  = []

                
#--------------------------------------
#
#     Options
#
#MCU          = 'stm32f2xx'

Optimization =' -O3 '
DebugLevel   =' -g3 '          # empty to disable debug info

ExtraFlags    = ' '# + ' -DNDEBUG '
ExtraCFlags   = ' '
ExtraCXXFlags = ' '
ExtraLFlags   = ' '

#p = subprocess.Popen('git describe --tags'.split(), stdin  = subprocess.PIPE, stdout = subprocess.PIPE, stderr = subprocess.PIPE )
#out = p.communicate()[0]
rcode, out, err = utils.pexec('git describe --tags')

VersionFileContents  = '#ifndef VERSION_H' + os.linesep
VersionFileContents += '#define VERSION_H' + os.linesep
VersionFileContents += '__attribute__((used))' + os.linesep
VersionFileContents += 'static const char * SOFTWARE_REVISION = "' + out.strip() + '";' + os.linesep
VersionFileContents += '#endif' + os.linesep

print 'Build: ' + out.strip()                 

open('src/version.h', 'wb').write(VersionFileContents)

BUILD_DATE = datetime.now().strftime('%Y-%m-%d')
BUILD_TIME = datetime.now().strftime('%H:%M:%S')


#===============================================================================
#
#     !!! Service Area !!!
#
#

#-------------------------------------------------------------------------------
#
#    MCU
#
CPU = 'cortex-a9'

#-------------------------------------------------------------------------------
#
#    Project structure
#
ConfigDir   = 'cfg'
ScriptDir   = 'cfg/script'
ObjDir      = 'obj'
ListDir     = 'lst'
BinDir      = 'bin'

ServiceDirs = [ ObjDir, ListDir, BinDir ]
for i in ServiceDirs:
    curdir = os.path.join( os.getcwd(), i )
    if not os.path.exists(curdir):
        print 'Directory ' + curdir + ' does not exists. Creating the directory...',
        os.mkdir(curdir)
        print 'done'

#-------------------------------------------------------------------------------
#
#    Platform specific stuff
#
AsmExt  = 'S'
CExt    = 'c'
CppExt  = 'cpp'
ObjExt  = 'o'
LstExt  = 'lst'
MapExt  = 'map'
BinExt  = 'elf'
LdrExt  = 'ldr'
LdExt   = 'ld'

#-------------------------------------------------------------------------------
#
#      Toolkit
#
ALTERA_EABI = False

if not ALTERA_EABI:

    TOOLKIT_PATH = os.environ['CAD'] + '/gcc/arm'

    CPP      = TOOLKIT_PATH + '/bin/arm-none-eabi-cpp'
    ASM      = TOOLKIT_PATH + '/bin/arm-none-eabi-as'
    CC       = TOOLKIT_PATH + '/bin/arm-none-eabi-gcc'
    Linker   = TOOLKIT_PATH + '/bin/arm-none-eabi-ld'
    OBJDUMP  = TOOLKIT_PATH + '/bin/arm-none-eabi-objdump'
    LOADER   = TOOLKIT_PATH + '/bin/arm-none-eabi-ldr'
    SIZE     = TOOLKIT_PATH + '/bin/arm-none-eabi-size'

else:
    TOOLKIT_PATH = '/opt/cad/altera/16.1/embedded/host_tools/mentor/gnu/arm/baremetal'

    CPP      = TOOLKIT_PATH + '/bin/arm-altera-eabi-cpp'
    ASM      = TOOLKIT_PATH + '/bin/arm-altera-eabi-as'
    CC       = TOOLKIT_PATH + '/bin/arm-altera-eabi-gcc'
    Linker   = TOOLKIT_PATH + '/bin/arm-altera-eabi-ld'
    OBJDUMP  = TOOLKIT_PATH + '/bin/arm-altera-eabi-objdump'
    LOADER   = TOOLKIT_PATH + '/bin/arm-altera-eabi-ldr'
    SIZE     = TOOLKIT_PATH + '/bin/arm-altera-eabi-size'

ToolkitIncPath = [] 
ToolkitLibPath = [TOOLKIT_PATH]

#-------------------------------------------------------------------------------
#
#      Tools options
#
#----------------------------------------------------
#
#      Include, library, config files and paths
#
IncludePath = ''.join(' -I' + i for i in SourceDirs + ToolkitIncPath)
LibraryPathOptions = ''#.join(' -L ' + i for i in ToolkitLibPath)

LinkerMainScript   = os.path.join(ConfigDir, 'fsbl.' + LdExt)
                 
#-------------------------------------------------------------------------------
#
#    The options
#
FLAGS     = ' -mcpu=' + CPU #+ ' -mthumb2'
#FLAGS    += ' -march=armv7-a' 
FLAGS    += ' -mtune=cortex-a9' 
FLAGS    += ' -mfloat-abi=hard' 
FLAGS    += ' -mfpu=vfpv3' 
FLAGS    += IncludePath
FLAGS    += Optimization
FLAGS    += DebugLevel
FLAGS    += ' -pipe'
FLAGS    += ' -ffunction-sections -fdata-sections'
FLAGS    += ' -DBUILD_DATE=\"' + BUILD_DATE + '\"'
FLAGS    += ' -DBUILD_TIME=\"' + BUILD_TIME + '\"'
FLAGS    += ExtraFlags
#-----------------------------------------------------------
GCCFLAGS  = FLAGS           
GCCFLAGS += ' -c'
GCCFLAGS += ' -MD' 
GCCFLAGS += ' -DPRINTF_FLOAT -fomit-frame-pointer -ffast-math'

GCC_W_FLAGS  = ' -Wall' 
GCC_W_FLAGS += ' -Wextra'
GCC_W_FLAGS += ' -Wcast-align -Wpointer-arith -Wredundant-decls'
GCC_W_FLAGS += ' -Wshadow -Wcast-qual -pedantic'
#-----------------------------------------------------------
AFLAGS    = FLAGS + ' -c'
#-----------------------------------------------------------
CFLAGS    = GCCFLAGS 
CFLAGS   += ' -std=c99' 
CFLAGS   += ' -Wimplicit -Wnested-externs'
CFLAGS   += ExtraCFlags
#-----------------------------------------------------------
CXXFLAGS  = GCCFLAGS 
CXXFLAGS += ' -fno-exceptions -fno-rtti '
CXXFLAGS += ' -std=c++11'
CXXFLAGS += ' -funsigned-bitfields -fshort-enums'
CXXFLAGS += ExtraCXXFlags
#-----------------------------------------------------------
LFLAGS    = FLAGS
LFLAGS   += ' -Wl,--gc-sections'
if not ALTERA_EABI:
    LFLAGS   += ' --specs=nano.specs'
LFLAGS   += ' -T' + LinkerMainScript
LFLAGS   += ' -L '   + ObjDir + LibraryPathOptions
LFLAGS   += ' -nostartfiles'
LFLAGS   += ' -lm -lstdc++'
LFLAGS   += ExtraLFlags
#-------------------------------------------------------------------------------
#
#    Action functions
#
SEP_LEN = 76
#-------------------------------------------------------------------------------
#
#    Build object file from c source file
#
def compile_c(target, source, env):
    #------------------------------------------------
    #
    #   Check if the first source is compiling
    #
    if env['FIRST_ENTRY'] == False:
        env['FIRST_ENTRY'] = True
        print '*'*SEP_LEN

    #------------------------------------------------
    #
    #   Launch compiler
    #
    print 'cc: ' + os.path.basename( str(source[0]) )
    if 'xil' in str(source[0]):
        FLAGS = env['CFLAGS']
    else:
        FLAGS = env['CFLAGS'] + ' ' + env['GCC_W_FLAGS']

    cmd = env['CC'] + FLAGS + ' -o ' + str(target[0]) + ' ' + str(source[0])
    #print cmd 
    rcode, out, err = utils.pexec(cmd)
    out += err
    if out:        print out.replace('`', '\'')
    if rcode != 0: return rcode

    #------------------------------------------------
    #
    #   Create listing
    #
    cmd = env['OBJDUMP'] + ' -dCS ' + str(target[0])
    rcode, out, err = utils.pexec(cmd)
    open( os.path.join( ListDir, utils.namegen(str(source[0]), LstExt) ), 'wb' ).write(out)
#-------------------------------------------------------------------------------
#
#    Build object file from cpp source file
#
def compile_cpp(target, source, env):
    #------------------------------------------------
    #
    #   Check if the first source is compiling
    #
    if env['FIRST_ENTRY'] == False:
        env['FIRST_ENTRY'] = True
        print '*'*SEP_LEN

    #------------------------------------------------
    #
    #   Launch compiler
    #
    print 'cc: ' + os.path.basename( str(source[0]) )
    if 'xil' in str(source[0]):
        FLAGS = env['CXXFLAGS']
    else:
        FLAGS = env['CXXFLAGS'] + ' ' + env['GCC_W_FLAGS']
    cmd = env['CC'] + FLAGS + ' -o ' + str(target[0]) + ' ' + os.path.abspath(str(source[0]))
    rcode, out, err = utils.pexec(cmd)
    out += err
    if out:        print out.replace('`', '\'')
    if rcode != 0: return rcode

    #------------------------------------------------
    #
    #   Create listing
    #
    cmd = env['OBJDUMP'] + ' -dCS ' + str(target[0])
    rcode, out, err = utils.pexec(cmd)
    open( os.path.join( ListDir, utils.namegen(str(source[0]), LstExt) ), 'wb' ).write(out)
#-------------------------------------------------------------------------------
#
#    Build object file from assembler source file
#
def assembly_src(target, source, env):
    #------------------------------------------------
    #
    #   Check if the first source is compiling
    #
    if env['FIRST_ENTRY'] == False:
        env['FIRST_ENTRY'] = True
        print '*'*SEP_LEN

    #------------------------------------------------
    #
    #   Launch assembler
    #
    src_name = str(source[0])
    print 'as: ' + os.path.basename( src_name ) 
    cmd = env['CC'] + env['AFLAGS'] + ' -Wa,-adhlns=' +\
          os.path.join( ListDir, utils.namegen(str(source[0]), LstExt)) +\
          ' -o ' + str(target[0]) + ' ' + src_name
          
    #print cmd 
 
    rcode, out, err = utils.pexec(cmd)
    out += err
    if out:        print out.replace('`', '\'')
    if rcode != 0: return rcode

    #------------------------------------------------
    #
    #   Create listing
    #
    cmd = env['OBJDUMP'] + ' -dC ' + str(target[0])
    rcode, out, err = utils.pexec(cmd)
    open( os.path.join( ListDir, utils.namegen(str(source[0]), LstExt) ), 'wb' ).write(out)
#-------------------------------------------------------------------------------
#
#    Build executable file from object files
#
def build_bin(target, source, env):
    if env['FIRST_ENTRY'] == False:
        print '*'*SEP_LEN
        print 'Sources are up to date'

    print '*'*SEP_LEN
    print 'ld: ' + str(target[0])

    src_list = ''
    for i in xrange(len(source)):
        src_list += ' ' + str(source[i])

    #-------------------------------------------------------------
    #
    #    Preprocess linker script
    #
    cmd  =  env['CPP'] + ' -P -undef ' + LinkerMainScript + '.S ' + LinkerMainScript 
    rcode, out, err = utils.pexec(cmd)
    out += err

    #-------------------------------------------------------------
    #
    #    Link executable
    #
    cmd  =  env['CC'] + ' -o ' + str(target[0]) + src_list + env['LFLAGS'] +\
           ' -Wl,-Map=' + os.path.join(ListDir, utils.namegen(str(target[0]), MapExt)) + ',--cref'
           
    #print cmd
    rcode, out, err = utils.pexec(cmd)
    out += err
    if out: print out.replace('`', '\'')

    #print '*'*61
    if rcode != 0:
        return rcode

    #------------------------------------------------
    #
    #   Create listing
    #
    cmd = env['OBJDUMP'] + ' -dSC ' + str(target[0])
    rcode, out, er = utils.pexec(cmd)
    open( os.path.join( ListDir, utils.namegen(str(target[0]), LstExt) ), 'wb' ).write(out)

    #------------------------------------------------
    #
    #   Print sizes
    #
    cmd = env['SIZE'] + ' --format=sysv ' + str(target[0])
    rcode, out, er = utils.pexec(cmd)
    utils.print_size(out)

#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#
#   Builder objects
#
#-------------------------------------------------------------------------------
c2obj      = Builder(action         = compile_c,
                     suffix         = ObjExt,
                     src_suffix     = CExt,
                     source_scanner = CScanner)  
#-------------------------------------------------------------------------------
cpp2obj    = Builder(action         = compile_cpp,
                     suffix         = ObjExt,
                     src_suffix     = CppExt,
                     source_scanner = CScanner)  
#-------------------------------------------------------------------------------
asm2obj    = Builder(action         = assembly_src,
                     suffix         = ObjExt,
                     src_suffix     = AsmExt,
                     source_scanner = CScanner)
#-------------------------------------------------------------------------------
binBuilder = Builder(action     = build_bin,
                     suffix     = BinExt,
                     src_suffix = ObjExt)
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#
#    State variables
#
FirstEntry = False

#-------------------------------------------------------------------------------
#
#    Environment
#

env = Environment(TOOLS = {})

env['BUILDERS'] = {
                     'asmObj' : asm2obj,
                     'cObj'   : c2obj,
                     'cppObj' : cpp2obj,
                     'elf'    : binBuilder
                  }

env['CPP'          ] = CPP
env['ASM'          ] = ASM
env['CC'           ] = CC
env['LINKER'       ] = Linker
env['LOADER'       ] = LOADER
env['OBJDUMP'      ] = OBJDUMP
env['SIZE'         ] = SIZE
env['AFLAGS'       ] = AFLAGS
env['CFLAGS'       ] = CFLAGS
env['CXXFLAGS'     ] = CXXFLAGS
env['GCC_W_FLAGS'  ] = GCC_W_FLAGS
env['LFLAGS'       ] = LFLAGS
env['SRC_PATH'     ] = SourceDirs
env['CPPPATH'      ] = SourceDirs + ToolkitIncPath
env['FIRST_ENTRY'  ] = FirstEntry

#-------------------------------------------------------------------------------
#
#    Service functions
#
#-------------------------------------------------------------------------------
def files_by_ext(l, ext):
    res = []
    for f in l:
        if fnmatch.fnmatch(f, '*.' + ext):
            res.append(f)
            
    return res
#-------------------------------------------------------------------------------
#
#   Make dictionary in form { src : obj } from input source list
#
def make_target_dict(src_list):
    targets = {}

    for i in src_list:
        name_ext = os.path.split(i)[1]
        name     = os.path.splitext(name_ext)[0] + '.' + ObjExt
        targets[i] = ObjDir + '/' + name
        Depends(targets[i],  'SConstruct')      

    return targets

#-------------------------------------------------------------------------------
#
#   Make objects from sources
#
def make_objects(asm, cpp, c = {}):
    obj_list = []

    for i in cpp.items():
        obj_list.append( env.cppObj(i[1], i[0]) )
    for i in asm.items():
        obj_list.append( env.asmObj(i[1], i[0]) )

    for i in c.items():
        obj_list.append( env.cObj(i[1], i[0]) )

    return obj_list

#-------------------------------------------------------------------------------
#
#   Prepare source files list for object building
#
#-------------------------------------------------------------------------------
#
#    C source file list
#
c_files = []
for i in SourceDirs:
    c_files += glob.glob(i + '/*.' + CExt)

c_files += files_by_ext(SourceFiles, 'c')
c = make_target_dict(c_files)

#-------------------------------------------------------------------------------
#
#    Cpp source file list
#
cpp_files = []
for i in SourceDirs:
    cpp_files += glob.glob(i + '/*.' + CppExt)

cpp_files += files_by_ext(SourceFiles, 'cpp')
cpp = make_target_dict(cpp_files)
       
#-------------------------------------------------------------------------------
#
#    Asm source file list
#
asm_files = []
for i in SourceDirs:
    asm_files += glob.glob(i + '/*.' + AsmExt)
    
asm_files += files_by_ext(SourceFiles, 'S')
asm = make_target_dict(asm_files)

#-------------------------------------------------------------------------------
#
#    Objects builing
#
obj_list = make_objects(asm, cpp, c)

#-------------------------------------------------------------------------------
#
#    Executable targets building
#
main_elf   = os.path.join(BinDir, ProjectName)
main_trg   = env.elf( source = obj_list, target = main_elf )
Depends(main_trg, LinkerMainScript + '.S')


#-------------------------------------------------------------------------------
#
#    Clean and Rebuid
#
def remove_files(dir_, mask):
    file_list = glob.glob(dir_ + '/*.' + mask)
    for i in file_list:
        os.remove(i)

#-------------------------------------------------------------------------------
def clean(target, source, env):
    print 'Cleaning targets...',
    if os.path.exists('.sconsign.dblite'): os.remove('.sconsign.dblite')

    remove_files(ObjDir,    ObjExt)
    remove_files(BinDir,    BinExt)
    remove_files(ListDir,   LstExt)
    remove_files(ListDir,   MapExt)
    remove_files(ConfigDir, 'ld')
    remove_files(ScriptDir, 'pyc')
    print 'Done'

#-------------------------------------------------------------------------------
#
#
#
def check_scmRTOS_cfg(target, source, env):
    if env['FIRST_ENTRY'] == False:
        print '*'*SEP_LEN
        print 'Sources are up to date'

    print 'Run integrity checker...',
    rcode = scmIC.checker(env['SRC_PATH'])
    if rcode != 0:
        print '\n scmRTOS configuration is invalid.'

    print '*'*SEP_LEN

#-------------------------------------------------------------------------------
#
#       Targets
#
#-------------------------------------------------------
#
#   Intermediate targets
#
main      = env.Alias(ProjectName, os.path.join(BinDir, ProjectName) + '.' + BinExt)
rtinit    = env.Alias('rtinit', os.path.join(BinDir, 'rtinit') + '.' + BinExt)
clean_all = env.Alias('cln',      action = clean)
ic        = env.Alias('ic',       action = check_scmRTOS_cfg)

all = [main, ic]

#-------------------------------------------------------------------------------
#
#    Final targets
#
env.Alias('all', all)
env.Alias('rebuild', [clean_all, all])

#env.AlwaysBuild('all', 'cln', 'ic')
env.AlwaysBuild('all', 'cln')

Default(all)

#-------------------------------------------------------------------------------
              
#===============================================================================


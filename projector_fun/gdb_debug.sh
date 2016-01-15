#!/bin/bash
#
# todo: abstract stuff out into a separate script
#
project_name="projector_fun"
architecture="linux"
breakpoints_filename="."$project_name"_breakpoints"

bin_dir="../bin/$architecture"

gdb_flags=""
gdb_flags+=" -q "            # skip intro/copyright messages"
gdb_flags+=" -cd=$bin_dir "  # set working directory
# gdb_flags+=" -readnow "      # preload a bunch of stuff


gdb_flags+=" -ix $bin_dir/.gdbcmd "
echo "
source $breakpoints_filename

define savebreaks
    if \$argc == 0
        save breakpoints $breakpoints_filename
    else
        save breakpoints $arg0
    end
end

define print_stdvec
    if \$argc == 0
        print *($arg0._M_impl._M_start)@$arg0.size()
    else
        print *($arg0._M_impl._M_start)@$arg1
    end
end

define pvec
    print (float[])$arg0
end

set print pretty on
" > $bin_dir"/.gdbcmd"

echo "gdb $bin_dir/$project_name $gdb_flags $@"
gdb $project_name $gdb_flags $@
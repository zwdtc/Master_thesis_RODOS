#
# Copyright (C) 2014 DLR.
#
# This file is part of the port for RODOS made by DLR to allow Rodos
# to operate with Xilinx Zynq devices.
#
# Modified based on the FreeRTOS_Zynq files, under
# the terms of the GNU General Public License (version 2) as published by the
# Free Software Foundation
#
# @Author Wei Zhang (wei.zhang@dlr.de or wei.zhang0117@gmail.com)
# @func  associate with the mld file to define the parameters for Rodos, let xilinx SDK recognize and fully support.
#		 please read the xilinx PlatformSpecification Format Reference Manual below in detail.
#  		 https://www.google.de/?gws_rd=ssl#q=Platform+Specification+Format+Reference+Manual+13.4
# @
# @

# standalone bsp version. set this to the latest "ACTIVE" version.
set standalone_version standalone_amp_v3_10_a

proc rodos_drc {os_handle} {

    global env

    set sw_proc_handle [xget_libgen_proc_handle]
    set hw_proc_handle [xget_handle $sw_proc_handle "IPINST"]
    set proctype [xget_value $hw_proc_handle "OPTION" "IPNAME"]

}

proc generate {os_handle} {

    variable standalone_version

    set sw_proc_handle [xget_libgen_proc_handle]
    set hw_proc_handle [xget_handle $sw_proc_handle "IPINST"]
    set proctype [xget_value $hw_proc_handle "OPTION" "IPNAME"]
    
    set need_config_file "false"

	# proctype should be "microblaze" or "ppc405" or "ppc405_virtex4" or "ppc440" or ps7_cortexa9
    set armsrcdir "../${standalone_version}/src/cortexa9"
    set armccdir "../${standalone_version}/src/cortexa9/armcc"
    set ccdir "../${standalone_version}/src/cortexa9/gcc"
	set commonsrcdir "../${standalone_version}/src/common"
    set mbsrcdir "../${standalone_version}/src/microblaze"
    set ppcsrcdir "../${standalone_version}/src/ppc405"
    set ppc440srcdir "../${standalone_version}/src/ppc440"

	foreach entry [glob -nocomplain [file join $commonsrcdir *]] {
		file copy -force $entry [file join ".." "${standalone_version}" "src"]
	}
	
	# proctype should be "ps7_cortexa9"
	switch $proctype {
	
	"ps7_cortexa9"  {
		foreach entry [glob -nocomplain [file join $armsrcdir *]] {
			file copy -force $entry [file join ".." "${standalone_version}" "src"]
		}
		
		foreach entry [glob -nocomplain [file join $ccdir *]] {
					file copy -force $entry [file join ".." "${standalone_version}" "src"]
		}
		
		file delete -force "../${standalone_version}/src/armcc"
        #file delete -force "../${standalone_version}/src/gcc"
		#####might g++, not sure right now.
		file delete -force "../${standalone_version}/src/gcc"
            
		set need_config_file "true"
		
		set file_handle [xopen_include_file "xparameters.h"]
		puts $file_handle "#include \"xparameters_ps.h\""
		puts $file_handle ""
		close $file_handle
		}
		"default" {puts "processor type $proctype not supported\n"}
	}
	
	# Write the Config.make file
	set makeconfig [open "../${standalone_version}/src/config.make" w]
	
	if { $proctype == "ps7_cortexa9" } {
		#####change from .c to .cpp
	    puts $makeconfig "LIBSOURCES = *.c *.S"
	    puts $makeconfig "LIBS = standalone_libs"
	}
	
	close $makeconfig

	# Remove arm directory...
	file delete -force $armsrcdir

	# copy required files to the main src directory
	file copy -force [file join src Source independent globalobjects.cpp] ./src
	file copy -force [file join src Source independent applica.cpp] ./src
	file copy -force [file join src Source independent checksumes.cpp] ./src
	file copy -force [file join src Source bare_metal_generic crt.cpp] ./src
	file copy -force [file join src Source bare_metal_generic cxxabi.cpp] ./src
	file copy -force [file join src Source independent debug.cpp] ./src
	file copy -force [file join src Source independent genericIO.cpp] ./src
	file copy -force [file join src Source independent globaldata.cpp] ./src
	file copy -force [file join src Source independent interruptservers.cpp] ./src
	file copy -force [file join src Source independent listelement.cpp] ./src
	file copy -force [file join src Source independent main.cpp] ./src
	file copy -force [file join src Source independent main_usermain.cpp] ./src
	file copy -force [file join src Source independent miscs.cpp] ./src
	file copy -force [file join src Source independent printfverbosity.cpp] ./src
	file copy -force [file join src Source bare_metal_generic scheduler.cpp] ./src
	file copy -force [file join src Source bare_metal_generic semaphore.cpp] ./src
	file copy -force [file join src Source independent stream-bytesex.cpp] ./src
	#file copy -force [file join src Source string_pico.cpp] ./src
	file copy -force [file join src Source independent subscriber.cpp] ./src
	file copy -force [file join src Source independent swapbytesex.cpp] ./src
	file copy -force [file join src Source independent thread.cpp] ./src
	file copy -force [file join src Source bare_metal_generic thread_on_hw.cpp] ./src
	file copy -force [file join src Source independent timeevent.cpp] ./src
	file copy -force [file join src Source independent timemodel.cpp] ./src
	file copy -force [file join src Source independent timepoints.cpp] ./src
	file copy -force [file join src Source independent topic.cpp] ./src
	file copy -force [file join src Source independent udp.cpp] ./src
	file copy -force [file join src Source independent yprintf.cpp] ./src
	file copy -force [file join src Source independent thread.cpp] ./src
	file copy -force [file join src Source portable hostinfos.cpp] ./src
	file copy -force [file join src Source portable hw_datatypes.h] ./src
	file copy -force [file join src Source portable hw_getchar-dummy.cpp] ./src
	file copy -force [file join src Source portable hw_specific.cpp] ./src
	file copy -force [file join src Source portable hw_stdlib.h] ./src
	#file copy -force [file join src Source portable bcm2835.h] ./src
	#file copy -force [file join src Source portable hal_uart.cpp] ./src
	file copy -force [file join src Source portable hw_timer.cpp] ./src
	#file copy -force [file join src Source portable hw_udp.cpp] ./src
	file copy -force [file join src Source portable hw_udp.h] ./src
	#file copy -force [file join src Source portable params.h] ./src
	file copy -force [file join src Source portable stdint.h] ./src
	file copy -force [file join src Source portable xil_printf2.h] ./src
	#file copy -force [file join src Source portable syscalls.cpp] ./src
	file copy -force [file join src Source portable context_switch.S] ./src
	file copy -force [file join src Source portable startup kernel_init.c] ./src
	file copy -force [file join src Source portable startup vectors.S] ./src
	file copy -force [file join src Source portable multicore.h] ./src
	
	
	set headers [glob -join ./src/Source/include *.\[h\]]
	foreach header $headers {
		file copy -force $header src
	}
	
	file delete -force [file join src Source]
	file delete -force [file join src Source]
	
	# Remove microblaze, ppc405, ppc440, cortexa9 and common directories...
	file delete -force $mbsrcdir
	file delete -force $ppcsrcdir
	file delete -force $ppc440srcdir
	file delete -force $armsrcdir
	file delete -force $ccdir
    file delete -force $commonsrcdir
    file delete -force $armccdir

	# Handle stdin and stdout
	xhandle_stdin $os_handle
	xhandle_stdout $os_handle

    set config_file [xopen_new_include_file "./src/params.h" "Rodos Configuration parameters"]
    puts $config_file "\#include \"xparameters.h\" \n"
	puts $config_file "\#ifndef NO_RODOS_NAMESPACE \n"
	puts $config_file "namespace RODOS \{ \n"
	puts $config_file "\#endif \n"
	
	xput_define $config_file "OSVERSION" "xilinx_zynq"
	
	xput_define $config_file "UART_DEBUG" "UART_IDX0"
	
    

    set XMALLOC_SIZE [xget_value $os_handle "PARAMETER" "XMALLOC_SIZE"]
    xput_define $config_file "XMALLOC_SIZE"     "($XMALLOC_SIZE)"
	
	set DEFAULT_STACKSIZE [xget_value $os_handle "PARAMETER" "DEFAULT_STACKSIZE"]
    xput_define $config_file "DEFAULT_STACKSIZE"     "($DEFAULT_STACKSIZE)"
	
	set PARAM_TIMER_INTERVAL [xget_value $os_handle "PARAMETER" "PARAM_TIMER_INTERVAL"]
    xput_define $config_file "PARAM_TIMER_INTERVAL"     "($PARAM_TIMER_INTERVAL)"
	
	set SCHEDULER_STACKSIZE [xget_value $os_handle "PARAMETER" "SCHEDULER_STACKSIZE"]
    xput_define $config_file "SCHEDULER_STACKSIZE"     "($SCHEDULER_STACKSIZE)"
	
	xput_define $config_file "TIME_SLICE_FOR_SAME_PRIORITY" "100*MILLISECONDS"
	
	set DEFAULT_THREAD_PRIORITY [xget_value $os_handle "PARAMETER" "DEFAULT_THREAD_PRIORITY"]
    xput_define $config_file "DEFAULT_THREAD_PRIORITY"     "($DEFAULT_THREAD_PRIORITY)"
	
	set MAX_THREAD_PRIORITY [xget_value $os_handle "PARAMETER" "MAX_THREAD_PRIORITY"]
    xput_define $config_file "MAX_THREAD_PRIORITY"     "($MAX_THREAD_PRIORITY)"
	
	set NETWORKREADER_PRIORITY [xget_value $os_handle "PARAMETER" "NETWORKREADER_PRIORITY"]
    xput_define $config_file "NETWORKREADER_PRIORITY"     "($NETWORKREADER_PRIORITY)"
	
	set CEILING_PRIORITY [xget_value $os_handle "PARAMETER" "CEILING_PRIORITY"]
    xput_define $config_file "CEILING_PRIORITY"     "($CEILING_PRIORITY)"
	
	set MAX_NUMBER_OF_NODES [xget_value $os_handle "PARAMETER" "MAX_NUMBER_OF_NODES"]
    xput_define $config_file "MAX_NUMBER_OF_NODES"     "($MAX_NUMBER_OF_NODES)"
	
	set MAX_NETWORK_MESSAGE_LENGTH [xget_value $os_handle "PARAMETER" "MAX_NETWORK_MESSAGE_LENGTH"]
    xput_define $config_file "MAX_NETWORK_MESSAGE_LENGTH"     "($MAX_NETWORK_MESSAGE_LENGTH)"
	
	set MAX_SUBSCRIBERS [xget_value $os_handle "PARAMETER" "MAX_SUBSCRIBERS"]
    xput_define $config_file "MAX_SUBSCRIBERS"     "($MAX_SUBSCRIBERS)"
    
	
	puts $config_file "\#ifndef NO_RODOS_NAMESPACE \n"
	puts $config_file " \} \n"
	puts $config_file "\#endif \n"
	

    # complete the header protectors
    puts $config_file "\#endif"
    close $config_file
}

proc xopen_new_include_file { filename description } {
    set inc_file [open $filename w]
    xprint_generated_header $inc_file $description
    set newfname [string map {. _} [lindex [split $filename {\/}] end]]
    puts $inc_file "\#ifndef _[string toupper $newfname]"
    puts $inc_file "\#define _[string toupper $newfname]\n\n"
    return $inc_file
}

proc xput_define { config_file parameter param_value } {
    puts $config_file "#define $parameter $param_value\n"
}

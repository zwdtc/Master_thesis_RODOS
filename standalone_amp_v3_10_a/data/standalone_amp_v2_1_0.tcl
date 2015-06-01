##############################################################################
#
# $Id: standalone_v2_1_0.tcl,v 1.1.2.2 2011/10/24 09:36:46 sadanan Exp $
#
# (c) Copyright 2004-2011 Xilinx, Inc. All rights reserved.
#
# This file contains confidential and proprietary information of Xilinx, Inc.
# and is protected under U.S. and international copyright and other
# intellectual property laws.
#
# DISCLAIMER
# This disclaimer is not a license and does not grant any rights to the
# materials distributed herewith. Except as otherwise provided in a valid
# license issued to you by Xilinx, and to the maximum extent permitted by
# applicable law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL
# FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS,
# IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
# MERCHANTABILITY, NON-INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE;
# and (2) Xilinx shall not be liable (whether in contract or tort, including
# negligence, or under any other theory of liability) for any loss or damage
# of any kind or nature related to, arising under or in connection with these
# materials, including for any direct, or any indirect, special, incidental,
# or consequential loss or damage (including loss of data, profits, goodwill,
# or any type of loss or damage suffered as a result of any action brought by
# a third party) even if such damage or loss was reasonably foreseeable or
# Xilinx had been advised of the possibility of the same.
#
# CRITICAL APPLICATIONS
# Xilinx products are not designed or intended to be fail-safe, or for use in
# any application requiring fail-safe performance, such as life-support or
# safety devices or systems, Class III medical devices, nuclear facilities,
# applications related to the deployment of airbags, or any other applications
# that could lead to death, personal injury, or severe property or
# environmental damage (individually and collectively, "Critical
# Applications"). Customer assumes the sole risk and liability of any use of
# Xilinx products in Critical Applications, subject only to applicable laws
# and regulations governing limitations on product liability.
#
# THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE
# AT ALL TIMES.
#
##############################################################################

# ----------------------------------------------------------------------------
# The following are hardcoded for Zynq.
# We can obtain the scu timer/gic baseaddr from the xml, but other parameters
# need to be hardcoded. hardcode everything..
# ----------------------------------------------------------------------------
set scutimer_baseaddr	0xF8F00600
set scutimer_intr	29
set scugic_cpu_base	0xF8F00100
set scugic_dist_base	0xF8F01000

# --------------------------------------
# Tcl procedure standalone_drc
# -------------------------------------
proc standalone_drc {os_handle} {
}

# --------------------------------------
# Tcl procedure generate
# -------------------------------------
proc generate {os_handle} {

    global env

    set need_config_file "false"

    # Copy over the right set of files as src based on processor type
    set sw_proc_handle [xget_libgen_proc_handle]
    set hw_proc_handle [xget_handle $sw_proc_handle "IPINST"]

    set proctype [xget_value $hw_proc_handle "OPTION" "IPNAME"]
    set procname [xget_value $hw_proc_handle "NAME"]
    set procver [xget_value $hw_proc_handle "PARAMETER" "HW_VER"]

    set enable_sw_profile [xget_value $os_handle "PARAMETER" "enable_sw_intrusive_profiling"]
    set mb_exceptions false

    # proctype should be "microblaze" or "ppc405" or "ppc405_virtex4" or "ppc440" or ps7_cortexa9
    set mbsrcdir "./src/microblaze"
    set ppcsrcdir "./src/ppc405"
    set ppc440srcdir "./src/ppc440"
    set cortexa9srcdir "./src/cortexa9"
    set commonsrcdir "./src/common"

    foreach entry [glob -nocomplain [file join $commonsrcdir *]] {
	file copy -force $entry "./src"
    }

    # detect the relevant PowerPC
    if { [string compare -nocase -length 6 $proctype "ppc440"] == 0 } {
	set proctype "ppc440"
    }

    switch $proctype {
	"microblaze" { 
            foreach entry [glob -nocomplain [file join $mbsrcdir *]] {
		# if { [string first "exception" $entry] == -1 || [mb_has_exceptions $hw_proc_handle] } {
		    # Copy over only files that are not related to exception handling. All such files have exception in their names

		    file copy -force $entry "./src/"
		# }
            }
	    set need_config_file "true"
	    set mb_exceptions [mb_has_exceptions $hw_proc_handle]
	}
	"ppc405"  {
	    foreach entry [glob -nocomplain [file join $ppcsrcdir *]] {
		file copy -force $entry "./src/"
	    }
	}
	"ppc405_virtex4"  {
	    foreach entry [glob -nocomplain [file join $ppcsrcdir *]] {
		file copy -force $entry "./src/"
	    }
	}
	"ppc440"  {
	    foreach entry [glob -nocomplain [file join $ppc440srcdir *]] {
		file copy -force $entry "./src/"
	    }
	}
	"ps7_cortexa9"  {
	    set procdrv [xget_sw_driver_handle_for_ipinst $sw_proc_handle $procname]
	    set compiler [xget_value $procdrv "PARAMETER" "compiler"]
	    if {[string compare -nocase $compiler "armcc"] == 0} {
		set ccdir "./src/cortexa9/armcc"
	    } else {
		set ccdir "./src/cortexa9/gcc"
	    }
	    foreach entry [glob -nocomplain [file join $cortexa9srcdir *]] {
		file copy -force $entry "./src/"
	    }
	    foreach entry [glob -nocomplain [file join $ccdir *]] {
		file copy -force $entry "./src/"
	    }
            file delete -force "./src/armcc"
            file delete -force "./src/gcc"
	    if {[string compare -nocase $compiler "armcc"] == 0} {
                file delete -force "./src/profile"
                set enable_sw_profile "false"
	    }
	    set file_handle [xopen_include_file "xparameters.h"]
	    puts $file_handle "#include \"xparameters_ps.h\""
	    puts $file_handle ""
	    close $file_handle
	}
	"default" {puts "unknown processor type $proctype\n"}
    }

    # Write the Config.make file
    set makeconfig [open "./src/config.make" w]
#    xprint_generated_header_tcl $makeconfig "Configuration parameters for Standalone Makefile"
    if { $proctype == "microblaze" } {
        puts $makeconfig "LIBSOURCES = *.c *.s *.S"
	puts $makeconfig "PROFILE_ARCH_OBJS = profile_mcount_mb.o"
    } elseif { $proctype == "ps7_cortexa9" } {
        if {[string compare -nocase $compiler "armcc"] == 0} {
            puts $makeconfig "LIBSOURCES = *.c *.s"
        } else {
            puts $makeconfig "LIBSOURCES = *.c *.s *.S"
            puts $makeconfig "PROFILE_ARCH_OBJS = profile_mcount_arm.o"
        }
    } else {
	puts $makeconfig "PROFILE_ARCH_OBJS = profile_mcount_ppc.o"
    }
    if { $enable_sw_profile == "true" } {
	puts $makeconfig "LIBS = standalone_libs profile_libs"
    } else {
	puts $makeconfig "LIBS = standalone_libs"
    }
    close $makeconfig
    
    # Remove microblaze, ppc405, ppc440, cortexa9 and common directories...
    file delete -force $mbsrcdir
    file delete -force $ppcsrcdir
    file delete -force $ppc440srcdir
    file delete -force $cortexa9srcdir
    file delete -force $commonsrcdir

    # Handle stdin and stdout
    xhandle_stdin $os_handle
    xhandle_stdout $os_handle
    
    #Handle Profile configuration
    if { $enable_sw_profile == "true" } {
	handle_profile $os_handle $proctype	
    }
    
    set file_handle [xopen_include_file "xparameters.h"]
    puts $file_handle "\n/******************************************************************/\n"
#     if { [string compare -nocase -length 6 $proctype "ppc440"] == 0 \
# 	     || [string compare -nocase -length 6 $proctype "ppc405"] == 0} {
# 	set cache_mask [format "0x%x" [xget_ppc_cache_mask]]
# 	puts $file_handle "#ifdef __PPC__"
# 	puts $file_handle "#define XPPC_CACHEABLE_REGION_MASK $mask"
# 	puts $file_handle "#endif\n"
#     }
    close $file_handle

    # Create config file for microblaze interrupt handling
    if { [string compare -nocase $need_config_file "true"] == 0 } {
	xhandle_mb_interrupts
    }

    # Create config files for Microblaze exception handling
    if { $proctype == "microblaze" && [mb_has_exceptions $hw_proc_handle] } {
        xcreate_mb_exc_config_file $os_handle
    } 

    # Create bspconfig file
    set bspcfg_fn [file join "src" "bspconfig.h"] 
    file delete $bspcfg_fn
    set bspcfg_fh [open $bspcfg_fn w]
    xprint_generated_header $bspcfg_fh "Configurations for Standalone BSP"

    if { $proctype == "microblaze" && [mb_has_pvr $hw_proc_handle] } {
        
        set pvr [xget_value $hw_proc_handle "PARAMETER" "C_PVR"]
        
        switch $pvr {
            "0" {
                puts $bspcfg_fh "#define MICROBLAZE_PVR_NONE"
            }
            "1" {
                puts $bspcfg_fh "#define MICROBLAZE_PVR_BASIC"
            }
            "2" {
                puts $bspcfg_fh "#define MICROBLAZE_PVR_FULL"
            }
            "default" {
                puts $bspcfg_fh "#define MICROBLAZE_PVR_NONE"
            }
        }    
    } else {
        puts $bspcfg_fh "#define MICROBLAZE_PVR_NONE"
    }

    close $bspcfg_fh
}



# --------------------------------------
# Tcl procedure xhandle_mb_interrupts
# --------------------------------------
proc xhandle_mb_interrupts {} {

    set default_interrupt_handler "XNullHandler"
    set default_arg "XNULL"

    set source_interrupt_handler $default_interrupt_handler
    set source_handler_arg $default_arg
    
    # Handle the interrupt pin
    set sw_proc_handle [xget_libgen_proc_handle] 
    set periph [xget_handle $sw_proc_handle "IPINST"]
    set source_ports [xget_interrupt_sources $periph]
    if {[llength $source_ports] > 1} {
	error "Too many interrupting ports on the MicroBlaze. Should only find 1" "" "libgen_error"
	return
    }
    
    if {[llength $source_ports] == 1} {
	set source_port [lindex $source_ports 0]
	if {[llength $source_port] != 0} {
	    set source_port_name [xget_value $source_port "NAME"]	
	    set source_periph [xget_handle $source_port "PARENT"]
	    set source_name [xget_value $source_periph "NAME"]
	    set source_type [xget_hw_value $source_periph]            
 	    set source_driver [xget_sw_driver_handle_for_ipinst $sw_proc_handle $source_name]

	    if {[string compare -nocase $source_driver ""] != 0} {
		set int_array [xget_handle $source_driver "ARRAY" "interrupt_handler"]
		if {[llength $int_array] != 0} {
		    set int_array_elems [xget_handle $int_array "ELEMENTS" "*"]
		    if { [llength $int_array_elems] != 0 } {
			foreach int_array_elem $int_array_elems {
			    set int_port [xget_value $int_array_elem "PARAMETER" "int_port"]
			    if {[llength $int_port] != 0} {
				if {[string compare -nocase $int_port $source_port_name] == 0 } {
				    set source_interrupt_handler [xget_value $int_array_elem "PARAMETER" "int_handler"]
				    set source_handler_arg [xget_value $int_array_elem "PARAMETER" "int_handler_arg"]
				    if {[string compare -nocase $source_handler_arg DEVICE_ID] == 0 } {
					set source_handler_arg [xget_name $source_periph "DEVICE_ID"]
				    } else {
					if {[string compare -nocase "global" [xget_port_type $source_port]] == 0} {
					    set source_handler_arg $default_arg
					} else {
					    set source_handler_arg [xget_name $source_periph "C_BASEADDR"]
					}
				    }
				    break
				}
			    }
			}
		    }
		}
	    } 
	}
    }

    # Generate microblaze_interrupts_g.c file...
    xcreate_mb_intr_config_file $source_interrupt_handler $source_handler_arg
    
}

# -------------------------------------------
# Tcl procedure xcreate_mb_intr_config file
# -------------------------------------------
proc xcreate_mb_intr_config_file {handler arg} {
    
    set mb_table "MB_InterruptVectorTable"

    set filename [file join "src" "microblaze_interrupts_g.c"] 
    file delete $filename
    set config_file [open $filename w]

    xprint_generated_header $config_file "Interrupt Handler Table for MicroBlaze Processor"
    
    puts $config_file "#include \"microblaze_interrupts_i.h\""
    puts $config_file "#include \"xparameters.h\""
    puts $config_file "\n"
    puts $config_file [format "extern void %s (void *);" $handler]
    puts $config_file "\n/*"
    puts $config_file "* The interrupt handler table for microblaze processor"
    puts $config_file "*/\n"
    puts $config_file [format "%sEntry %s\[\] =" $mb_table $mb_table]
    puts $config_file "\{"
    puts -nonewline $config_file [format "\{\t%s" $handler]
    puts -nonewline $config_file [format ",\n\t(void*) %s\}" $arg]
    puts -nonewline $config_file "\n\};"
    puts $config_file "\n"
    close $config_file
}


# -------------------------------------------
# Tcl procedure xcreate_mb_exc_config file
# -------------------------------------------
proc xcreate_mb_exc_config_file {os_handle} {
    
    set hfilename [file join "src" "microblaze_exceptions_g.h"] 
    file delete $hfilename
    set hconfig_file [open $hfilename w]
    xprint_generated_header $hconfig_file "Exception Handling Header for MicroBlaze Processor"
    set sw_proc_handle [xget_libgen_proc_handle]
    set hw_proc_handle [xget_handle $sw_proc_handle "IPINST"]
    set procver [xget_value $hw_proc_handle "PARAMETER" "HW_VER"]
    set interconnect [xget_value $hw_proc_handle "PARAMETER" "C_INTERCONNECT"]
    if { $interconnect == "" || $interconnect == 0 } {
        set ibus_ee [xget_value $hw_proc_handle "PARAMETER" "C_IOPB_BUS_EXCEPTION"]
        set dbus_ee [xget_value $hw_proc_handle "PARAMETER" "C_DOPB_BUS_EXCEPTION"]
    } elseif { $interconnect == 1 } {
        set ibus_ee [xget_value $hw_proc_handle "PARAMETER" "C_IPLB_BUS_EXCEPTION"]
        set dbus_ee [xget_value $hw_proc_handle "PARAMETER" "C_DPLB_BUS_EXCEPTION"]
    } else {
        set ibus_ee [xget_value $hw_proc_handle "PARAMETER" "C_M_AXI_I_BUS_EXCEPTION"]
        set dbus_ee [xget_value $hw_proc_handle "PARAMETER" "C_M_AXI_D_BUS_EXCEPTION"]
    }
    set ill_ee [xget_value $hw_proc_handle "PARAMETER" "C_ILL_OPCODE_EXCEPTION"]
    set unalign_ee [xget_value $hw_proc_handle "PARAMETER" "C_UNALIGNED_EXCEPTIONS"]
    set div0_ee [xget_value $hw_proc_handle "PARAMETER" "C_DIV_ZERO_EXCEPTION"]
    set mmu_ee [xget_value $hw_proc_handle "PARAMETER" "C_USE_MMU"]
    if { $mmu_ee == "" } {
        set mmu_ee 0
    }
    set fsl_ee [xget_value $hw_proc_handle "PARAMETER" "C_FSL_EXCEPTION"]
    if { $fsl_ee == "" } {
        set fsl_ee 0
    }
    if { [mb_has_fpu_exceptions $hw_proc_handle] } {
        set fpu_ee [xget_value $hw_proc_handle "PARAMETER" "C_FPU_EXCEPTION"]
    } else {
        set fpu_ee 0
    }
    set sp_ee [xget_value $hw_proc_handle "PARAMETER" "C_USE_STACK_PROTECTION"]
    if { $sp_ee == "" } {
        set sp_ee 0
    }
    set ft_ee [xget_value $hw_proc_handle "PARAMETER" "C_FAULT_TOLERANT"]
    if { $ft_ee == "" } {
        set ft_ee 0
    }

    if { $ibus_ee == 0 && $dbus_ee == 0 && $ill_ee == 0 && $unalign_ee == 0 
         && $div0_ee == 0 && $fpu_ee == 0 && $mmu_ee == 0 && $fsl_ee == 0 
         && $sp_ee == 0 && $ft_ee == 0} {
        ;# NO exceptions are enabled
        ;# Do not generate any info in either the header or the C file
        close $hconfig_file
        return
    } 

    puts $hconfig_file "\#define MICROBLAZE_EXCEPTIONS_ENABLED 1"
    if { [mb_can_handle_exceptions_in_delay_slots $procver] } {
        puts $hconfig_file "#define MICROBLAZE_CAN_HANDLE_EXCEPTIONS_IN_DELAY_SLOTS"
    }
    if { $unalign_ee == 0 } {
        puts $hconfig_file "\#define NO_UNALIGNED_EXCEPTIONS 1"
    }
    if { $ibus_ee == 0 && $dbus_ee == 0 && $ill_ee == 0 && $div0_ee == 0 
         && $fpu_ee == 0 && $mmu_ee == 0 && $fsl_ee == 0 } { 
        ;# NO other exceptions are enabled
        puts $hconfig_file "\#define NO_OTHER_EXCEPTIONS 1"
    }
        
    if { $fpu_ee != 0 } {
        puts $hconfig_file "\#define MICROBLAZE_FP_EXCEPTION_ENABLED 1"        
        set predecode_fpu_exceptions [xget_value $os_handle "PARAMETER" "predecode_fpu_exceptions"]
        if {$predecode_fpu_exceptions != false } {
            puts $hconfig_file "\#define MICROBLAZE_FP_EXCEPTION_DECODE 1"        
        }   
    }

    puts $hconfig_file "\n"
    close $hconfig_file
}

# --------------------------------------
# Tcl procedure post_generate
#
# This proc removes _interrupt_handler.o
# from libxil.a
# --------------------------------------
proc post_generate {os_handle} {
    
    set sw_proc_handle [xget_libgen_proc_handle]
    set hw_proc_handle [xget_handle $sw_proc_handle "IPINST"]

    set procname [xget_value $hw_proc_handle "NAME"]
    set proctype [xget_value $hw_proc_handle "OPTION" "IPNAME"]

    # detect the right PowerPC
    if { [string compare -nocase -length 6 $proctype "ppc440"] == 0 } {
	set proctype "ppc440"
    }

    if {[string compare -nocase $proctype "microblaze"] == 0} {

 	set procdrv [xget_sw_driver_handle_for_ipinst $sw_proc_handle $procname]
	# Remove _interrupt_handler.o from libxil.a for mb-gcc
	set archiver [xget_value $procdrv "PARAMETER" "archiver"]
	set libxil_a [file join .. .. lib libxil.a]
	exec $archiver -d $libxil_a _interrupt_handler.o

        # Remove _hw_exception_handler.o from libxil.a for microblaze_v3_00_a
        set procver [xget_value $hw_proc_handle "PARAMETER" "HW_VER"]
        if {[mb_has_exceptions $hw_proc_handle]} {
			exec $archiver -d $libxil_a _hw_exception_handler.o
        }
    }

}


# --------------------------------------
# Return true if this MB has 
# exception handling support
# --------------------------------------
proc mb_has_exceptions { hw_proc_handle } {
   
    # Check if the following parameters exist on this MicroBlaze's MPD
    set ee [xget_value $hw_proc_handle "PARAMETER" "C_UNALIGNED_EXCEPTIONS"]
    if { $ee != "" } {
        return true
    }

    set ee [xget_value $hw_proc_handle "PARAMETER" "C_ILL_OPCODE_EXCEPTION"]
    if { $ee != "" } {
        return true
    }

    set ee [xget_value $hw_proc_handle "PARAMETER" "C_IOPB_BUS_EXCEPTION"]
    if { $ee != "" } {
        return true
    }

    set ee [xget_value $hw_proc_handle "PARAMETER" "C_DOPB_BUS_EXCEPTION"]
    if { $ee != "" } {
        return true
    }

    set ee [xget_value $hw_proc_handle "PARAMETER" "C_IPLB_BUS_EXCEPTION"]
    if { $ee != "" } {
        return true
    }

    set ee [xget_value $hw_proc_handle "PARAMETER" "C_DPLB_BUS_EXCEPTION"]
    if { $ee != "" } {
        return true
    }

    set ee [xget_value $hw_proc_handle "PARAMETER" "C_M_AXI_I_BUS_EXCEPTION"]
    if { $ee != "" } {
        return true
    }

    set ee [xget_value $hw_proc_handle "PARAMETER" "C_M_AXI_D_BUS_EXCEPTION"]
    if { $ee != "" } {
        return true
    }

    set ee [xget_value $hw_proc_handle "PARAMETER" "C_DIV_BY_ZERO_EXCEPTION"]
    if { $ee != "" } {
        return true
    } 

    set ee [xget_value $hw_proc_handle "PARAMETER" "C_DIV_ZERO_EXCEPTION"]
    if { $ee != "" } {
        return true
    } 

    set ee [xget_value $hw_proc_handle "PARAMETER" "C_FPU_EXCEPTION"]
    if { $ee != "" } {
        return true
    } 

    set ee [xget_value $hw_proc_handle "PARAMETER" "C_FSL_EXCEPTION"]
    if { $ee != "" } {
        return true
    } 

    set ee [xget_value $hw_proc_handle "PARAMETER" "C_USE_MMU"]
    if { $ee != ""} {
        return true
    } 

    set ee [xget_value $hw_proc_handle "PARAMETER" "C_USE_STACK_PROTECTION"]
    if { $ee != ""} {
        return true
    }

    set ee [xget_value $hw_proc_handle "PARAMETER" "C_FAULT_TOLERANT"]
    if { $ee != ""} {
        return true
    }

    return false
}

# --------------------------------------
# Return true if this MB has 
# FPU exception handling support
# --------------------------------------
proc mb_has_fpu_exceptions { hw_proc_handle } {
    
    # Check if the following parameters exist on this MicroBlaze's MPD
    set ee [xget_value $hw_proc_handle "PARAMETER" "C_FPU_EXCEPTION"]
    if { $ee != "" } {
        return true
    } 

    return false
}

# --------------------------------------
# Return true if this MB has PVR support
# --------------------------------------
proc mb_has_pvr { hw_proc_handle } {
    
    # Check if the following parameters exist on this MicroBlaze's MPD
    set pvr [xget_value $hw_proc_handle "PARAMETER" "C_PVR"]
    if { $pvr != "" } {
        return true
    } 

    return false
}

# --------------------------------------
# Return true if MB ver 'procver' has 
# support for handling exceptions in 
# delay slots
# --------------------------------------
proc mb_can_handle_exceptions_in_delay_slots { procver } {
    
    if { [string compare -nocase $procver "5.00.a"] >= 0 } {
        return true
    } else {
        return false
    }
}


# --------------------------------------
# Generate Profile Configuration
# --------------------------------------
proc handle_profile { os_handle proctype } {
    global env
    variable scutimer_baseaddr
    variable scutimer_intr
    variable scugic_cpu_base
    variable scugic_dist_base

    set proc [xget_processor]

    if {$proctype == "ps7_cortexa9"} {
        set sw_proc_handle [xget_libgen_proc_handle]
        set hw_proc_handle [xget_handle $sw_proc_handle "IPINST"]
	set cpu_freq [xget_value $hw_proc_handle "PARAMETER" "C_CPU_CLK_FREQ_HZ"]
	if { [string compare -nocase $cpu_freq ""] == 0 } {
	    puts "WARNING<profile> :: CPU Clk Frequency not specified, Assuming 666Mhz"
	    set cpu_freq 666000000
	}
    } else {
	set cpu_freq [xget_value $proc "PARAMETER" "CORE_CLOCK_FREQ_HZ"]
	if { [string compare -nocase $cpu_freq ""] == 0 } {
	    puts "WARNING<profile> :: CPU Clk Frequency not specified, Assuming 100Mhz"
	    set cpu_freq 100000000
	}
    }

    set filename [file join "src" "profile" "profile_config.h"]
    file delete -force $filename
    set config_file [open $filename w]

    xprint_generated_header $config_file "Profiling Configuration parameters. These parameters
* can be overwritten thru run configuration in SDK"
    puts $config_file "#ifndef _PROFILE_CONFIG_H"
    puts $config_file "#define _PROFILE_CONFIG_H\n"
    
    puts $config_file "#define BINSIZE 4"
    puts $config_file "#define CPU_FREQ_HZ $cpu_freq"
    puts $config_file "#define SAMPLE_FREQ_HZ 100000"
    puts $config_file "#define TIMER_CLK_TICKS [expr $cpu_freq / 100000]"
    
    # proctype should be "microblaze" or "ppc405" or "ppc405_virtex4" or "ppc440" or "ps7_cortexa9"
    switch $proctype {
	"microblaze" { 
	    # Microblaze Processor.

	    puts $config_file "#define PROC_MICROBLAZE 1"
	    set timer_inst [xget_value $os_handle "PARAMETER" "profile_timer"]
	    if { [string compare -nocase $timer_inst "none"] == 0 } {
		# Profile Timer Not Selected
		error "ERROR :: Timer for Profiling NOT selected.\nS/W Intrusive Profiling on MicroBlaze requires an xps_timer or an opb_timer." "" "mdt_error"
	    } else {
		handle_profile_opbtimer $config_file $timer_inst
	    }
	}
	"ppc405" -
	"ppc405_virtex4" {
	    # PowerPC Processor
	    # - PIT Timer is used for Profiling by default

	    puts $config_file "#define PROC_PPC 1"
	    puts $config_file "#define PROC_PPC405 1"
	    set timer_inst [xget_value $os_handle "PARAMETER" "profile_timer"]
	    if { [string compare -nocase $timer_inst "none"] == 0 } {
		# PIT Timer
		puts $config_file "#define PPC_PIT_INTERRUPT 1"
		puts $config_file "#define ENABLE_SYS_INTR 1"
	    } else {
		handle_profile_opbtimer $config_file $timer_inst
	    }
	}
	"ppc440" {

	   # similar to ppc405

	    puts $config_file "#define PROC_PPC 1"
	    puts $config_file "#define PROC_PPC440 1"
	    set timer_inst [xget_value $os_handle "PARAMETER" "profile_timer"]
	    if { [string compare -nocase $timer_inst "none"] == 0 } {
		# PIT Timer
		puts $config_file "#define PPC_PIT_INTERRUPT 1"
		puts $config_file "#define ENABLE_SYS_INTR 1"
	    } else {
		handle_profile_opbtimer $config_file $timer_inst
	    }
	}

	"ps7_cortexa9" { 
	    # Cortex A9 Processor.

	    puts $config_file "#define PROC_CORTEXA9 1"
	    set timer_inst [xget_value $os_handle "PARAMETER" "profile_timer"]
	    if { [string compare -nocase $timer_inst "none"] == 0 } {
		# SCU Timer
		puts $config_file "#define ENABLE_SCU_TIMER 1"
		puts $config_file "#define ENABLE_SYS_INTR 1"
		puts $config_file "#define PROFILE_TIMER_BASEADDR $scutimer_baseaddr"
		puts $config_file "#define PROFILE_TIMER_INTR_ID $scutimer_intr"
		puts $config_file "#define SCUGIC_CPU_BASEADDR $scugic_cpu_base"
		puts $config_file "#define SCUGIC_DIST_BASEADDR $scugic_dist_base"
            }
	}

	"default" {error "unknown processor type\n"}
    }

    puts $config_file "\n#endif"
    puts $config_file "\n/******************************************************************/\n"
    close $config_file
}

# - The xps/opb_timer can be connected directly to Microblaze External Intr Pin.
# - (OR) xps/opb_timer can be connected to xps/opb_intc
proc handle_profile_opbtimer { config_file timer_inst } {
    set timer_handle [xget_hwhandle $timer_inst]
    set timer_baseaddr [xget_value $timer_handle "PARAMETER" "C_BASEADDR"]
    puts $config_file "#define PROFILE_TIMER_BASEADDR [xformat_addr_string $timer_baseaddr "C_BASEADDR"]"

    # Figure out how Timer is connected.
    set timer_intr [xget_value $timer_handle "PORT" "Interrupt"]
    if { [string compare -nocase $timer_intr ""] == 0 } {
	error "ERROR<profile> :: Timer Interrupt PORT is not specified" "" "mdt_error"
    } 
    set mhs_handle [xget_handle $timer_handle "parent"]
    # CR 302300 - There can be multiple "sink" for the interrupt. So need to iterate through the list
    set intr_port_list [xget_connected_ports_handle $mhs_handle $timer_intr "sink"]
    set timer_connection 0
    foreach intr_port $intr_port_list {
	set intc_handle [xget_handle $intr_port "parent"]
	# Check if the Sink is a Global Port. If so, Skip the Port Connection
	if { $intc_handle == $mhs_handle } {
	    continue 
	}
	set iptype [xget_value $intc_handle "OPTION" "IPTYPE"]
	if { [string compare -nocase $iptype "PROCESSOR"] == 0 } {
	    # Timer Directly Connected to the Processor
	    puts $config_file "#define ENABLE_SYS_INTR 1"
	    set timer_connection 1
	    break
	}

	set ipsptype [xget_value $intc_handle "OPTION" "SPECIAL"]
	if { [string compare -nocase $iptype "PERIPHERAL"] == 0  &&
	     [string compare -nocase $ipsptype "INTERRUPT_CONTROLLER"] == 0 } {
	    # Timer connected to Interrupt controller
	    puts $config_file "#define TIMER_CONNECT_INTC 1"
	    puts $config_file "#define INTC_BASEADDR [xget_value $intc_handle "PARAMETER" "C_BASEADDR"]"
	    set num_intr_inputs [xget_value $intc_handle "PARAMETER" "C_NUM_INTR_INPUTS"]
	    # if { $num_intr_inputs == 1 } {  ## Always enable system interrupt CR 472288
		 puts $config_file "#define ENABLE_SYS_INTR 1"
	    # }

	    set signals [split [xget_value $intr_port "VALUE"] "&"]
	    set i 1
	    foreach signal $signals {
		set signal [string trim $signal]
		if {[string compare -nocase $signal $timer_intr] == 0} {
		    set timer_id [expr ($num_intr_inputs - $i)]
		    set timer_mask [expr 0x1 << $timer_id]
		    puts $config_file "#define PROFILE_TIMER_INTR_ID $timer_id"
		    puts $config_file "#define PROFILE_TIMER_INTR_MASK [format "0x%x" $timer_mask]"
		    break
		}
		incr i
	    }
	    set timer_connection 1
	    break
	} 
    }
    
    if { $timer_connection == 0 } {
	error "ERROR<profile> :: Profile Timer Interrupt Signal Not Connected Properly" 
    }
}

#***--------------------------------***-----------------------------------***
# Utility process to call a command and pipe it's output to screen.
# Used instead of Tcl's exec
proc execpipe {COMMAND} {

  if { [catch {open "| $COMMAND 2>@stdout"} FILEHANDLE] } {
    return "Can't open pipe for '$COMMAND'"
  }

  set PIPE $FILEHANDLE
  fconfigure $PIPE -buffering none

  set OUTPUT ""

  while { [gets $PIPE DATA] >= 0 } {
    puts $DATA
    append OUTPUT $DATA "\n"
  }

  if { [catch {close $PIPE} ERRORMSG] } {

    if { [string compare "$ERRORMSG" "child process exited abnormally"] == 0 } {
      # this error means there was nothing on stderr (which makes sense) and
      # there was a non-zero exit code - this is OK as we intentionally send
      # stderr to stdout, so we just do nothing here (and return the output)
    } else {
      return "Error '$ERRORMSG' on closing pipe for '$COMMAND'"
    }

  }

  regsub -all -- "\n$" $OUTPUT "" STRIPPED_STRING
  return "$STRIPPED_STRING"

}

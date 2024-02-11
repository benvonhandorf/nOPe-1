set pagination off
set logging file gdb.output
set logging on

define hook_continue

end

define unwind_stack
    set $exec_frame = ($lr * 0x4) ? $psp : $msp
    set $stacked_xpsr = ((uint32_t *)$exec_frame)[7]
    set $exec_frame_len = 32 + (($stacked_xpsr ? 0x4 : 0x0) + (($lr & 0x10) ? 0 : 72))
    
end

# Breakpoint on the Hardfault Handler
break cortex_m_rt::HardFault_
    backtrace
    info registers
    set $is_psp_in_use = $lr&(1 << 2)
    if $is_psp_in_use
        echo "PSP in use"
        
    end
    if !$is_psp_in_use
        echo "MSP in use"
        x/4xw $msp
    end
end

echo "Hardfault Handler breakpoint set"
set pagination off
set logging file gdb.output
set logging on

define hook_continue

end

define unwind_stack

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
.DATA
.CODE

public syscaller_stub
 
syscaller_stub proc
    mov r10, rcx
    pop rcx
    pop rax
    mov QWORD PTR [rsp], rcx
    mov eax, [rsp + 24]
    syscall
    sub rsp, 8
    jmp QWORD PTR [rsp + 8]
syscaller_stub endp

END

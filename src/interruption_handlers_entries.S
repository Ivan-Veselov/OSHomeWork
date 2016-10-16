.text
.code64
.global interruption_handlers_tbl
.global interruption_handler

#define MakeHandler(id) \
handler##id: \
  pushq $0; /* error code */ \
  pushq $id; \
  jmp call_c_func

#define MakeHandlerWithErr(id) \
handler##id: \
  pushq $id; \
  jmp call_c_func

MakeHandler(0)
MakeHandler(1)
MakeHandler(2)
MakeHandler(3)
MakeHandler(4)
MakeHandler(5)
MakeHandler(6)
MakeHandler(7)
MakeHandlerWithErr(8)
MakeHandler(9)
MakeHandlerWithErr(10)
MakeHandlerWithErr(11)
MakeHandlerWithErr(12)
MakeHandlerWithErr(13)
MakeHandlerWithErr(14)
MakeHandler(15)
MakeHandler(16)
MakeHandlerWithErr(17)
MakeHandler(18)
MakeHandler(19)
MakeHandler(20)
MakeHandler(21)
MakeHandler(22)
MakeHandler(23)
MakeHandler(24)
MakeHandler(25)
MakeHandler(26)
MakeHandler(27)
MakeHandler(28)
MakeHandler(29)
MakeHandler(30)
MakeHandler(31)
MakeHandler(32)
MakeHandler(33)
MakeHandler(34)
MakeHandler(35)
MakeHandler(36)
MakeHandler(37)
MakeHandler(38)
MakeHandler(39)
MakeHandler(40)
MakeHandler(41)
MakeHandler(42)
MakeHandler(43)
MakeHandler(44)
MakeHandler(45)
MakeHandler(46)
MakeHandler(47)

call_c_func:
  pushq %rdi
  
  // passing argument
  movq 8(%rsp), %rdi

  pushq %rax
  pushq %rcx
  pushq %rdx
  pushq %rsi
  pushq %r8
  pushq %r9
  pushq %r10
  pushq %r11
  
  cld
  call interruption_handler
  
  popq %r11
  popq %r10
  popq %r9
  popq %r8
  popq %rsi
  popq %rdx
  popq %rcx
  popq %rax
  
  popq %rdi
  
  addq $16, %rsp
  iretq

interruption_handlers_tbl:
  .quad handler0
  .quad handler1
  .quad handler2
  .quad handler3
  .quad handler4
  .quad handler5
  .quad handler6
  .quad handler7
  .quad handler8
  .quad handler9
  .quad handler10
  .quad handler11
  .quad handler12
  .quad handler13
  .quad handler14
  .quad handler15
  .quad handler16
  .quad handler17
  .quad handler18
  .quad handler19
  .quad handler20
  .quad handler21
  .quad handler22
  .quad handler23
  .quad handler24
  .quad handler25
  .quad handler26
  .quad handler27
  .quad handler28
  .quad handler29
  .quad handler30
  .quad handler31
  .quad handler32
  .quad handler33
  .quad handler34
  .quad handler35
  .quad handler36
  .quad handler37
  .quad handler38
  .quad handler39
  .quad handler40
  .quad handler41
  .quad handler42
  .quad handler43
  .quad handler44
  .quad handler45
  .quad handler46
  .quad handler47

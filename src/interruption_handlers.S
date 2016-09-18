.text
.code64
.global interruption_handlers_tbl

handler0:
  label0:
    jmp label0;

interruption_handlers_tbl:
  .quad handler0
